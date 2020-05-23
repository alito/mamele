// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_truthtable.h
 *
 */

#ifndef NLID_TRUTHTABLE_H_
#define NLID_TRUTHTABLE_H_

#include "netlist/devices/nlid_system.h"
#include "netlist/nl_base.h"
#include "netlist/nl_setup.h"
#include "plib/putil.h"

#define USE_TT_ALTERNATIVE (0)

namespace netlist
{
namespace devices
{

	template<unsigned bits>
	struct need_bytes_for_bits
	{
		enum { value =
			bits <= 8       ?   1 :
			bits <= 16      ?   2 :
			bits <= 32      ?   4 :
								8
		};
	};

	template<unsigned bits> struct uint_for_size;
	template<> struct uint_for_size<1> { using type = uint_least8_t; };
	template<> struct uint_for_size<2> { using type = uint_least16_t; };
	template<> struct uint_for_size<4> { using type = uint_least32_t; };
	template<> struct uint_for_size<8> { using type = uint_least64_t; };

	template<std::size_t m_NI, std::size_t m_NO>
	NETLIB_OBJECT(truthtable_t)
	{
	private:
		detail::family_setter_t m_fam;
	public:

		using type_t = typename uint_for_size<need_bytes_for_bits<m_NO + m_NI>::value>::type;

		static constexpr const std::size_t m_num_bits = m_NI;
		static constexpr const std::size_t m_size = (1 << (m_num_bits));
		static constexpr const type_t m_outmask = ((1 << m_NO) - 1);

		struct truthtable_t
		{
			truthtable_t()
			: m_timing_index{0}
			{}

			std::array<type_t, m_size> m_out_state;
			std::array<uint_least8_t, m_size * m_NO> m_timing_index;
			std::array<netlist_time, 16> m_timing_nt;
		};

		template <class C>
		nld_truthtable_t(C &owner, const pstring &name,
				const logic_family_desc_t &fam,
				truthtable_t &ttp, const std::vector<pstring> &desc)
		: device_t(owner, name)
		, m_fam(*this, fam)
#if USE_TT_ALTERNATIVE
		, m_state(*this, "m_state", 0)
#endif
		, m_ign(*this, "m_ign", 0)
		, m_ttp(ttp)
		/* FIXME: the family should provide the names of the power-terminals! */
		, m_power_pins(*this)
		{
			init(desc);
		}

		void init(const std::vector<pstring> &desc);

		NETLIB_RESETI()
		{
			int active_outputs = 0;
			m_ign = 0;
#if USE_TT_ALTERNATIVE
			m_state = 0;
#endif
			for (std::size_t i = 0; i < m_NI; ++i)
			{
				m_I[i].activate();
#if USE_TT_ALTERNATIVE
				m_state |= (m_I[i]() << i);
#endif
			}
			for (auto &q : m_Q)
				if (q.has_net() && q.net().has_connections())
					active_outputs++;
			set_active_outputs(active_outputs);
		}

		// update is only called during startup here ...
		NETLIB_UPDATEI()
		{
#if USE_TT_ALTERNATIVE
			m_state = 0;
			for (std::size_t i = 0; i < m_NI; ++i)
			{
				m_state |= (m_I[i]() << i);
			}
#endif
			process<true>();
		}

#if USE_TT_ALTERNATIVE
		template <std::size_t N>
		void update_N() noexcept
		{
			m_state &= ~(1<<N);
			m_state |= (m_I[N]() << N);
			process<true>();
		}
#endif

		void inc_active() noexcept override
		{
			process<false>();
		}

		void dec_active() noexcept override
		{
			for (std::size_t i = 0; i< m_NI; i++)
				m_I[i].inactivate();
			m_ign = (1<<m_NI)-1;
		}

	protected:

	private:

		template<bool doOUT>
		void process() noexcept
		{
			netlist_time_ext mt(netlist_time_ext::zero());
			type_t nstate(0);
			type_t ign(m_ign);

			if (doOUT)
			{
#if !USE_TT_ALTERNATIVE
				for (auto I = m_I.begin(); ign != 0; ign >>= 1, ++I)
					if (ign & 1)
						I->activate();
				for (std::size_t i = 0; i < m_NI; i++)
					nstate |= (m_I[i]() << i);
#else
				nstate = m_state;
				for (std::size_t i = 0; ign != 0; ign >>= 1, ++i)
				{
					if (ign & 1)
					{
						nstate &= ~(1 << i);
						m_I[i].activate();
						nstate |= (m_I[i]() << i);
					}
				}
#endif
			}
			else
				for (std::size_t i = 0; i < m_NI; i++)
				{
					m_I[i].activate();
					nstate |= (m_I[i]() << i);
					mt = std::max(this->m_I[i].net().next_scheduled_time(), mt);
				}

			const type_t outstate(m_ttp.m_out_state[nstate]);
			type_t out(outstate & m_outmask);

			m_ign = outstate >> m_NO;

			const std::size_t timebase(nstate * m_NO);
			const auto *t(&m_ttp.m_timing_index[timebase]);
			const auto *tim = m_ttp.m_timing_nt.data();

			if (doOUT)
				for (std::size_t i = 0; i < m_NO; out >>= 1, ++i)
					m_Q[i].push(out & 1, tim[t[i]]);
			else
				for (std::size_t i = 0; i < m_NO; out >>= 1, ++i)
					m_Q[i].set_Q_time(out & 1, mt + tim[t[i]]);

			ign = m_ign;
			for (auto I = m_I.begin(); ign != 0; ign >>= 1, ++I)
				if (ign & 1)
					I->inactivate();
#if USE_TT_ALTERNATIVE
			m_state = nstate;
#endif
		}

		plib::uninitialised_array_t<logic_input_t, m_NI> m_I;
		plib::uninitialised_array_t<logic_output_t, m_NO> m_Q;

		/* FIXME: check width */
#if USE_TT_ALTERNATIVE
		state_var<type_t>   m_state;
#endif
		state_var<type_t>   m_ign;
		const truthtable_t &m_ttp;
		/* FIXME: the family should provide the names of the power-terminals! */
		nld_power_pins m_power_pins;
	};

} // namespace devices

namespace factory
{
	class truthtable_base_element_t : public factory::element_t
	{
	public:
		truthtable_base_element_t(const pstring &name, const pstring &classname,
				const pstring &def_param, const pstring &sourcefile);

		std::vector<pstring> m_desc;
		pstring m_family_name;
		const logic_family_desc_t *m_family_desc;
	};

	// FIXME: the returned element is missing a pointer to the family ...
	plib::unique_ptr<truthtable_base_element_t> truthtable_create(tt_desc &desc, const pstring &sourcefile);

} // namespace factory
} // namespace netlist



#endif // NLID_TRUTHTABLE_H_
