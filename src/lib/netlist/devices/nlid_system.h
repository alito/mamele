// license:GPL-2.0+
// copyright-holders:Couriersud
#ifndef NLID_SYSTEM_H_
#define NLID_SYSTEM_H_

///
/// \file nlid_system.h
///

#include "netlist/analog/nlid_twoterm.h"
#include "netlist/nl_base.h"
#include "netlist/nl_factory.h"
#include "netlist/plib/prandom.h"
#include "netlist/plib/pstonum.h"
#include "netlist/plib/putil.h"

#include <random>

namespace netlist
{
namespace devices
{
	// -----------------------------------------------------------------------------
	// netlistparams
	// -----------------------------------------------------------------------------

	NETLIB_OBJECT(netlistparams)
	{
		NETLIB_CONSTRUCTOR(netlistparams)
		, m_use_deactivate(*this, "USE_DEACTIVATE", false)
		, m_startup_strategy(*this, "STARTUP_STRATEGY", 0)
		, m_mos_capmodel(*this, "DEFAULT_MOS_CAPMODEL", 2)
		, m_max_link_loops(*this, "MAX_LINK_RESOLVE_LOOPS", 100)
		{
		}
		//NETLIB_RESETI() {}
		//NETLIB_UPDATE_PARAMI() { }
	public:
		param_logic_t m_use_deactivate;
		param_num_t<unsigned>   m_startup_strategy;
		param_num_t<unsigned>   m_mos_capmodel;
		//! How many times do we try to resolve links (connections)
		param_num_t<unsigned>   m_max_link_loops;
	};

	// -----------------------------------------------------------------------------
	// clock
	// -----------------------------------------------------------------------------

	NETLIB_OBJECT(clock)
	{
		NETLIB_CONSTRUCTOR(clock)
		, m_feedback(*this, "FB", NETLIB_DELEGATE(fb))
		, m_Q(*this, "Q")
		, m_freq(*this, "FREQ", nlconst::magic(7159000.0 * 5.0))
		, m_supply(*this)
		{
			m_inc = netlist_time::from_fp(plib::reciprocal(m_freq()*nlconst::two()));

			connect(m_feedback, m_Q);
		}

		NETLIB_UPDATE_PARAMI()
		{
			m_inc = netlist_time::from_fp(plib::reciprocal(m_freq()*nlconst::two()));
		}

		NETLIB_HANDLERI(fb)
		{
			m_Q.push(m_feedback() ^ 1, m_inc);
		}

	private:
		logic_input_t m_feedback;
		logic_output_t m_Q;

		param_fp_t m_freq;
		netlist_time m_inc;

		NETLIB_NAME(power_pins) m_supply;
};

	// -----------------------------------------------------------------------------
	// varclock
	// -----------------------------------------------------------------------------

	NETLIB_OBJECT(varclock)
	{
		NETLIB_CONSTRUCTOR(varclock)
		, m_N(*this, "N", 1)
		, m_func(*this,"FUNC", "T")
		, m_feedback(*this, "FB", NETLIB_DELEGATE(fb))
		, m_Q(*this, "Q")
		, m_compiled(*this, "m_compiled")
		, m_supply(*this)
		{
			if (!m_func().empty())
			{
				std::vector<pstring> inps;
				inps.push_back(pstring("T"));
				m_vals.push_back(nlconst::zero());
				for (int i=0; i < m_N(); i++)
				{
					pstring inpname = plib::pfmt("A{1}")(i);
					m_I.push_back(state().make_pool_object<analog_input_t>(*this, inpname, NETLIB_DELEGATE(fb)));
					inps.push_back(inpname);
					m_vals.push_back(nlconst::zero());
				}
				m_compiled->compile(m_func(), inps);
			}
			connect(m_feedback, m_Q);
		}
		//NETLIB_RESETI();
		//NETLIB_UPDATE_PARAMI()

	private:
		NETLIB_HANDLERI(fb)
		{
			m_vals[0] = exec().time().as_fp<nl_fptype>();
			for (std::size_t i = 0; i < static_cast<unsigned>(m_N()); i++)
			{
				m_vals[i+1] = (*m_I[i])();
			}
			const netlist_time m_inc = netlist_time::from_fp(m_compiled->evaluate(m_vals));
			m_Q.push(m_feedback() ^ 1, m_inc);
		}

		using pf_type = plib::pfunction<nl_fptype>;
		param_int_t m_N;
		param_str_t m_func;
		logic_input_t m_feedback;
		logic_output_t m_Q;
		std::vector<device_arena::unique_ptr<analog_input_t>> m_I;

		pf_type::values_container m_vals;
		state_var<pf_type> m_compiled;

		NETLIB_NAME(power_pins) m_supply;
	};

	// -----------------------------------------------------------------------------
	// extclock
	// -----------------------------------------------------------------------------

	NETLIB_OBJECT(extclock)
	{
		NETLIB_CONSTRUCTOR(extclock)
		, m_freq(*this, "FREQ", nlconst::magic(7159000.0 * 5.0))
		, m_pattern(*this, "PATTERN", "1,1")
		, m_offset(*this, "OFFSET", nlconst::zero())
		, m_feedback(*this, "FB", NETLIB_DELEGATE(first))
		, m_Q(*this, "Q")
		, m_cnt(*this, "m_cnt", 0)
		, m_off(*this, "m_off", netlist_time::zero())
		{
			m_inc[0] = netlist_time::from_fp(plib::reciprocal(m_freq()*nlconst::two()));

			connect(m_feedback, m_Q);

			netlist_time base = netlist_time::from_fp(plib::reciprocal(m_freq()*nlconst::two()));
			std::vector<pstring> pat(plib::psplit(m_pattern(),","));
			m_off = netlist_time::from_fp(m_offset());

			std::array<std::int64_t, 32> pati = { 0 };

			m_size = static_cast<std::uint8_t>(pat.size());
			netlist_time::mult_type total = 0;
			for (unsigned i=0; i<m_size; i++)
			{
				pati[i] = plib::pstonum<std::int64_t>(pat[i]);
				total += pati[i];
			}
			netlist_time ttotal = netlist_time::zero();
			auto sm1 = static_cast<uint8_t>(m_size - 1);
			for (unsigned i=0; i < sm1; i++)
			{
				m_inc[i] = base * pati[i];
				ttotal += m_inc[i];
			}
			m_inc[sm1] = base * total - ttotal;

		}

		NETLIB_RESETI()
		{
			m_cnt = 0;
			m_off = netlist_time::from_fp<decltype(m_offset())>(m_offset());
			m_feedback.set_delegate(NETLIB_DELEGATE(first));
		}
		//NETLIB_UPDATE_PARAMI();

	private:

		NETLIB_HANDLERI(clk2)
		{
			m_Q.push((m_cnt & 1) ^ 1, m_inc[m_cnt]);
			if (++m_cnt >= m_size)
				m_cnt = 0;
		}

		NETLIB_HANDLERI(clk2_pow2)
		{
			m_Q.push((m_cnt & 1) ^ 1, m_inc[m_cnt]);
			m_cnt = (++m_cnt) & (m_size-1);
		}

		NETLIB_HANDLERI(first)
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


		param_fp_t m_freq;
		param_str_t m_pattern;
		param_fp_t m_offset;

		logic_input_t m_feedback;
		logic_output_t m_Q;
		state_var_u8 m_cnt;
		std::uint8_t m_size;
		state_var<netlist_time> m_off;
		std::array<netlist_time, 32> m_inc;
	};

	// -----------------------------------------------------------------------------
	// Special support devices ...
	// -----------------------------------------------------------------------------

	NETLIB_OBJECT(logic_input)
	{
		NETLIB_CONSTRUCTOR(logic_input)
		, m_Q(*this, "Q")
		, m_IN(*this, "IN", false)
		, m_supply(*this)
		{
		}

		NETLIB_RESETI() { m_Q.initial(0); }
		NETLIB_UPDATE_PARAMI()
		{
			//printf("%s %d\n", name().c_str(), m_IN());
			m_Q.push(m_IN() & 1, netlist_time::from_nsec(1));
		}

	private:
		logic_output_t m_Q;

		param_logic_t m_IN;
		NETLIB_NAME(power_pins) m_supply;
	};

	template<std::size_t N>
	NETLIB_OBJECT(logic_inputN)
	{
		NETLIB_CONSTRUCTOR(logic_inputN)
		, m_Q(*this, "Q{}")
		, m_IN(*this, "IN", 0)
		, m_supply(*this)
		{
		}

		NETLIB_RESETI() { for (auto &q : m_Q) q.initial(0); }
		NETLIB_UPDATE_PARAMI()
		{
			//printf("%s %d\n", name().c_str(), m_IN());
			for (std::size_t i=0; i<N; i++)
				m_Q[i].push((m_IN()>>i) & 1, netlist_time::from_nsec(1));
		}

	private:
		object_array_t<logic_output_t, N> m_Q;

		param_int_t m_IN;
		NETLIB_NAME(power_pins) m_supply;
	};

	NETLIB_OBJECT(analog_input)
	{
		NETLIB_CONSTRUCTOR(analog_input)
		, m_Q(*this, "Q")
		, m_IN(*this, "IN", nlconst::zero())
		{
		}

		NETLIB_RESETI() { m_Q.initial(nlconst::zero()); }
		NETLIB_UPDATE_PARAMI() { m_Q.push(m_IN()); }

	private:
		analog_output_t m_Q;
		param_fp_t m_IN;
	};

	// -----------------------------------------------------------------------------
	// nld_gnd
	// -----------------------------------------------------------------------------

	NETLIB_OBJECT(gnd)
	{
		NETLIB_CONSTRUCTOR(gnd)
		, m_Q(*this, "Q")
		{
		}

		NETLIB_UPDATE_PARAMI()
		{
			m_Q.push(nlconst::zero());
		}

		//NETLIB_RESETI() {}
	protected:
		analog_output_t m_Q;
	};

	// -----------------------------------------------------------------------------
	// nld_nc_pin
	// -----------------------------------------------------------------------------

	NETLIB_OBJECT(nc_pin)
	{
	public:
		NETLIB_CONSTRUCTOR(nc_pin)
		, m_I(*this, "I", NETLIB_DELEGATE(noop))
		{
		}

	protected:
		//NETLIB_RESETI() {}

	private:
		NETLIB_HANDLERI(noop)
		{
		}

		analog_input_t m_I;

	};

	// -----------------------------------------------------------------------------
	// nld_frontier
	// -----------------------------------------------------------------------------


	/// \brief Frontiers divides a netlist into sub netlist
	///
	/// Example:
	///
	/// Consider the following mixing stage
	///
	///                 R1
	///      S1 >-----1RRRR2---------+
	///                              |
	///                 R2           |
	///      S2 >-----1RRRR2---------+----------> Out
	///                              |
	///                              R
	///                           R3 R
	///                              R
	///                              |
	///                             GND
	///
	/// With OPTIMIZE_FRONTIER(R2.2, R3, R2) this becomes:
	///
	///                 R1
	///      S1 >-----1RRRR2--------------------------------+
	///                                                     |
	///                       ##########################    |
	///                 R2    #                    R2  #    |
	///      S2 >-----1RRRR2-----+-->AnIn AnOut>--RRRR------+----------> Out
	///                       #  |                     #    |
	///                       #  R                     #    R
	///                       #  R R3                  # R3 R
	///                       #  R                     #    R
	///                       #  |                     #    |
	///                       # GND          Frontier  #   GND
	///                       #                        #
	///                       ##########################
	///
	/// As a result, provided there are no other connections between the parts
	/// generating S1 and S2 the "S2 part" will now have a separate solver.
	///
	/// The size (aka number of nets) of the solver for S1 will be smaller.
	/// The size of the solver for S2 and the rest of the circuit will be smaller
	/// as well.
	///
	///
	///
	///
	///
	///
	///
	///
	///
	///

	NETLIB_OBJECT(frontier)
	{
	public:
		NETLIB_CONSTRUCTOR(frontier)
		, m_RIN(*this, "m_RIN", NETLIB_DELEGATE(input))
		, m_ROUT(*this, "m_ROUT", NETLIB_DELEGATE(input))
		, m_I(*this, "_I", NETLIB_DELEGATE(input))
		, m_Q(*this, "_Q")
		, m_p_RIN(*this, "RIN", nlconst::magic(1.0e6))
		, m_p_ROUT(*this, "ROUT", nlconst::magic(50.0))

		{
			register_subalias("I", m_RIN.P());
			register_subalias("G", m_RIN.N());
			connect(m_I, m_RIN.P());

			register_subalias("_OP", m_ROUT.P());
			register_subalias("Q", m_ROUT.N());
			connect(m_Q, m_ROUT.P());
		}

		NETLIB_RESETI()
		{
			m_RIN.set_G_V_I(plib::reciprocal(m_p_RIN()),0,0);
			m_ROUT.set_G_V_I(plib::reciprocal(m_p_ROUT()),0,0);
		}

	private:
		NETLIB_HANDLERI(input)
		{
			m_Q.push(m_I());
		}

		analog::NETLIB_NAME(twoterm) m_RIN;
		analog::NETLIB_NAME(twoterm) m_ROUT;
		analog_input_t m_I;
		analog_output_t m_Q;

		param_fp_t m_p_RIN;
		param_fp_t m_p_ROUT;
	};

	// -----------------------------------------------------------------------------
	// nld_function
	// ----------------------------------------------------------------------------- */

	NETLIB_OBJECT(function)
	{
		NETLIB_CONSTRUCTOR(function)
		, m_N(*this, "N", 1)
		, m_func(*this, "FUNC", "A0")
		, m_Q(*this, "Q")
		, m_compiled(*this, "m_compiled")
		{
			std::vector<pstring> inps;
			for (int i=0; i < m_N(); i++)
			{
				pstring inpname = plib::pfmt("A{1}")(i);
				m_I.push_back(state().make_pool_object<analog_input_t>(*this, inpname, NETLIB_DELEGATE(inputs)));
				inps.push_back(inpname);
				m_vals.push_back(nlconst::zero());
			}
			m_compiled->compile(m_func(), inps);
		}

	protected:
		NETLIB_RESETI()
		{
			//m_Q.initial(0.0);
		}

		NETLIB_HANDLERI(inputs)
		{
			for (std::size_t i = 0; i < static_cast<unsigned>(m_N()); i++)
			{
				m_vals[i] = (*m_I[i])();
			}
			m_Q.push(m_compiled->evaluate(m_vals));
		}

	private:
		using pf_type = plib::pfunction<nl_fptype>;
		param_int_t m_N;
		param_str_t m_func;
		analog_output_t m_Q;
		std::vector<device_arena::unique_ptr<analog_input_t>> m_I;

		pf_type::values_container m_vals;
		state_var<pf_type> m_compiled;

	};

	// -----------------------------------------------------------------------------
	// nld_sys_dsw1
	// -----------------------------------------------------------------------------

	NETLIB_OBJECT(sys_dsw1)
	{
	public:
		NETLIB_CONSTRUCTOR(sys_dsw1)
		, m_RON(*this, "RON", nlconst::one())
		, m_ROFF(*this, "ROFF", nlconst::magic(1.0E20))
		, m_R(*this, "_R")
		, m_I(*this, "I", NETLIB_DELEGATE(input))
		, m_last_state(*this, "m_last_state", 0)
		{
			register_subalias("1", m_R.P());
			register_subalias("2", m_R.N());
		}

		NETLIB_RESETI()
		{
			m_last_state = 0;
			m_R.set_R(m_ROFF());
		}

		//NETLIB_UPDATE_PARAMI();

		//FIXME: used by 74123

		const terminal_t &P() const noexcept { return m_R.P(); }
		const terminal_t &N() const noexcept { return m_R.N(); }
		const logic_input_t &I() const noexcept { return m_I; }

		param_fp_t m_RON;
		param_fp_t m_ROFF;

	private:
		NETLIB_HANDLERI(input)
		{
			const netlist_sig_t state = m_I();
			if (state != m_last_state)
			{
				m_last_state = state;
				const nl_fptype R = (state != 0) ? m_RON() : m_ROFF();

				m_R.change_state([this, &R]()
				{
					m_R.set_R(R);
				});
			}
		}

		analog::NETLIB_SUB(R_base) m_R;
		logic_input_t m_I;

		state_var<netlist_sig_t> m_last_state;
	};

	// -----------------------------------------------------------------------------
	// nld_sys_dsw2
	// -----------------------------------------------------------------------------

	NETLIB_OBJECT(sys_dsw2)
	{
	public:
		NETLIB_CONSTRUCTOR(sys_dsw2)
		, m_R1(*this, "_R1")
		, m_R2(*this, "_R2")
		, m_I(*this, "I", NETLIB_DELEGATE(input))
		, m_GON(*this, "GON", nlconst::magic(1e9)) // FIXME: all switches should have some on value
		, m_GOFF(*this, "GOFF", nlconst::cgmin())
		, m_power_pins(*this)
		{
			// connect and register pins
			register_subalias("1", m_R1.P());
			register_subalias("2", m_R1.N());
			register_subalias("3", m_R2.N());
			connect(m_R1.N(), m_R2.P());
		}

		NETLIB_RESETI()
		{
			m_R1.set_G(m_GOFF());
			m_R2.set_G(m_GON());
		}

		//NETLIB_UPDATE_PARAMI();

	private:
		NETLIB_HANDLERI(input)
		{
			const netlist_sig_t state = m_I();

			//printf("Here %d\n", state);
			const nl_fptype G1 = (state != 0) ? m_GON() : m_GOFF();
			const nl_fptype G2 = (state != 0) ? m_GOFF() : m_GON();
			if (m_R1.solver() == m_R2.solver())
			{
				m_R1.change_state([this, &G1, &G2]()
				{
					m_R1.set_G(G1);
					m_R2.set_G(G2);
				});
			}
			else
			{
				m_R1.change_state([this, &G1]()
				{
					m_R1.set_G(G1);
				});
				m_R2.change_state([this, &G2]()
				{
					m_R2.set_G(G2);
				});
			}
		}

		analog::NETLIB_SUB(R_base) m_R1;
		analog::NETLIB_SUB(R_base) m_R2;
		logic_input_t m_I;
		param_fp_t m_GON;
		param_fp_t m_GOFF;

		nld_power_pins m_power_pins;
	};


	// -----------------------------------------------------------------------------
	// nld_sys_comp
	// -----------------------------------------------------------------------------

	NETLIB_OBJECT(sys_compd)
	{
	public:
		NETLIB_CONSTRUCTOR(sys_compd)
		, m_IP(*this, "IP", NETLIB_DELEGATE(inputs))
		, m_IN(*this, "IN", NETLIB_DELEGATE(inputs))
		, m_Q(*this, "Q")
		, m_QQ(*this, "QQ")
		, m_power_pins(*this)
		, m_last_state(*this, "m_last_state", 2) // ensure first execution
		{
		}

		NETLIB_RESETI()
		{
			m_last_state = 0;
		}

		//NETLIB_UPDATE_PARAMI();

	private:
		NETLIB_HANDLERI(inputs)
		{
			const netlist_sig_t state = (m_IP() > m_IN());
			if (state != m_last_state)
			{
				m_last_state = state;
				// FIXME: make timing a parameter
				m_Q.push(state, NLTIME_FROM_NS(10));
				m_QQ.push(!state, NLTIME_FROM_NS(10));
			}
		}

		analog_input_t m_IP;
		analog_input_t m_IN;
		logic_output_t m_Q;
		logic_output_t m_QQ;
		nld_power_pins m_power_pins;

		state_var<netlist_sig_t> m_last_state;
	};

	///
	/// \brief  noise source
	///
	/// An externally clocked noise source. The noise acts as a voltage source
	/// with internal resistance RI.
	///
	/// Since a new random value is used on each state change on I the effective
	/// frequency is clock source frequency times two!
	///
	/// Typical application:
	///
	///             VCC
	///              |
	///              R
	///              R
	///              R
	///              |
	///              +-----> Output
	///              |
	///         +-------+
	///         |    1  |
	///     --->| I     |
	///         |    2  |
	///         +-------+
	///              |
	///              R
	///              R
	///              R
	///              |
	///             GND
	///
	// -----------------------------------------------------------------------------
	template <typename E, template<class> class D>
	NETLIB_OBJECT(sys_noise)
	{
	public:

		using engine = E;
		using distribution = D<nl_fptype>;

		NETLIB_CONSTRUCTOR(sys_noise)
		, m_T(*this, "m_T")
		, m_I(*this, "I", NETLIB_DELEGATE(input))
		, m_RI(*this, "RI", nlconst::magic(0.1))
		, m_sigma(*this, "SIGMA", nlconst::zero())
		, m_mt(*this, "m_mt")
		, m_dis(*this, "m_dis",m_sigma())
		{

			register_subalias("1", m_T.P());
			register_subalias("2", m_T.N());
		}

	private:
		NETLIB_HANDLERI(input)
		{
			nl_fptype val = m_dis.var()(m_mt.var());
			m_T.change_state([this, val]()
			{
				m_T.set_G_V_I(plib::reciprocal(m_RI()), val, nlconst::zero());
			});
		}

		NETLIB_RESETI()
		{
			m_T.set_G_V_I(plib::reciprocal(m_RI()), nlconst::zero(), nlconst::zero());
		}

		analog::NETLIB_SUB(twoterm) m_T;
		logic_input_t m_I;
		param_fp_t m_RI;
		param_fp_t m_sigma;
		state_var<engine> m_mt;
		state_var<distribution> m_dis;
	};

} // namespace devices
} // namespace netlist

#endif // NLD_SYSTEM_H_
