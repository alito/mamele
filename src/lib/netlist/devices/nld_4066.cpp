// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_4066.c
 *
 */

#include "nld_4066.h"

#include "netlist/analog/nlid_twoterm.h"
#include "netlist/solver/nld_solver.h"
#include "nlid_system.h"

namespace netlist
{
	namespace devices
	{
	NETLIB_OBJECT(CD4066_GATE)
	{
		NETLIB_CONSTRUCTOR(CD4066_GATE)
		NETLIB_FAMILY("CD4XXX")
		, m_supply(*this, "VDD", "VSS")
		, m_R(*this, "R")
		, m_control(*this, "CTL")
		, m_base_r(*this, "BASER", nlconst::magic(270.0))
		, m_last(*this, "m_last", false)
		{
		}

		NETLIB_RESETI();
		NETLIB_UPDATEI();

	private:
		nld_power_pins             m_supply;
		analog::NETLIB_SUB(R_base) m_R;

		analog_input_t             m_control;
		param_fp_t                 m_base_r;
		state_var<bool>            m_last;
	};

	NETLIB_RESET(CD4066_GATE)
	{
		// Start in off condition
		// FIXME: is ROFF correct?
		m_R.set_R(plib::reciprocal(exec().gmin()));

	}

	NETLIB_UPDATE(CD4066_GATE)
	{
		nl_fptype sup = (m_supply.VCC().Q_Analog() - m_supply.GND().Q_Analog());
		nl_fptype low = nlconst::magic(0.45) * sup;
		nl_fptype high = nlconst::magic(0.55) * sup;
		nl_fptype in = m_control() - m_supply.GND().Q_Analog();
		nl_fptype rON = m_base_r() * nlconst::magic(5.0) / sup;
		nl_fptype R = -nlconst::one();
		bool new_state(false);

		if (in < low)
		{
			R = plib::reciprocal(exec().gmin());
		}
		else if (in > high)
		{
			R = rON;
			new_state = true;
		}
		//printf("%s %f %f %g\n", name().c_str(), sup, in, R);
		if (R > nlconst::zero() && (m_last != new_state))
		{
			m_last = new_state;
			m_R.update();
			m_R.set_R(R);
			m_R.solve_later();
		}
	}

	NETLIB_DEVICE_IMPL(CD4066_GATE,         "CD4066_GATE",            "")

	} //namespace devices
} // namespace netlist
