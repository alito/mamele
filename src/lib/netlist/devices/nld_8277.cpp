// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
/*
 * nld_8277.cpp
 *
 *  8277: Dual 8-Bit Shift Register
 *
 *          +--------------+
 *    RESET |1     ++    16| VCC
 *     /Q7A |2           15| /Q7B
 *      Q7A |3           14| Q7B
 *      DSA |4    8277   13| DSB
 *      D1A |5           12| D1B
 *      D0A |6           11| D0B
 *     CLKA |7           10| CLKB
 *      GND |8            9| CLK
 *          +--------------+
 *
 */

#include "nld_8277.h"
#include "netlist/nl_base.h"

namespace netlist
{
	namespace devices
	{
	NETLIB_OBJECT(8277_shifter)
	{
		NETLIB_CONSTRUCTOR(8277_shifter)
		, m_D0(*this, "D0", NETLIB_DELEGATE(inputs))
		, m_D1(*this, "D1", NETLIB_DELEGATE(inputs))
		, m_DS(*this, "DS", NETLIB_DELEGATE(inputs))
		, m_buffer(*this, "m_buffer", 0)
		, m_Q7(*this, "Q7")
		, m_Q7Q(*this, "Q7Q")
		, m_power_pins(*this)
		{
		}

	public:
		void reset_shifter() noexcept
		{
			m_buffer = 0;
			m_Q7.push(0, NLTIME_FROM_NS(40));
			m_Q7Q.push(1, NLTIME_FROM_NS(40));
		}

		void shift() noexcept
		{
			uint32_t in = (m_DS() ? m_D1() : m_D0());
			m_buffer <<= 1;
			m_buffer |= in;
			uint32_t out = (m_buffer >> 7) & 1;
			uint32_t outq = (~m_buffer >> 7) & 1;
			m_Q7.push(out, NLTIME_FROM_NS(40));
			m_Q7Q.push(outq, NLTIME_FROM_NS(40));
		}

		logic_input_t m_D0;
		logic_input_t m_D1;
		logic_input_t m_DS;

		state_var<uint8_t> m_buffer;

		logic_output_t m_Q7;
		logic_output_t m_Q7Q;
		nld_power_pins m_power_pins;
	private:
		NETLIB_HANDLERI(inputs)
		{
			/* do nothing */
		}
	};

	NETLIB_OBJECT(8277)
	{
		NETLIB_CONSTRUCTOR(8277)
		, m_A(*this, "A")
		, m_B(*this, "B")
		, m_RESET(*this, "RESET", NETLIB_DELEGATE(shifter_reset))
		, m_CLK(*this, "CLK", NETLIB_DELEGATE(clk))
		, m_CLKA(*this, "CLKA", NETLIB_DELEGATE(clka))
		, m_CLKB(*this, "CLKB", NETLIB_DELEGATE(clkb))
		, m_last_CLK(*this, "m_last_CLK", 0)
		, m_last_CLKA(*this, "m_last_CLKA", 0)
		, m_last_CLKB(*this, "m_last_CLKB", 0)
		// FIXME: needs family!
		{
			register_subalias("RESET", m_RESET);
			register_subalias("CLK", m_CLK);
			register_subalias("CLKA", m_CLKA);
			register_subalias("CLKB", m_CLKB);
			register_subalias("D0A", m_A.m_D0);
			register_subalias("D1A", m_A.m_D1);
			register_subalias("DSA", m_A.m_DS);
			register_subalias("Q7A", m_A.m_Q7);
			register_subalias("Q7QA", m_A.m_Q7Q);
			register_subalias("D0B", m_B.m_D0);
			register_subalias("D1B", m_B.m_D1);
			register_subalias("DSB", m_B.m_DS);
			register_subalias("Q7B", m_B.m_Q7);
			register_subalias("Q7QB", m_B.m_Q7Q);

			connect("A.VCC", "B.VCC");
			connect("A.GND", "B.GND");

			register_subalias("VCC", "A.VCC");
			register_subalias("GND", "A.GND");
		}

		NETLIB_RESETI()
		{
			m_last_CLK = 0;
			m_last_CLKA = 0;
			m_last_CLKB = 0;
		}

		friend class NETLIB_NAME(8277_dip);
	private:
		NETLIB_HANDLERI(shifter_reset)
		{
			if (!m_RESET())
			{
				m_A.reset_shifter();
				m_B.reset_shifter();
			}
		}

		NETLIB_HANDLERI(clk)
		{
			if (!m_last_CLK && m_CLK())
			{
				m_A.shift();
				m_B.shift();
			}
			m_last_CLK = m_CLK();
			m_last_CLKA = m_CLKA();
			m_last_CLKB = m_CLKB();
		}

		NETLIB_HANDLERI(clka)
		{
			if (!m_last_CLKA && m_CLKA())
			{
				m_A.shift();
			}
			m_last_CLKA = m_CLKA();
		}

		NETLIB_HANDLERI(clkb)
		{
			if (!m_last_CLKB && m_CLKB())
			{
				m_B.shift();
			}
			m_last_CLKB = m_CLKB();
		}

		NETLIB_SUB(8277_shifter) m_A;
		NETLIB_SUB(8277_shifter) m_B;
		logic_input_t m_RESET;
		logic_input_t m_CLK;
		logic_input_t m_CLKA;
		logic_input_t m_CLKB;

		state_var<uint32_t> m_last_CLK;
		state_var<uint32_t> m_last_CLKA;
		state_var<uint32_t> m_last_CLKB;
	};

	NETLIB_OBJECT(8277_dip)
	{
		NETLIB_CONSTRUCTOR(8277_dip)
		, A(*this, "A")
		{
			register_subalias("1", A.m_RESET);
			register_subalias("2", A.m_A.m_Q7Q);
			register_subalias("3", A.m_A.m_Q7);
			register_subalias("4", A.m_A.m_DS);
			register_subalias("5", A.m_A.m_D1);
			register_subalias("6", A.m_A.m_D0);
			register_subalias("7", A.m_CLKA);
			register_subalias("8", "A.GND");

			register_subalias("9",  A.m_CLK);
			register_subalias("10", A.m_CLKB);
			register_subalias("11", A.m_B.m_D0);
			register_subalias("12", A.m_B.m_D1);
			register_subalias("13", A.m_B.m_DS);
			register_subalias("14", A.m_B.m_Q7);
			register_subalias("15", A.m_B.m_Q7Q);
			register_subalias("16", "A.VCC");
		}
		//NETLIB_RESETI() {}
	private:
		NETLIB_SUB(8277) A;
	};

	NETLIB_DEVICE_IMPL(8277,     "TTL_8277",     "+RESET,+CLK,+CLKA,+D0A,+D1A,+DSA,+CLKB,+D0B,+D1B,+DSB,@VCC,@GND")
	NETLIB_DEVICE_IMPL(8277_dip, "TTL_8277_DIP", "")

	} //namespace devices
} // namespace netlist
