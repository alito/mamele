// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_74174.cpp
 *
 */

#include "nld_74174.h"
#include "netlist/nl_base.h"

namespace netlist
{
namespace devices
{
	NETLIB_OBJECT(74174)
	{
		NETLIB_CONSTRUCTOR(74174)
		, m_CLK(*this, "CLK", NETLIB_DELEGATE(clk))
		, m_Q(*this, {"Q1", "Q2", "Q3", "Q4", "Q5", "Q6"})
		, m_clrq(*this, "m_clr", 0)
		, m_data(*this, "m_data", 0)
		, m_D(*this, {"D1", "D2", "D3", "D4", "D5", "D6"}, NETLIB_DELEGATE(other))
		, m_CLRQ(*this, "CLRQ", NETLIB_DELEGATE(other))
		, m_power_pins(*this)
		{
		}

		NETLIB_RESETI()
		{
			m_CLK.set_state(logic_t::STATE_INP_LH);
			m_clrq = 0;
			m_data = 0xFF;
		}

		NETLIB_HANDLERI(other)
		{
			uint_fast8_t d = 0;
			for (std::size_t i=0; i<6; i++)
			{
				d |= (m_D[i]() << i);
			}
			m_clrq = m_CLRQ();
			if (!m_clrq)
			{
				for (std::size_t i=0; i<6; i++)
				{
					m_Q[i].push(0, NLTIME_FROM_NS(40));
				}
				m_data = 0;
			} else if (d != m_data)
			{
				m_data = d;
				m_CLK.activate_lh();
			}
		}

		NETLIB_HANDLERI(clk)
		{
			if (m_clrq)
			{
				for (std::size_t i=0; i<6; i++)
				{
					netlist_sig_t d = (m_data >> i) & 1;
					m_Q[i].push(d, NLTIME_FROM_NS(25));
				}
				m_CLK.inactivate();
			}
		}

		friend class NETLIB_NAME(74174_dip);
	private:
		logic_input_t m_CLK;
		object_array_t<logic_output_t, 6> m_Q;

		state_var<netlist_sig_t> m_clrq;
		state_var<unsigned>      m_data;

		object_array_t<logic_input_t, 6> m_D;
		logic_input_t m_CLRQ;
		nld_power_pins m_power_pins;
	};

	NETLIB_OBJECT(74174_dip)
	{
		NETLIB_CONSTRUCTOR(74174_dip)
		, A(*this, "A")
		{
			register_subalias("1",  A.m_CLRQ);
			register_subalias("9",  A.m_CLK);

			register_subalias("3",  A.m_D[0]);
			register_subalias("2",  A.m_Q[0]);

			register_subalias("4",  A.m_D[1]);
			register_subalias("5",  A.m_Q[1]);

			register_subalias("6",  A.m_D[2]);
			register_subalias("7",  A.m_Q[2]);

			register_subalias("11", A.m_D[3]);
			register_subalias("10", A.m_Q[3]);

			register_subalias("13", A.m_D[4]);
			register_subalias("12", A.m_Q[4]);

			register_subalias("14", A.m_D[5]);
			register_subalias("15", A.m_Q[5]);

			register_subalias("8", "A.GND");
			register_subalias("16", "A.VCC");
		}
		//NETLIB_RESETI() {}
	private:
		NETLIB_SUB(74174) A;
	};

	NETLIB_DEVICE_IMPL(74174,   "TTL_74174", "+CLK,+D1,+D2,+D3,+D4,+D5,+D6,+CLRQ,@VCC,@GND")
	NETLIB_DEVICE_IMPL(74174_dip,"TTL_74174_DIP",          "")

	} //namespace devices
} // namespace netlist
