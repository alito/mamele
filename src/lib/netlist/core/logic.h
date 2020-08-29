// license:GPL-2.0+
// copyright-holders:Couriersud

///
/// \file param.h
///

#ifndef NL_CORE_LOGIC_H_
#define NL_CORE_LOGIC_H_

#include "../nltypes.h"
#include "base_objects.h"
#include "logic_family.h"
#include "nets.h"
#include "state_var.h"

#include "../plib/plists.h"
#include "../plib/pstring.h"

#include <array>
#include <utility>

namespace netlist
{
	// -----------------------------------------------------------------------------
	// logic_t
	// -----------------------------------------------------------------------------

	class logic_t : public detail::core_terminal_t, public logic_family_t
	{
	public:
		logic_t(device_t &dev, const pstring &aname,
				state_e terminal_state, nldelegate delegate);

		logic_net_t & net() noexcept
		{
			return plib::downcast<logic_net_t &>(core_terminal_t::net());
		}
		const logic_net_t &  net() const noexcept
		{
			return plib::downcast<const logic_net_t &>(core_terminal_t::net());
		}
	};

	// -----------------------------------------------------------------------------
	// logic_input_t
	// -----------------------------------------------------------------------------

	class logic_input_t : public logic_t
	{
	public:
		logic_input_t(device_t &dev, const pstring &aname,
				nldelegate delegate);

		inline netlist_sig_t operator()() const noexcept
		{
			nl_assert(terminal_state() != STATE_INP_PASSIVE);
	#if NL_USE_COPY_INSTEAD_OF_REFERENCE
			return m_Q;
	#else
			return net().Q();
	#endif
		}

		void inactivate() noexcept
		{
			if (!is_state(STATE_INP_PASSIVE))
			{
				set_state(STATE_INP_PASSIVE);
				net().remove_from_active_list(*this);
			}
		}

		void activate() noexcept
		{
			if (is_state(STATE_INP_PASSIVE))
			{
				net().add_to_active_list(*this);
				set_state(STATE_INP_ACTIVE);
			}
		}

		void activate_hl() noexcept
		{
			if (is_state(STATE_INP_PASSIVE))
			{
				net().add_to_active_list(*this);
				set_state(STATE_INP_HL);
			}
		}

		void activate_lh() noexcept
		{
			if (is_state(STATE_INP_PASSIVE))
			{
				net().add_to_active_list(*this);
				set_state(STATE_INP_LH);
			}
		}
	};

	// -----------------------------------------------------------------------------
	// logic_output_t
	// -----------------------------------------------------------------------------

	class logic_output_t : public logic_t
	{
	public:

		/// \brief logic output constructor
		///
		/// The third parameter does nothing. It is provided only for
		/// compatibility with tristate_output_t in templatized device models
		///
		/// \param dev Device owning this output
		/// \param aname The name of this output
		/// \param dummy Dummy parameter to allow construction like tristate output
		///
		logic_output_t(device_t &dev, const pstring &aname, bool dummy = false);

		void initial(netlist_sig_t val) noexcept;

		inline void push(const netlist_sig_t &newQ, const netlist_time &delay) noexcept
		{
			m_my_net.set_Q_and_push(newQ, delay); // take the shortcut
		}

		inline void set_Q_time(const netlist_sig_t &newQ, const netlist_time_ext &at) noexcept
		{
			m_my_net.set_Q_time(newQ, at); // take the shortcut
		}

		/// \brief Dummy implementation for templatized generic devices
		///
		/// This function shall never be called. It is defined here so that
		/// templatized generic device models do not have to do tons of
		/// template magic.
		///
		/// This function terminates if actually called.
		///
		[[noreturn]] static void set_tristate(netlist_sig_t v,
			netlist_time ts_off_on, netlist_time ts_on_off)
		{
			plib::unused_var(v, ts_off_on, ts_on_off);
			plib::terminate("set_tristate on logic_output should never be called!");
		}
	private:
		logic_net_t m_my_net;
	};

	// -----------------------------------------------------------------------------
	// tristate_output_t
	// -----------------------------------------------------------------------------

	/// \brief Tristate output
	///
	/// In a lot of applications tristate enable inputs are just connected to
	/// VCC/GND to permanently enable the outputs. In this case a pure
	/// implementation using analog outputs would not perform well.
	///
	/// For this object during creation it can be decided if a logic output or
	/// a tristate output is used. Generally the owning device uses parameter
	/// FORCE_TRISTATE_LOGIC to determine this.
	///
	/// This is the preferred way to implement tristate outputs.
	///

	class tristate_output_t : public logic_output_t
	{
	public:

		tristate_output_t(device_t &dev, const pstring &aname, bool force_logic);

		void push(netlist_sig_t newQ, netlist_time delay) noexcept
		{
			if (!m_tristate)
				logic_output_t::push(newQ, delay);
			m_last_logic = newQ;
		}

		void set_tristate(netlist_sig_t v,
			netlist_time ts_off_on, netlist_time ts_on_off) noexcept
		{
			if (!m_force_logic)
				if (v != m_tristate)
				{
					logic_output_t::push((v != 0) ? OUT_TRISTATE() : m_last_logic, v ? ts_off_on : ts_on_off);
					m_tristate = v;
				}
		}

		bool is_force_logic() const noexcept
		{
			return m_force_logic;
		}

	private:
		using logic_output_t::initial;
		using logic_output_t::set_Q_time;
		state_var<netlist_sig_t> m_last_logic;
		state_var<netlist_sig_t> m_tristate;
		bool m_force_logic;
	};

} // namespace netlist


#endif // NL_CORE_LOGIC_H_
