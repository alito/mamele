// license:GPL-2.0+
// copyright-holders:Couriersud

#ifndef PMEMPOOL_H_
#define PMEMPOOL_H_

///
/// \file pmempool.h
///

#include "palloc.h"
#include "pconfig.h"
#include "pstream.h"
#include "pstring.h"
#include "ptypes.h"
#include "putil.h"

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace plib {

	//============================================================
	//  Memory pool
	//============================================================

	class mempool
	{
	public:

		using size_type = std::size_t;

		static constexpr const bool is_stateless = false;


		template <class T, size_type ALIGN = alignof(T)>
		using allocator_type = arena_allocator<mempool, T, ALIGN>;

		mempool(size_t min_alloc = (1<<21), size_t min_align = PALIGN_CACHELINE)
		: m_min_alloc(min_alloc)
		, m_min_align(min_align)
		, m_stat_cur_alloc(0)
		, m_stat_max_alloc(0)
		{
		}

		COPYASSIGNMOVE(mempool, delete)

		~mempool()
		{

			for (auto & b : m_blocks)
			{
				if (b->m_num_alloc != 0)
				{
					plib::perrlogger("Found {} info blocks\n", sinfo().size());
					plib::perrlogger("Found block with {} dangling allocations\n", b->m_num_alloc);
				}
				aligned_arena::free(b);
				//::operator delete(b->m_data);
			}
		}
#if 0
		static inline mempool &instance()
		{
			static mempool s_mempool;
			return s_mempool;
		}
#endif
		void *allocate(size_t align, size_t size)
		{
			block *b = nullptr;

			if (align < m_min_align)
				align = m_min_align;

			size_t rs = size + align;
			for (auto &bs : m_blocks)
			{
				if (bs->m_free > rs)
				{
					b = bs;
					break;
				}
			}
			if (b == nullptr)
			{
				b = new_block(rs);
			}
			b->m_free -= rs;
			b->m_num_alloc++;
			void *ret = reinterpret_cast<void *>(b->m_data + b->m_cur);
			auto capacity(rs);
			ret = std::align(align, size, ret, capacity);
			sinfo().insert({ ret, info(b, b->m_cur)});
			rs -= (capacity - size);
			b->m_cur += rs;
			m_stat_cur_alloc += size;
			m_stat_max_alloc = std::max(m_stat_max_alloc, m_stat_cur_alloc);

			return ret;
		}

		static void deallocate(void *ptr, size_t size)
		{

			auto it = sinfo().find(ptr);
			if (it == sinfo().end())
				plib::terminate("mempool::free - pointer not found");
			block *b = it->second.m_block;
			if (b->m_num_alloc == 0)
				plib::terminate("mempool::free - double free was called");
			else
			{
				mempool &mp = b->m_mempool;
				b->m_num_alloc--;
				mp.m_stat_cur_alloc -= size;
				//printf("Freeing in block %p %lu\n", b, b->m_num_alloc);
				if (b->m_num_alloc == 0)
				{
					auto itb = std::find(mp.m_blocks.begin(), mp.m_blocks.end(), b);
					if (itb == mp.m_blocks.end())
						plib::terminate("mempool::free - block not found");

					mp.m_blocks.erase(itb);
					aligned_arena::free(b);
				}
				sinfo().erase(it);
			}
		}

		template <typename T>
		using owned_pool_ptr = plib::owned_ptr<T, arena_deleter<mempool, T>>;

		template <typename T>
		using unique_pool_ptr = std::unique_ptr<T, arena_deleter<mempool, T>>;

		template<typename T, typename... Args>
		owned_pool_ptr<T> make_owned(Args&&... args)
		{
			auto *mem = this->allocate(alignof(T), sizeof(T));
			try
			{
				auto *mema = new (mem) T(std::forward<Args>(args)...);
				return owned_pool_ptr<T>(mema, true, arena_deleter<mempool, T>(this));
			}
			catch (...)
			{
				this->deallocate(mem, sizeof(T));
				throw;
			}
		}

		template<typename T, typename... Args>
		unique_pool_ptr<T> make_unique(Args&&... args)
		{
			auto *mem = this->allocate(alignof(T), sizeof(T));
			try
			{
				auto *mema = new (mem) T(std::forward<Args>(args)...);
				return unique_pool_ptr<T>(mema, arena_deleter<mempool, T>(this));
			}
			catch (...)
			{
				this->deallocate(mem, sizeof(T));
				throw;
			}
		}

		size_type cur_alloc() const noexcept { return m_stat_cur_alloc; }
		size_type max_alloc() const noexcept { return m_stat_max_alloc; }

		bool operator ==(const mempool &rhs) const noexcept { return this == &rhs; }

	private:
		struct block
		{
			block(mempool &mp, size_type min_bytes)
			: m_num_alloc(0)
			, m_cur(0)
			, m_data(nullptr)
			, m_mempool(mp)
			{
				min_bytes = std::max(mp.m_min_alloc, min_bytes);
				m_free = min_bytes;
				size_type alloc_bytes = (min_bytes + mp.m_min_align); // - 1); // & ~(mp.m_min_align - 1);
				//m_data_allocated = ::operator new(alloc_bytes);
				m_data_allocated = new char[alloc_bytes];
				void *r = m_data_allocated;
				std::align(mp.m_min_align, min_bytes, r, alloc_bytes);
				m_data  = reinterpret_cast<char *>(r);
			}
			~block()
			{
				//::operator delete(m_data_allocated);
				delete [] m_data_allocated;
			}

			block(const block &) = delete;
			block(block &&) = delete;
			block &operator =(const block &) = delete;
			block &operator =(block &&) = delete;

			size_type m_num_alloc;
			size_type m_free;
			size_type m_cur;
			char *m_data;
			char *m_data_allocated;
			mempool &m_mempool;
		};

		struct info
		{
			info(block *b, size_type p) : m_block(b), m_pos(p) { }
			~info() = default;
			COPYASSIGNMOVE(info, default)

			block * m_block;
			size_type m_pos;
		};

		block * new_block(size_type min_bytes)
		{
			auto *b = aligned_arena::alloc<block>(*this, min_bytes);
			m_blocks.push_back(b);
			return b;
		}

		static std::unordered_map<void *, info> &sinfo()
		{
			static std::unordered_map<void *, info> spinfo;
			return spinfo;
		}

		size_t m_min_alloc;
		size_t m_min_align;

		std::vector<block *> m_blocks;

		size_t m_stat_cur_alloc;
		size_t m_stat_max_alloc;
	};

} // namespace plib

#endif // PMEMPOOL_H_
