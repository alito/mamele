// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_system.c
 *
 */

#include "netlist/solver/nld_solver.h"
#include "netlist/solver/nld_matrix_solver.h"
#include "nlid_system.h"

namespace netlist
{
namespace devices
{
	// ----------------------------------------------------------------------------------------
	// netlistparams
	// ----------------------------------------------------------------------------------------


	// ----------------------------------------------------------------------------------------
	// extclock
	// ----------------------------------------------------------------------------------------

	NETLIB_RESET(extclock)
	{
		m_cnt = 0;
		m_off = netlist_time::from_fp<decltype(m_offset())>(m_offset());
		m_feedback.set_delegate(NETLIB_DELEGATE(update));
	}

	NETLIB_HANDLER(extclock, clk2)
	{
		m_Q.push((m_cnt & 1) ^ 1, m_inc[m_cnt]);
		if (++m_cnt >= m_size)
			m_cnt = 0;
	}

	NETLIB_HANDLER(extclock, clk2_pow2)
	{
		m_Q.push((m_cnt & 1) ^ 1, m_inc[m_cnt]);
		m_cnt = (++m_cnt) & (m_size-1);
	}

	NETLIB_UPDATE(extclock)
	{
		m_Q.push((m_cnt & 1) ^ 1, m_inc[m_cnt] + m_off());
		m_off = netlist_time::zero();
		if (++m_cnt >= m_size)
			m_cnt = 0;

		// continue with optimized clock handlers ....

		if ((m_size & (m_size-1)) == 0) // power of 2?
			m_feedback.set_delegate(nldelegate(&NETLIB_NAME(extclock)::clk2_pow2, this));
		else
			m_feedback.set_delegate(nldelegate(&NETLIB_NAME(extclock)::clk2, this));
	}


	NETLIB_DEVICE_IMPL(nc_pin,              "NC_PIN",                 "")
	NETLIB_DEVICE_IMPL(frontier,            "FRONTIER_DEV",           "+I,+G,+Q")
	NETLIB_DEVICE_IMPL(function,            "AFUNC",                  "N,FUNC")
	NETLIB_DEVICE_IMPL(analog_input,        "ANALOG_INPUT",           "IN")
	NETLIB_DEVICE_IMPL(clock,               "CLOCK",                  "FREQ")
	NETLIB_DEVICE_IMPL(varclock,            "VARCLOCK",               "FUNC")
	NETLIB_DEVICE_IMPL(extclock,            "EXTCLOCK",               "FREQ,PATTERN")
	NETLIB_DEVICE_IMPL(sys_dsw1,            "SYS_DSW",                "+I,+1,+2")
	NETLIB_DEVICE_IMPL(sys_dsw2,            "SYS_DSW2",               "")
	NETLIB_DEVICE_IMPL(sys_compd,           "SYS_COMPD",              "")

	using NETLIB_NAME(sys_noise_mt_u) =
		NETLIB_NAME(sys_noise)<plib::mt19937_64, plib::uniform_distribution_t>;
	NETLIB_DEVICE_IMPL(sys_noise_mt_u,      "SYS_NOISE_MT_U",         "SIGMA")

	using NETLIB_NAME(sys_noise_mt_n) =
		NETLIB_NAME(sys_noise)<plib::mt19937_64, plib::normal_distribution_t>;
	NETLIB_DEVICE_IMPL(sys_noise_mt_n,      "SYS_NOISE_MT_N",         "SIGMA")

	NETLIB_DEVICE_IMPL(mainclock,           "MAINCLOCK",              "FREQ")
	NETLIB_DEVICE_IMPL(gnd,                 "GNDA",                   "")
	NETLIB_DEVICE_IMPL(netlistparams,       "PARAMETER",              "")

	using NETLIB_NAME(logic_input8) = NETLIB_NAME(logic_inputN)<8>;
	NETLIB_DEVICE_IMPL(logic_input8,         "LOGIC_INPUT8",            "IN,MODEL")

	NETLIB_DEVICE_IMPL(logic_input,         "LOGIC_INPUT",            "IN,MODEL")
	NETLIB_DEVICE_IMPL_ALIAS(logic_input_ttl, logic_input, "TTL_INPUT", "IN")

} // namespace devices
} // namespace netlist
