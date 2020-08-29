// license:GPL-2.0+
// copyright-holders:Couriersud

#ifndef NLD_MATRIX_SOLVER_H_
#define NLD_MATRIX_SOLVER_H_

///
/// \file nld_matrix_solver.h
///

#include "netlist/nl_base.h"
#include "netlist/nl_errstr.h"
#include "netlist/plib/mat_cr.h"
#include "netlist/plib/palloc.h"
#include "netlist/plib/penum.h"
#include "netlist/plib/pmatrix2d.h"
#include "netlist/plib/pmempool.h"
#include "netlist/plib/putil.h"
#include "netlist/plib/vector_ops.h"

#include <numeric>

//FIXME: remove again

#define PFDEBUG(x)

namespace netlist
{
namespace solver
{

	enum static_compile_target
	{
		CXX_EXTERNAL_C,
		CXX_STATIC
	};

	PENUM(matrix_sort_type_e,
		NOSORT,
		ASCENDING,
		DESCENDING,
		PREFER_IDENTITY_TOP_LEFT,
		PREFER_BAND_MATRIX
	)

	PENUM(matrix_type_e,
		SOR_MAT,
		MAT_CR,
		MAT,
		SM,
		W,
		SOR,
		GMRES
	)

	PENUM(matrix_fp_type_e,
		  FLOAT
		, DOUBLE
		, LONGDOUBLE
		, FLOATQ128
	)

	using static_compile_container = std::vector<std::pair<pstring, pstring>>;

	struct solver_parameter_defaults
	{
		constexpr nl_fptype          m_freq() { return nlconst::magic(48000.0); }

		// iteration parameters
		constexpr nl_fptype          m_gs_sor() { return nlconst::magic(1.059); }
		constexpr matrix_type_e      m_method() { return matrix_type_e::MAT_CR; }
		constexpr matrix_fp_type_e   m_fp_type() { return matrix_fp_type_e::DOUBLE; }
		constexpr nl_fptype          m_reltol() { return nlconst::magic(1e-3); }
		constexpr nl_fptype          m_vntol() { return nlconst::magic(1e-7); }
		constexpr nl_fptype          m_accuracy() { return nlconst::magic(1e-7); }
		constexpr std::size_t        m_nr_loops() { return 250; }
		constexpr std::size_t        m_gs_loops() { return 9; }

		// general parameters
		constexpr nl_fptype          m_gmin() { return nlconst::magic(1e-9); }
		constexpr bool               m_pivot() { return false; }
		constexpr nl_fptype          m_nr_recalc_delay(){ return netlist_time::quantum().as_fp<nl_fptype>(); }
		constexpr int                m_parallel() { return 0; }

		constexpr nl_fptype          m_min_ts_ts() { return nlconst::magic(1e-9); }
		// automatic time step
		constexpr bool               m_dynamic_ts() { return false; }
		constexpr nl_fptype          m_dynamic_lte() { return nlconst::magic(1e-5); }
		constexpr nl_fptype          m_dynamic_min_ts() { return nlconst::magic(1e-6); }

		// matrix sorting
		constexpr matrix_sort_type_e m_sort_type() { return matrix_sort_type_e::PREFER_IDENTITY_TOP_LEFT; }

		// special
		constexpr bool               m_use_gabs() { return true; }

		static solver_parameter_defaults &get_instance()
		{
			static solver_parameter_defaults s;
			return s;
		}
	};

	struct solver_parameters_t
	{
		template <typename D>
		solver_parameters_t(device_t &parent, const pstring &prefix, D &defaults)
		: m_freq(parent, prefix + "FREQ", defaults.m_freq())

		// iteration parameters
		, m_gs_sor(parent,   prefix + "SOR_FACTOR", defaults.m_gs_sor())
		, m_method(parent,   prefix + "METHOD", defaults.m_method())
		, m_fp_type(parent,  prefix + "FPTYPE", defaults.m_fp_type())
		, m_reltol(parent,   prefix + "RELTOL", defaults.m_reltol())            ///< SPICE RELTOL parameter
		, m_vntol(parent,    prefix + "VNTOL",  defaults.m_vntol())            ///< SPICE VNTOL parameter
		, m_accuracy(parent, prefix + "ACCURACY", defaults.m_accuracy())          ///< Iterative solver accuracy
		, m_nr_loops(parent, prefix + "NR_LOOPS", defaults.m_nr_loops())           ///< Maximum number of Newton-Raphson loops
		, m_gs_loops(parent, prefix + "GS_LOOPS", defaults.m_gs_loops())             ///< Maximum number of Gauss-Seidel loops

		// general parameters
		, m_gmin(parent, prefix + "GMIN", defaults.m_gmin())
		, m_pivot(parent, prefix + "PIVOT", defaults.m_pivot())               ///< use pivoting on supported solvers
		, m_nr_recalc_delay(parent, prefix + "NR_RECALC_DELAY", defaults.m_nr_recalc_delay()) ///< Delay to next solve attempt if nr loops exceeded
		, m_parallel(parent, prefix + "PARALLEL", defaults.m_parallel())
		, m_min_ts_ts(parent, prefix + "MIN_TS_TS", defaults.m_min_ts_ts()) ///< The minimum time step for solvers with time stepping devices.

		// automatic time step
		, m_dynamic_ts(parent, prefix + "DYNAMIC_TS", defaults.m_dynamic_ts())     ///< Use dynamic time stepping
		, m_dynamic_lte(parent, prefix + "DYNAMIC_LTE", defaults.m_dynamic_lte())    ///< dynamic time stepping slope
		, m_dynamic_min_ts(parent, prefix + "DYNAMIC_MIN_TIMESTEP", defaults.m_dynamic_min_ts()) ///< smallest time step allowed

		// matrix sorting
		, m_sort_type(parent, prefix + "SORT_TYPE", defaults.m_sort_type())

		// special
		, m_use_gabs(parent, prefix + "USE_GABS", defaults.m_use_gabs())

		{
			m_min_timestep = m_dynamic_min_ts();
			m_max_timestep = netlist_time::from_fp(plib::reciprocal(m_freq())).as_fp<decltype(m_max_timestep)>();

			if (m_dynamic_ts)
			{
				m_max_timestep *= 1;//NL_FCONST(1000.0);
			}
			else
			{
				m_min_timestep = m_max_timestep;
			}
		}

		param_fp_t m_freq;
		param_fp_t m_gs_sor;
		param_enum_t<matrix_type_e> m_method;
		param_enum_t<matrix_fp_type_e> m_fp_type;
		param_fp_t m_reltol;
		param_fp_t m_vntol;
		param_fp_t m_accuracy;
		param_num_t<std::size_t> m_nr_loops;
		param_num_t<std::size_t> m_gs_loops;
		param_fp_t m_gmin;
		param_logic_t  m_pivot;
		param_fp_t m_nr_recalc_delay;
		param_int_t m_parallel;
		param_fp_t m_min_ts_ts;
		param_logic_t m_dynamic_ts;
		param_fp_t m_dynamic_lte;
		param_fp_t m_dynamic_min_ts;
		param_enum_t<matrix_sort_type_e> m_sort_type;

		param_logic_t m_use_gabs;

		nl_fptype m_min_timestep;
		nl_fptype m_max_timestep;
	};


	class terms_for_net_t
	{
	public:
		terms_for_net_t(analog_net_t * net = nullptr);

		void clear();

		void add_terminal(terminal_t *term, int net_other, bool sorted);

		std::size_t count() const noexcept { return m_terms.size(); }

		std::size_t railstart() const noexcept { return m_railstart; }

		terminal_t **terms() noexcept { return m_terms.data(); }

		nl_fptype getV() const noexcept { return m_net->Q_Analog(); }

		void setV(nl_fptype v) noexcept { m_net->set_Q_Analog(v); }

		bool is_net(const analog_net_t * net) const noexcept { return net == m_net; }

		void set_railstart(std::size_t val) noexcept { m_railstart = val; }

		PALIGNAS_VECTOROPT()

		plib::aligned_vector<unsigned> m_nz;   //!< all non zero for multiplication
		plib::aligned_vector<unsigned> m_nzrd; //!< non zero right of the diagonal for elimination, may include RHS element
		plib::aligned_vector<unsigned> m_nzbd; //!< non zero below of the diagonal for elimination

		plib::aligned_vector<int> m_connected_net_idx;
	private:
		analog_net_t * m_net;
		plib::aligned_vector<terminal_t *> m_terms;
		std::size_t m_railstart;
	};

	class proxied_analog_output_t : public analog_output_t
	{
	public:

		proxied_analog_output_t(core_device_t &dev, const pstring &aname, analog_net_t *pnet)
		: analog_output_t(dev, aname)
		, m_proxied_net(pnet)
		{ }

		analog_net_t *proxied_net() const { return m_proxied_net;}
	private:
		analog_net_t *m_proxied_net; // only for proxy nets in analog input logic
	};

	class matrix_solver_t : public device_t
	{
	public:
		using list_t = std::vector<matrix_solver_t *>;
		using fptype = nl_fptype;
		using arena_type = plib::mempool_arena<plib::aligned_arena, PALIGN_VECTOROPT>;
		using net_list_t =  plib::aligned_vector<analog_net_t *>;

		// after every call to solve, update inputs must be called.
		// this can be done as well as a batch to ease parallel processing.

		netlist_time solve(netlist_time_ext now, const char *source);
		void update_inputs();

		/// \brief Checks if solver may alter a net
		///
		/// This checks if a solver will alter a net. Returns true if the
		/// net is either part of the voltage vector or if it belongs to
		/// the analog input nets connected to the solver.

		bool updates_net(const analog_net_t *net) const noexcept;

		std::size_t dynamic_device_count() const noexcept { return m_dynamic_funcs.size(); }
		std::size_t timestep_device_count() const noexcept { return m_step_funcs.size(); }

		/// \brief reschedule solver execution
		///
		/// Calls reschedule on main solver
		///
		void reschedule(netlist_time ts);

		/// \brief Immediately solve system at current time
		///
		/// This should only be called from update and update_param events.
		/// It's purpose is to bring voltage values to the current timestep.
		/// This will be called BEFORE updating object properties.
		void solve_now()
		{
			// this should only occur outside of execution and thus
			// using time should be safe.

			const netlist_time new_timestep = solve(exec().time(), "solve_now");
			plib::unused_var(new_timestep);

			update_inputs();

			if (timestep_device_count() > 0)
			{
				this->reschedule(netlist_time::from_fp(m_params.m_dynamic_ts ? m_params.m_min_timestep : m_params.m_max_timestep));
			}
		}

		template <typename F>
		void change_state(F f)
		{
			// We only need to update the net first if this is a time stepping net
			if (timestep_device_count() > 0)
			{
				const netlist_time new_timestep = solve(exec().time(), "change_state");
				plib::unused_var(new_timestep);
				update_inputs();
			}
			f();
			if (timestep_device_count() > 0)
			{
				PFDEBUG(printf("here2\n");)
				this->reschedule(netlist_time::from_fp(m_params.m_min_ts_ts()));
			}
			else
				this->reschedule(netlist_time::quantum());
		}

		NETLIB_RESETI();

		virtual void log_stats();

		virtual std::pair<pstring, pstring> create_solver_code(solver::static_compile_target target)
		{
			plib::unused_var(target);
			return std::pair<pstring, pstring>("", plib::pfmt("/* solver doesn't support static compile */\n\n"));
		}

		// return number of floating point operations for solve
		constexpr std::size_t ops() const { return m_ops; }

	protected:
		matrix_solver_t(devices::nld_solver &main_solver, const pstring &name,
			const net_list_t &nets,
			const solver_parameters_t *params);

		virtual void vsolve_non_dynamic() = 0;
		virtual netlist_time compute_next_timestep(fptype cur_ts, fptype min_ts, fptype max_ts) = 0;
		virtual bool check_err() const = 0;
		virtual void store() = 0;
		virtual void backup() = 0;
		virtual void restore() = 0;

		std::size_t max_railstart() const noexcept
		{
			std::size_t max_rail = 0;
			for (std::size_t k = 0; k < m_terms.size(); k++)
				max_rail = std::max(max_rail, m_terms[k].railstart());
			return max_rail;
		}

		const solver_parameters_t &m_params;

		plib::pmatrix2d_vrl<fptype, arena_type>    m_gonn;
		plib::pmatrix2d_vrl<fptype, arena_type>    m_gtn;
		plib::pmatrix2d_vrl<fptype, arena_type>    m_Idrn;
		plib::pmatrix2d_vrl<fptype *, arena_type>  m_connected_net_Vn;

		state_var<std::size_t> m_iterative_fail;
		state_var<std::size_t> m_iterative_total;

		plib::aligned_vector<terms_for_net_t> m_terms; // setup only

	private:

		// base setup - called from constructor
		void setup_base(setup_t &setup, const net_list_t &nets) noexcept(false);

		bool solve_nr_base();
		netlist_time newton_loops_exceeded(netlist_time delta);

		void sort_terms(matrix_sort_type_e sort);

		void update_dynamic() noexcept;
		void step(timestep_type ts_type, netlist_time delta) noexcept;

		int get_net_idx(const analog_net_t *net) const noexcept;
		std::pair<int, int> get_left_right_of_diag(std::size_t irow, std::size_t idiag);
		fptype get_weight_around_diag(std::size_t row, std::size_t diag);

		void add_term(std::size_t net_idx, terminal_t *term) noexcept(false);

		// calculate matrix
		void setup_matrix();

		void set_pointers();

		analog_net_t *get_connected_net(terminal_t *term);

		devices::nld_solver &m_main_solver;

		state_var<std::size_t> m_stat_calculations;
		state_var<std::size_t> m_stat_newton_raphson;
		state_var<std::size_t> m_stat_newton_raphson_fail;
		state_var<std::size_t> m_stat_vsolver_calls;

		state_var<netlist_time_ext> m_last_step;
		plib::aligned_vector<nldelegate_ts> m_step_funcs;
		plib::aligned_vector<nldelegate_dyn> m_dynamic_funcs;
		plib::aligned_vector<device_arena::unique_ptr<proxied_analog_output_t>> m_inps;

		std::size_t m_ops;

		plib::aligned_vector<terms_for_net_t> m_rails_temp; // setup only
	};

} // namespace solver
} // namespace netlist

#endif // NLD_MS_DIRECT_H_
