// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_7448.cpp
 *
 *  DM7448: BCD to 7-Segment decoders/drivers
 *
 *           +--------------+
 *         B |1     ++    16| VCC
 *         C |2           15| f
 * LAMP TEST |3           14| g
 *    BI/RBQ |4    7448   13| a
 *       RBI |5           12| b
 *         D |6           11| c
 *         A |7           10| d
 *       GND |8            9| e
 *           +--------------+
 *
 *
 *  Naming conventions follow National Semiconductor datasheet
 *
 */

#include "nl_base.h"
#include "nl_factory.h"

#include <array>

namespace netlist
{
	namespace devices
	{
	NETLIB_OBJECT(7448)
	{
		NETLIB_CONSTRUCTOR(7448)
		, m_A(*this, "A", NETLIB_DELEGATE(inputs))
		, m_B(*this, "B", NETLIB_DELEGATE(inputs))
		, m_C(*this, "C", NETLIB_DELEGATE(inputs))
		, m_D(*this, "D", NETLIB_DELEGATE(inputs))
		, m_LTQ(*this, "LTQ", NETLIB_DELEGATE(inputs))
		, m_BIQ(*this, "BIQ", NETLIB_DELEGATE(inputs))
		, m_RBIQ(*this, "RBIQ", NETLIB_DELEGATE(inputs))
		, m_state(*this, "m_state", 0)
		, m_Q(*this, {"a", "b", "c", "d", "e", "f", "g"})
		, m_power_pins(*this)
		{
		}

		NETLIB_RESETI()
		{
			m_state = 0;
		}

	private:
		void update_outputs(unsigned v) noexcept
		{
			nl_assert(v<16);
			if (v != m_state)
			{
				// max transfer time is 100 NS */

				uint8_t t = tab7448[v];
				for (std::size_t i = 0; i < 7; i++)
					m_Q[i].push((t >> (6-i)) & 1, NLTIME_FROM_NS(100));
				m_state = v;
			}
		}

		NETLIB_HANDLERI(inputs)
		{
			if (!m_BIQ() || (m_BIQ() && !m_LTQ()))
			{
				m_A.inactivate();
				m_B.inactivate();
				m_C.inactivate();
				m_D.inactivate();
				m_RBIQ.inactivate();
				if (m_BIQ() && !m_LTQ())
				{
					update_outputs(8);
				}
				else if (!m_BIQ())
				{
					update_outputs(15);
				}
			} else {
				m_RBIQ.activate();
				m_D.activate();
				m_C.activate();
				m_B.activate();
				m_A.activate();
				unsigned v = (m_A() << 0) | (m_B() << 1) | (m_C() << 2) | (m_D() << 3);
				if ((!m_RBIQ() && (v==0)))
						v = 15;
				update_outputs(v);
			}
		}

		logic_input_t m_A;
		logic_input_t m_B;
		logic_input_t m_C;
		logic_input_t m_D;
		logic_input_t m_LTQ;
		logic_input_t m_BIQ;
		logic_input_t m_RBIQ;

		state_var<unsigned> m_state;

		object_array_t<logic_output_t, 7> m_Q;  /* a .. g */
		nld_power_pins m_power_pins;

		static const std::array<uint8_t, 16> tab7448;
	};

#define BITS7(b6,b5,b4,b3,b2,b1,b0) ((b6)<<6) | ((b5)<<5) | ((b4)<<4) | ((b3)<<3) | ((b2)<<2) | ((b1)<<1) | ((b0)<<0)

	const std::array<uint8_t, 16> NETLIB_NAME(7448)::tab7448 =
	{
			BITS7(   1, 1, 1, 1, 1, 1, 0 ),  /* 00 - not blanked ! */
			BITS7(   0, 1, 1, 0, 0, 0, 0 ),  /* 01 */
			BITS7(   1, 1, 0, 1, 1, 0, 1 ),  /* 02 */
			BITS7(   1, 1, 1, 1, 0, 0, 1 ),  /* 03 */
			BITS7(   0, 1, 1, 0, 0, 1, 1 ),  /* 04 */
			BITS7(   1, 0, 1, 1, 0, 1, 1 ),  /* 05 */
			BITS7(   0, 0, 1, 1, 1, 1, 1 ),  /* 06 */
			BITS7(   1, 1, 1, 0, 0, 0, 0 ),  /* 07 */
			BITS7(   1, 1, 1, 1, 1, 1, 1 ),  /* 08 */
			BITS7(   1, 1, 1, 0, 0, 1, 1 ),  /* 09 */
			BITS7(   0, 0, 0, 1, 1, 0, 1 ),  /* 10 */
			BITS7(   0, 0, 1, 1, 0, 0, 1 ),  /* 11 */
			BITS7(   0, 1, 0, 0, 0, 1, 1 ),  /* 12 */
			BITS7(   1, 0, 0, 1, 0, 1, 1 ),  /* 13 */
			BITS7(   0, 0, 0, 1, 1, 1, 1 ),  /* 14 */
			BITS7(   0, 0, 0, 0, 0, 0, 0 ),  /* 15 */
	};

	NETLIB_DEVICE_IMPL(7448, "TTL_7448", "+A,+B,+C,+D,+LTQ,+BIQ,+RBIQ,@VCC,@GND")

	} //namespace devices
} // namespace netlist
