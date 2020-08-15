// license:GPL-2.0+
// copyright-holders:Couriersud

///
/// \file param.h
///

#ifndef NL_CORE_PARAM_H_
#define NL_CORE_PARAM_H_

#if 0
#include "../nl_config.h"
#include "../nl_factory.h"
#include "../nl_setup.h"

#include "../plib/ppreprocessor.h"

#include <initializer_list>
#include <stack>
#include <unordered_map>
#include <vector>
#endif

#include "../nltypes.h"

#include "base_objects.h"

#include "../plib/pstream.h"
#include "../plib/pstring.h"
#include "../plib/palloc.h"

#include <memory>

namespace netlist
{
	/// @brief Base class for all device parameters
	///
	/// All device parameters classes derive from this object.
	class param_t : public detail::device_object_t
	{
	public:

		enum param_type_t {
			STRING,
			DOUBLE,
			INTEGER,
			LOGIC,
			POINTER // Special-case which is always initialized at MAME startup time
		};

		//deviceless, it's the responsibility of the owner to register!
		param_t(const pstring &name);

		param_t(core_device_t &device, const pstring &name);

		PCOPYASSIGNMOVE(param_t, delete)
		virtual ~param_t() noexcept;

		param_type_t param_type() const noexcept(false);

		virtual pstring valstr() const = 0;

	protected:

		void update_param() noexcept;

		pstring get_initial(const core_device_t *dev, bool *found) const;

		template<typename C>
		void set_and_update_param(C &p, const C v) noexcept
		{
			if (p != v)
			{
				p = v;
				update_param();
			}
		}

	};

	// -----------------------------------------------------------------------------
	// numeric parameter template
	// -----------------------------------------------------------------------------

	template <typename T>
	class param_num_t final: public param_t
	{
	public:
		using value_type = T;

		param_num_t(core_device_t &device, const pstring &name, T val) noexcept(false);

		T operator()() const noexcept { return m_param; }
		operator T() const noexcept { return m_param; }

		void set(const T &param) noexcept { set_and_update_param(m_param, param); }

		pstring valstr() const override
		{
			return plib::pfmt("{}").e(gsl::narrow<nl_fptype>(m_param));
		}

	private:
		T m_param;
	};

	template <typename T>
	class param_enum_t final: public param_t
	{
	public:
		using value_type = T;

		param_enum_t(core_device_t &device, const pstring &name, T val) noexcept(false);

		T operator()() const noexcept { return m_param; }
		operator T() const noexcept { return m_param; }
		void set(const T &param) noexcept { set_and_update_param(m_param, param); }

		pstring valstr() const override
		{
			// returns the numerical value
			return plib::pfmt("{}")(static_cast<int>(m_param));
		}
	private:
		T m_param;
	};

	// FIXME: these should go as well
	using param_logic_t = param_num_t<bool>;
	using param_int_t = param_num_t<int>;
	using param_fp_t = param_num_t<nl_fptype>;

	// -----------------------------------------------------------------------------
	// pointer parameter
	// -----------------------------------------------------------------------------

	// FIXME: not a core component -> legacy
	class param_ptr_t final: public param_t
	{
	public:
		param_ptr_t(core_device_t &device, const pstring &name, std::uint8_t* val);
		std::uint8_t * operator()() const noexcept { return m_param; }
		void set(std::uint8_t *param) noexcept { set_and_update_param(m_param, param); }

		pstring valstr() const override
		{
			// returns something which errors
			return pstring("PTRERROR");
		}

	private:
		std::uint8_t* m_param;
	};

	// -----------------------------------------------------------------------------
	// string parameter
	// -----------------------------------------------------------------------------

	class param_str_t : public param_t
	{
	public:
		param_str_t(core_device_t &device, const pstring &name, const pstring &val);
		param_str_t(netlist_state_t &state, const pstring &name, const pstring &val);

		pstring operator()() const noexcept { return str(); }
		void set(const pstring &param)
		{
			if (*m_param != param)
			{
				*m_param = param;
				changed();
				update_param();
			}
		}
		pstring valstr() const override
		{
			return *m_param;
		}
	protected:
		virtual void changed() noexcept;
		pstring str() const noexcept { return *m_param; }
	private:
		host_arena::unique_ptr<pstring> m_param;
	};

	// -----------------------------------------------------------------------------
	// model parameter
	// -----------------------------------------------------------------------------

	class param_model_t : public param_str_t
	{
	public:

		template <typename T>
		class value_base_t
		{
		public:
			template <typename P, typename Y=T, typename DUMMY = std::enable_if_t<plib::is_arithmetic<Y>::value>>
			value_base_t(P &param, const pstring &name)
			: m_value(gsl::narrow<T>(param.value(name)))
			{
			}
			template <typename P, typename Y=T, std::enable_if_t<!plib::is_arithmetic<Y>::value, int> = 0>
			value_base_t(P &param, const pstring &name)
			: m_value(static_cast<T>(param.value_str(name)))
			{
			}
			T operator()() const noexcept { return m_value; }
			operator T() const noexcept { return m_value; }
		private:
			const T m_value;
		};

		using value_t = value_base_t<nl_fptype>;
		using value_str_t = value_base_t<pstring>;

		param_model_t(core_device_t &device, const pstring &name, const pstring &val)
		: param_str_t(device, name, val)
		{
		}

		pstring value_str(const pstring &entity);
		nl_fptype value(const pstring &entity);
		pstring type();
		// hide this
		void set(const pstring &param) = delete;
	protected:
		void changed() noexcept override;
	private:
	};

	// -----------------------------------------------------------------------------
	// data parameter
	// -----------------------------------------------------------------------------

	class param_data_t : public param_str_t
	{
	public:
		param_data_t(core_device_t &device, const pstring &name)
		: param_str_t(device, name, "")
		{
		}

		std::unique_ptr<std::istream> stream();
	protected:
		void changed() noexcept override { }
	};

	// -----------------------------------------------------------------------------
	// rom parameter
	// -----------------------------------------------------------------------------

	template <typename ST, std::size_t AW, std::size_t DW>
	class param_rom_t final: public param_data_t
	{
	public:

		param_rom_t(core_device_t &device, const pstring &name);

		const ST & operator[] (std::size_t n) const noexcept { return m_data[n]; }
	protected:
		void changed() noexcept override
		{
			plib::istream_read(*stream(), m_data.data(), 1<<AW);
		}

	private:
		std::array<ST, 1 << AW> m_data;
	};


} // namespace netlist


#endif // NL_CORE_PARAM_H_
