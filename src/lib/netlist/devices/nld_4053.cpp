// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_4053.cpp
 *
 *  CD4053: Triple 2-Channel Analog Multiplexer/Demultiplexer
 *
 *          +--------------+
 *  INOUTBY |1     ++    16| VDD
 *  INOUTBX |2           15| OUTINB
 *  INOUTCY |3           14| OUTINA
 *   OUTINC |4    4053   13| INOUTAY
 *  INOUTCX |5           12| INOUTAX
 *      INH |6           11| A
 *      VEE |7           10| B
 *      VSS |8            9| C
 *          +--------------+
 *
 *  FIXME: These devices are slow (~125 ns). This is currently not reflected
 *
 *  Naming conventions follow National semiconductor datasheet
 *
 */


#include "nld_4053.h"

#include "netlist/analog/nlid_twoterm.h"
#include "netlist/solver/nld_solver.h"

namespace netlist
{
	namespace devices
	{
	NETLIB_OBJECT(CD4053_GATE)
	{
		NETLIB_CONSTRUCTOR_MODEL(CD4053_GATE, "CD4XXX")
		, m_RX(*this, "RX")
		, m_RY(*this, "RY")
		, m_select(*this, "S", NETLIB_DELEGATE(controls))
		, m_inhibit(*this, "INH", NETLIB_DELEGATE(controls))
		, m_VEE(*this, "VEE", NETLIB_DELEGATE(controls))
		, m_base_r(*this, "BASER", nlconst::magic(270.0))
		, m_lastx(*this, "m_lastx", false)
		, m_lasty(*this, "m_lasty", false)
		, m_select_state(*this, "m_select_state", false)
		, m_inhibit_state(*this, "m_inhibit_state", false)
		, m_supply(*this)
		{
			connect(m_RX.N(), m_RY.N());
			register_subalias("X", m_RX.P());
			register_subalias("Y", m_RY.P());
			register_subalias("XY", m_RX.N());
		}

		NETLIB_RESETI()
		{
			update_state(true, false);
		}

	private:
		NETLIB_HANDLERI(controls)
		{
			bool newx = false, newy = false;
			if (!on(m_inhibit, m_inhibit_state))
			{
				if (!on(m_select, m_select_state))
				{
					newx = true;
				}
				else
				{
					newy = true;
				}
			}

			if (newx != m_lastx || newy != m_lasty)
				update_state(newx, newy);
		}

		bool on(analog_input_t &input, bool &state)
		{
			// digital inputs are based on VDD
			nl_fptype sup = (m_supply.VCC().Q_Analog() - m_supply.GND().Q_Analog());
			nl_fptype in = input() - m_supply.GND().Q_Analog();
			nl_fptype low = nlconst::magic(0.3) * sup;
			nl_fptype high = nlconst::magic(0.7) * sup;
			if (in < low)
			{
				state = false;
			}
			else if (in > high)
			{
				state = true;
			}
			return state;
		}

		void update_state(bool newx, bool newy)
		{
			// analog output is based on VEE
			const nl_fptype sup = (m_VEE() - m_supply.GND().Q_Analog());
			const nl_fptype Ron = m_base_r() * nlconst::magic(5.0) / sup;
			const nl_fptype Roff = plib::reciprocal(exec().gmin());
			const nl_fptype RX = newx ? Ron : Roff;
			const nl_fptype RY = newy ? Ron : Roff;
			if (m_RX.solver() == m_RY.solver())
			{
				m_RX.change_state([this, &RX, &RY]()
				{
					m_RX.set_R(RX);
					m_RY.set_R(RY);
				});
			}
			else
			{
				m_RX.change_state([this, &RX]()
				{
					m_RX.set_R(RX);
				});
				m_RY.change_state([this, &RY]()
				{
					m_RY.set_R(RY);
				});
			}
			m_lastx = newx;
			m_lasty = newy;
		}

		analog::NETLIB_SUB(R_base) m_RX;
		analog::NETLIB_SUB(R_base) m_RY;
		analog_input_t             m_select;
		analog_input_t             m_inhibit;
		analog_input_t             m_VEE;
		param_fp_t                 m_base_r;
		state_var<bool>            m_lastx;
		state_var<bool>            m_lasty;
		state_var<bool>            m_select_state;
		state_var<bool>            m_inhibit_state;
		nld_power_pins             m_supply;
	};

	NETLIB_DEVICE_IMPL(CD4053_GATE,         "CD4053_GATE",            "")
	} //namespace devices
} // namespace netlist
