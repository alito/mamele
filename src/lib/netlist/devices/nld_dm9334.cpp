// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
/*
 * nld_dm9334.cpp
 *
 */

#include "nld_dm9334.h"
#include "netlist/nl_base.h"

namespace netlist
{
	namespace devices
	{
	NETLIB_OBJECT(9334)
	{
		NETLIB_CONSTRUCTOR(9334)
		, m_CQ(*this, "CQ", NETLIB_DELEGATE(inputs))
		, m_EQ(*this, "EQ", NETLIB_DELEGATE(inputs))
		, m_D(*this, "D", NETLIB_DELEGATE(inputs))
		, m_A(*this, {"A0", "A1", "A2"}, NETLIB_DELEGATE(inputs))
		, m_Q(*this, {"Q0", "Q1", "Q2", "Q3", "Q4", "Q5", "Q6", "Q7"})
		, m_last_CQ(*this, "m_last_CQ", 0)
		, m_last_EQ(*this, "m_last_EQ", 0)
		, m_last_D(*this, "m_last_D", 0)
		, m_last_A(*this, "m_last_A", 0)
		, m_last_Q(*this, "m_last_Q", 0)
		, m_power_pins(*this)
		{
		}

		NETLIB_RESETI()
		{
			m_last_CQ = 0;
			m_last_EQ = 0;
			m_last_D = 0;
			m_last_A = 0;
			m_last_Q = 0;
		}

		friend class NETLIB_NAME(9334_dip);
	private:
		NETLIB_HANDLERI(inputs)
		{
			uint_fast8_t a = 0;
			for (std::size_t i=0; i<3; i++)
			{
				a |= (m_A[i]() << i);
			}

			netlist_time delay = NLTIME_FROM_NS(27); // Clear Low to High Level Output (not documented, making reasonable guess)

			if (a != m_last_A)
			{
				delay = NLTIME_FROM_NS(35);
			}
			else if (m_D() != m_last_D)
			{
				if (m_last_D)
				{
					delay = NLTIME_FROM_NS(28);
				}
				else
				{
					delay = NLTIME_FROM_NS(35);
				}
			}
			else if (m_EQ() != m_last_EQ)
			{
				if (m_last_EQ)
				{
					delay = NLTIME_FROM_NS(27);
				}
				else
				{
					delay = NLTIME_FROM_NS(28);
				}
			}

			unsigned q = m_last_Q;

			if (!m_CQ())
			{
				if (m_EQ())
				{
					q = 0;
				}
				else
				{
					q = m_D() << a;
				}
			}
			else if(!m_EQ())
			{
				q &= ~(1 << a);
				q |= (m_D() << a);
			}

			m_last_CQ = m_CQ();
			m_last_EQ = m_EQ();
			m_last_D = m_D();
			m_last_A = a;
			m_last_Q = q;

			for (std::size_t i=0; i<8; i++)
				m_Q[i].push((q >> i) & 1, delay);
		}

		logic_input_t m_CQ;
		logic_input_t m_EQ;
		logic_input_t m_D;
		object_array_t<logic_input_t, 3> m_A;
		object_array_t<logic_output_t, 8> m_Q;

		state_var<unsigned> m_last_CQ;
		state_var<unsigned> m_last_EQ;
		state_var<unsigned> m_last_D;
		state_var<unsigned> m_last_A;
		state_var<unsigned> m_last_Q;
		nld_power_pins m_power_pins;
	};

	NETLIB_OBJECT(9334_dip)
	{
		NETLIB_CONSTRUCTOR(9334_dip)
		, A(*this, "A")
		{
			register_subalias("1", A.m_A[0]);
			register_subalias("2", A.m_A[1]);
			register_subalias("3", A.m_A[2]);
			register_subalias("4", A.m_Q[0]);
			register_subalias("5", A.m_Q[1]);
			register_subalias("6", A.m_Q[2]);
			register_subalias("7", A.m_Q[3]);
			register_subalias("8", "A.GND");

			register_subalias("9",  A.m_Q[4]);
			register_subalias("10", A.m_Q[5]);
			register_subalias("11", A.m_Q[6]);
			register_subalias("12", A.m_Q[7]);
			register_subalias("13", A.m_D);
			register_subalias("14", A.m_EQ);
			register_subalias("15", A.m_CQ);
			register_subalias("16", "A.VCC");

		}
		//NETLIB_RESETI() {}
	private:
		NETLIB_SUB(9334) A;
	};

	NETLIB_DEVICE_IMPL(9334,     "TTL_9334",     "+CQ,+EQ,+D,+A0,+A1,+A2,@VCC,@GND")
	NETLIB_DEVICE_IMPL(9334_dip, "TTL_9334_DIP", "")

	} //namespace devices
} // namespace netlist
