// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_7493.cpp
 *
 *  DM7493: Binary Counters
 *
 *          +--------------+
 *        B |1     ++    14| A
 *      R01 |2           13| NC
 *      R02 |3           12| QA
 *       NC |4    7493   11| QD
 *      VCC |5           10| GND
 *       NC |6            9| QB
 *       NC |7            8| QC
 *          +--------------+
 *
 *          Counter Sequence
 *
 *          +-------++----+----+----+----+
 *          | COUNT || QD | QC | QB | QA |
 *          +=======++====+====+====+====+
 *          |    0  ||  0 |  0 |  0 |  0 |
 *          |    1  ||  0 |  0 |  0 |  1 |
 *          |    2  ||  0 |  0 |  1 |  0 |
 *          |    3  ||  0 |  0 |  1 |  1 |
 *          |    4  ||  0 |  1 |  0 |  0 |
 *          |    5  ||  0 |  1 |  0 |  1 |
 *          |    6  ||  0 |  1 |  1 |  0 |
 *          |    7  ||  0 |  1 |  1 |  1 |
 *          |    8  ||  1 |  0 |  0 |  0 |
 *          |    9  ||  1 |  0 |  0 |  1 |
 *          |   10  ||  1 |  0 |  1 |  0 |
 *          |   11  ||  1 |  0 |  1 |  1 |
 *          |   12  ||  1 |  1 |  0 |  0 |
 *          |   13  ||  1 |  1 |  0 |  1 |
 *          |   14  ||  1 |  1 |  1 |  0 |
 *          |   15  ||  1 |  1 |  1 |  1 |
 *          +-------++----+----+----+----+
 *
 *          Note C Output QA is connected to input B
 *
 *          Reset Count Function table
 *
 *          +-----+-----++----+----+----+----+
 *          | R01 | R02 || QD | QC | QB | QA |
 *          +=====+=====++====+====+====+====+
 *          |  1  |  1  ||  0 |  0 |  0 |  0 |
 *          |  0  |  X  ||       COUNT       |
 *          |  X  |  0  ||       COUNT       |
 *          +-----+-----++----+----+----+----+
 *
 *  Naming conventions follow National Semiconductor datasheet
 *
 */

#include "nld_7493.h"
#include "netlist/nl_base.h"

//- Identifier:  TTL_7493_DIP
//- Title: 7493 Binary Counters
//- Description:
//-   Each of these monolithic counters contains four master-slave
//-   flip-flops and additional gating to provide a divide-by-two
//-   counter and a three-stage binary counter for which the
//-   count cycle length is divide-by-five for the 90A and divide-by-eight
//-   for the 93A.
//-
//-   All of these counters have a gated zero reset and the 90A
//-   also has gated set-to-nine inputs for use in BCD nine’s complement
//-   applications.
//-
//-   To use their maximum count length (decade or four-bit binary),
//-   the B input is connected to the Q A output. The input count pulses
//-   are applied to input A and the outputs are as described in the
//-   appropriate truth table. A symmetrical divide-by-ten count can be
//-   obtained from the 90A counters by connecting the Q D output to the
//-   A input and applying the input count to the B input which gives
//-   a divide-by-ten square wave at output Q A.
//-
//- Pinalias: B,R01,R02,NC,VCC,NC,NC,QC,QB,GND,QD,QA,NC,A
//- Package: DIP
//- NamingConvention: Naming conventions follow National Semiconductor datasheet
//- Limitations: Internal resistor network currently fixed to 5k
//-      more limitations
//- Example: ne555_astable.c,ne555_example
//- FunctionTable:
//-    Counter Sequence
//-
//-    | COUNT || QD | QC | QB | QA |
//-    |------:||:--:|:--:|:--:|:--:|
//-    |    0  ||  0 |  0 |  0 |  0 |
//-    |    1  ||  0 |  0 |  0 |  1 |
//-    |    2  ||  0 |  0 |  1 |  0 |
//-    |    3  ||  0 |  0 |  1 |  1 |
//-    |    4  ||  0 |  1 |  0 |  0 |
//-    |    5  ||  0 |  1 |  0 |  1 |
//-    |    6  ||  0 |  1 |  1 |  0 |
//-    |    7  ||  0 |  1 |  1 |  1 |
//-    |    8  ||  1 |  0 |  0 |  0 |
//-    |    9  ||  1 |  0 |  0 |  1 |
//-    |   10  ||  1 |  0 |  1 |  0 |
//-    |   11  ||  1 |  0 |  1 |  1 |
//-    |   12  ||  1 |  1 |  0 |  0 |
//-    |   13  ||  1 |  1 |  0 |  1 |
//-    |   14  ||  1 |  1 |  1 |  0 |
//-    |   15  ||  1 |  1 |  1 |  1 |
//-
//-    Note C Output QA is connected to input B
//-
//-    Reset Count Function table
//-
//-    | R01 | R02 | QD | QC | QB | QA |
//-    |:---:|:---:|:--:|:--:|:--:|:--:|
//-    |  1  |  1  |  0 |  0 |  0 |  0 |
//-    |  0  |  X  |       COUNT       ||||
//-    |  X  |  0  |       COUNT       ||||
//-
//-

namespace netlist
{
	namespace devices
	{

	static constexpr const netlist_time out_delay = NLTIME_FROM_NS(18);
	static constexpr const netlist_time out_delay2 = NLTIME_FROM_NS(36);
	static constexpr const netlist_time out_delay3 = NLTIME_FROM_NS(54);

	NETLIB_OBJECT(7493)
	{
		NETLIB_CONSTRUCTOR(7493)
		, m_R1(*this, "R1", NETLIB_DELEGATE(inputs))
		, m_R2(*this, "R2", NETLIB_DELEGATE(inputs))
		, m_a(*this, "_m_a", 0)
		, m_bcd(*this, "_m_b", 0)
		, m_r(*this, "_m_r", 0)
		, m_CLKA(*this, "CLKA", NETLIB_DELEGATE(updA))
		, m_CLKB(*this, "CLKB", NETLIB_DELEGATE(updB))
		, m_QA(*this, "QA")
		, m_QB(*this, {"QB", "QC", "QD"})
		, m_power_pins(*this)
		{
		}

	private:
		NETLIB_RESETI()
		{
			m_a = m_bcd = 0;
			m_CLKA.set_state(logic_t::STATE_INP_HL);
			m_CLKB.set_state(logic_t::STATE_INP_HL);
		}

		NETLIB_HANDLERI(inputs)
		{
			if (!(m_R1() && m_R2()))
			{
				m_CLKA.activate_hl();
				m_CLKB.activate_hl();
			}
			else
			{
				m_CLKA.inactivate();
				m_CLKB.inactivate();
				m_QA.push(0, NLTIME_FROM_NS(40));
				m_QB.push(0, NLTIME_FROM_NS(40));
				m_a = m_bcd = 0;
			}
		}

		NETLIB_HANDLERI(updA)
		{
			m_a ^= 1;
			m_QA.push(m_a, out_delay);
		}

		NETLIB_HANDLERI(updB)
		{
			const auto cnt(++m_bcd &= 0x07);

			m_QB[2].push((cnt >> 2) & 1, out_delay3);
			m_QB[1].push((cnt >> 1) & 1, out_delay2);
			m_QB[0].push(cnt & 1, out_delay);
		}

		logic_input_t m_R1;
		logic_input_t m_R2;

		state_var_sig m_a;
		state_var_sig m_bcd;
		state_var_sig m_r;

		logic_input_t m_CLKA;
		logic_input_t m_CLKB;

		logic_output_t m_QA;
		object_array_t<logic_output_t, 3> m_QB;
		nld_power_pins m_power_pins;
	};

	NETLIB_DEVICE_IMPL(7493,        "TTL_7493", "+CLKA,+CLKB,+R1,+R2,@VCC,@GND")

	} // namespace devices
} // namespace netlist
