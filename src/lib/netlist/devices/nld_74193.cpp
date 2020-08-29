// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_74193.cpp
 *
 *  DM74193: Synchronous 4-Bit Binary Counter with Dual Clock
 *
 *          +--------------+
 *        B |1     ++    16| VCC
 *       QB |2           15| A
 *       QA |3           14| CLEAR
 *       CD |4    74193  13| BORROWQ
 *       CU |5           12| CARRYQ
 *       QC |6           11| LOADQ
 *       QD |7           10| C
 *      GND |8            9| D
 *          +--------------+
 *
 * CD: Count up
 * CU: Count down
 *
 *  Naming conventions follow National Semiconductor datasheet
 *
 */

#include "nld_74193.h"
#include "netlist/nl_base.h"

namespace netlist
{
	namespace devices
	{

	static constexpr const unsigned MAXCNT = 15;

	// FIXME: Timing, combine with 74192!
	static constexpr const std::array<netlist_time, 4> delay =
	{
			NLTIME_FROM_NS(40),
			NLTIME_FROM_NS(40),
			NLTIME_FROM_NS(40),
			NLTIME_FROM_NS(40)
	};

	// FIXME: Optimize!

	NETLIB_OBJECT(74193)
	{
		NETLIB_CONSTRUCTOR(74193)
		, m_A(*this, "A", NETLIB_DELEGATE(inputs))
		, m_B(*this, "B", NETLIB_DELEGATE(inputs))
		, m_C(*this, "C", NETLIB_DELEGATE(inputs))
		, m_D(*this, "D", NETLIB_DELEGATE(inputs))
		, m_CLEAR(*this, "CLEAR", NETLIB_DELEGATE(inputs))
		, m_LOADQ(*this, "LOADQ", NETLIB_DELEGATE(inputs))
		, m_CU(*this, "CU", NETLIB_DELEGATE(inputs))
		, m_CD(*this, "CD", NETLIB_DELEGATE(inputs))
		, m_cnt(*this, "m_cnt", 0)
		, m_last_CU(*this, "m_last_CU", 0)
		, m_last_CD(*this, "m_last_CD", 0)
		, m_Q(*this, {"QA", "QB", "QC", "QD"})
		, m_BORROWQ(*this, "BORROWQ")
		, m_CARRYQ(*this, "CARRYQ")
		, m_power_pins(*this)
		{
		}

	private:
		NETLIB_RESETI()
		{
			m_cnt = 0;
			m_last_CU = 0;
			m_last_CD = 0;
		}

		NETLIB_HANDLERI(inputs)
		{
			netlist_sig_t tCarry = 1;
			netlist_sig_t tBorrow = 1;
			if (m_CLEAR())
			{
				m_cnt = 0;
			}
			else if (!m_LOADQ())
			{
				m_cnt = (m_D() << 3) | (m_C() << 2)
						| (m_B() << 1) | (m_A() << 0);
			}
			else
			{
				if (m_CD() && !m_last_CU && m_CU())
				{
					++m_cnt &= MAXCNT;
				}
				if (m_CU() && !m_last_CD && m_CD())
				{
					if (m_cnt > 0)
						--m_cnt;
					else
						m_cnt = MAXCNT;
				}
			}

			if (!m_CU() && (m_cnt == MAXCNT))
				tCarry = 0;

			if (!m_CD() && (m_cnt == 0))
						tBorrow = 0;

			m_last_CD = m_CD();
			m_last_CU = m_CU();

			for (std::size_t i=0; i<4; i++)
				m_Q[i].push((m_cnt >> i) & 1, delay[i]);

			m_BORROWQ.push(tBorrow, NLTIME_FROM_NS(20)); //FIXME timing
			m_CARRYQ.push(tCarry, NLTIME_FROM_NS(20)); //FIXME timing
		}

		logic_input_t m_A;
		logic_input_t m_B;
		logic_input_t m_C;
		logic_input_t m_D;
		logic_input_t m_CLEAR;
		logic_input_t m_LOADQ;
		logic_input_t m_CU;
		logic_input_t m_CD;

		state_var<unsigned> m_cnt;
		state_var<unsigned> m_last_CU;
		state_var<unsigned> m_last_CD;

		object_array_t<logic_output_t, 4> m_Q;
		logic_output_t m_BORROWQ;
		logic_output_t m_CARRYQ;
		nld_power_pins m_power_pins;
	};

	NETLIB_DEVICE_IMPL(74193,     "TTL_74193", "+A,+B,+C,+D,+CLEAR,+LOADQ,+CU,+CD,@VCC,@GND")

	} //namespace devices
} // namespace netlist
