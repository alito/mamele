// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_7490.c
 *
 */

#include "nld_7490.h"
#include "netlist/nl_base.h"

namespace netlist
{
namespace devices
{

	static constexpr const std::array<netlist_time, 4> delay =
	{
			NLTIME_FROM_NS(18),
			NLTIME_FROM_NS(36) - NLTIME_FROM_NS(18),
			NLTIME_FROM_NS(54) - NLTIME_FROM_NS(18),
			NLTIME_FROM_NS(72) - NLTIME_FROM_NS(18)
	};

	NETLIB_OBJECT(7490)
	{
		NETLIB_CONSTRUCTOR(7490)
		, m_A(*this, "A")
		, m_B(*this, "B")
		, m_R1(*this, "R1")
		, m_R2(*this, "R2")
		, m_R91(*this, "R91")
		, m_R92(*this, "R92")
		, m_cnt(*this, "m_cnt", 0)
		, m_last_A(*this, "m_last_A", 0)
		, m_last_B(*this, "m_last_B", 0)
		, m_Q(*this, {"QA", "QB", "QC", "QD"})
		, m_power_pins(*this)
		{
		}

	private:
		NETLIB_UPDATEI()
		{
			const netlist_sig_t new_A = m_A();
			const netlist_sig_t new_B = m_B();

			if (m_R91() & m_R92())
			{
				m_cnt = 9;
				m_Q.push(9, delay);
			}
			else if (m_R1() & m_R2())
			{
				m_cnt = 0;
				m_Q.push(0, delay);
			}
			else
			{
				if (m_last_A && !new_A)  // High - Low
				{
					m_cnt ^= 1;
					m_Q[0].push(m_cnt & 1, delay[0]);
				}
				if (m_last_B && !new_B)  // High - Low
				{
					m_cnt += 2;
					if (m_cnt >= 10)
						m_cnt &= 1; /* Output A is not reset! */
					m_Q.push(m_cnt, delay);
				}
			}
			m_last_A = new_A;
			m_last_B = new_B;
		}

		NETLIB_RESETI()
		{
			m_cnt = 0;
			m_last_A = 0;
			m_last_B = 0;
		}

		logic_input_t m_A;
		logic_input_t m_B;
		logic_input_t m_R1;
		logic_input_t m_R2;
		logic_input_t m_R91;
		logic_input_t m_R92;

		state_var_u8 m_cnt;
		state_var<netlist_sig_t> m_last_A;
		state_var<netlist_sig_t> m_last_B;

		object_array_t<logic_output_t, 4> m_Q;
		nld_power_pins m_power_pins;
	};

	NETLIB_OBJECT(7490_dip)
	{
		NETLIB_CONSTRUCTOR(7490_dip)
		, A(*this, "A")
		{
			register_subalias("1", "A.B");
			register_subalias("2", "A.R1");
			register_subalias("3", "A.R2");

			// register_subalias("4", ); --> NC
			register_subalias("5", "A.VCC");
			register_subalias("6", "A.R91");
			register_subalias("7", "A.R92");

			register_subalias("8", "A.QC");
			register_subalias("9", "A.QB");
			register_subalias("10", "A.GND");
			register_subalias("11", "A.QD");
			register_subalias("12", "A.QA");
			// register_subalias("13", ); --> NC
			register_subalias("14", "A.A");
		}
		NETLIB_RESETI() {}
		NETLIB_UPDATEI() {}
	private:
		NETLIB_SUB(7490) A;
	};

	NETLIB_DEVICE_IMPL(7490,     "TTL_7490",        "+A,+B,+R1,+R2,+R91,+R92,@VCC,@GND")
	NETLIB_DEVICE_IMPL(7490_dip, "TTL_7490_DIP",    "")

} // namespace devices
} // namespace netlist
