// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_4020.cpp
 *
 *  CD4020: 14-Stage Ripple Carry Binary Counters
 *
 *          +--------------+
 *      Q12 |1     ++    16| VDD
 *      Q13 |2           15| Q11
 *      Q14 |3           14| Q10
 *       Q6 |4    4020   13| Q8
 *       Q5 |5           12| Q9
 *       Q7 |6           11| RESET
 *       Q4 |7           10| IP (Input pulses)
 *      VSS |8            9| Q1
 *          +--------------+
 *
 *
 *  CD4024: 7-Stage Ripple Carry Binary Counters
 *
 *          +--------------+
 *       IP |1     ++    14| VDD
 *    RESET |2           13| NC
 *       Q7 |3           12| Q1
 *       Q6 |4    4024   11| Q2
 *       Q5 |5           10| NC
 *       Q4 |6            9| Q3
 *      VSS |7            8| NC
 *          +--------------+
 *
 *
 *  Naming conventions follow Texas Instruments datasheet
 *
 *  FIXME: Timing depends on VDD-VSS
 *         This needs a cmos d-a/a-d proxy implementation.
 *
 */

#include "nld_4020.h"
#include "nl_base.h"
#include "nl_factory.h"

namespace netlist
{
	namespace devices
	{

	template <unsigned _TotalBits, unsigned _LiveBitmask>
	NETLIB_OBJECT(CD4020_sub)
	{
		static_assert((_LiveBitmask >> _TotalBits) == 0, "Live bitmask too large");

		NETLIB_CONSTRUCTOR_MODEL(CD4020_sub, "CD4XXX")
		, m_IP(*this, "IP", NETLIB_DELEGATE(ip))
		, m_RESET(*this, "RESET", NETLIB_DELEGATE(reseti))
		, m_Q(*this, 1, "Q{}")
		, m_cnt(*this, "m_cnt", 0)
		, m_supply(*this)
		{
		}

		NETLIB_RESETI()
		{
			m_IP.set_state(logic_t::STATE_INP_HL);
			m_cnt = 0;
		}

		NETLIB_HANDLERI(ip)
		{
			++m_cnt;
			update_outputs(m_cnt);
		}

		NETLIB_HANDLERI(reseti)
		{
			if (m_RESET())
			{
				m_cnt = 0;
				m_IP.inactivate();
				/* static */ const netlist_time reset_time = netlist_time::from_nsec(140);
				for (unsigned i = 0; i < _TotalBits; i++)
					if (((_LiveBitmask >> i) & 1) != 0)
						m_Q[i].push(0, reset_time);
			}
			else
				m_IP.activate_hl();
		}

	public:
		void update_outputs(const unsigned cnt) noexcept
		{
			static constexpr const std::array<netlist_time, 14> out_delayQn = {
					NLTIME_FROM_NS(180), NLTIME_FROM_NS(280),
					NLTIME_FROM_NS(380), NLTIME_FROM_NS(480),
					NLTIME_FROM_NS(580), NLTIME_FROM_NS(680),
					NLTIME_FROM_NS(780), NLTIME_FROM_NS(880),
					NLTIME_FROM_NS(980), NLTIME_FROM_NS(1080),
					NLTIME_FROM_NS(1180), NLTIME_FROM_NS(1280),
					NLTIME_FROM_NS(1380), NLTIME_FROM_NS(1480),
			};

			for (unsigned i = 0; i < _TotalBits; i++)
				if (((_LiveBitmask >> i) & 1) != 0)
					m_Q[i].push((cnt >> i) & 1, out_delayQn[i]);
		}
		logic_input_t m_IP;
		logic_input_t m_RESET;
		object_array_t<logic_output_t, _TotalBits> m_Q;

		state_var<unsigned> m_cnt;
		nld_power_pins m_supply;
	};

	NETLIB_OBJECT(CD4020)
	{
		NETLIB_CONSTRUCTOR_MODEL(CD4020, "CD4XXX")
		, m_sub(*this, "sub")
		{
			register_subalias("IP", m_sub.m_IP);
			register_subalias("RESET", m_sub.m_RESET);
			register_subalias("Q1", m_sub.m_Q[0]);
			register_subalias("Q4", m_sub.m_Q[3]);
			register_subalias("Q5", m_sub.m_Q[4]);
			register_subalias("Q6", m_sub.m_Q[5]);
			register_subalias("Q7", m_sub.m_Q[6]);
			register_subalias("Q8", m_sub.m_Q[7]);
			register_subalias("Q9", m_sub.m_Q[8]);
			register_subalias("Q10", m_sub.m_Q[9]);
			register_subalias("Q11", m_sub.m_Q[10]);
			register_subalias("Q12", m_sub.m_Q[11]);
			register_subalias("Q13", m_sub.m_Q[12]);
			register_subalias("Q14", m_sub.m_Q[13]);
			register_subalias("VDD", "sub.VDD");
			register_subalias("VSS", "sub.VSS");
		}

		//NETLIB_RESETI() {}

	private:
		NETLIB_SUB(CD4020_sub)<14, 0x3ff9> m_sub;
	};

	NETLIB_OBJECT(CD4024)
	{
		NETLIB_CONSTRUCTOR_MODEL(CD4024, "CD4XXX")
		, m_sub(*this, "sub")
		{
			register_subalias("IP", m_sub.m_IP);
			register_subalias("RESET", m_sub.m_RESET);
			register_subalias("Q1", m_sub.m_Q[0]);
			register_subalias("Q2", m_sub.m_Q[1]);
			register_subalias("Q3", m_sub.m_Q[2]);
			register_subalias("Q4", m_sub.m_Q[3]);
			register_subalias("Q5", m_sub.m_Q[4]);
			register_subalias("Q6", m_sub.m_Q[5]);
			register_subalias("Q7", m_sub.m_Q[6]);
			register_subalias("VDD", "sub.VDD");
			register_subalias("VSS", "sub.VSS");
		}

		//NETLIB_RESETI() {}

	private:
		NETLIB_SUB(CD4020_sub)<7, 0x7f> m_sub;
	};



	NETLIB_DEVICE_IMPL(CD4020,         "CD4020", "+IP,+RESET,+VDD,+VSS")

	NETLIB_DEVICE_IMPL(CD4024,         "CD4024", "")

	} //namespace devices
} // namespace netlist
