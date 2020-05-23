// license:GPL-2.0+
// copyright-holders:Couriersud

#include "solver/nld_matrix_solver.h"
#include "solver/nld_solver.h"

#include "plib/palloc.h"
#include "plib/pfmtlog.h"
#include "plib/pmempool.h"
#include "plib/putil.h"

#include "devices/nlid_proxy.h"
#include "devices/nlid_system.h"
#include "macro/nlm_base.h"
#include "nl_base.h"

#include "nl_errstr.h"

#include <limits>

// ----------------------------------------------------------------------------------------
// Statically compiled solvers for mame netlist
// ----------------------------------------------------------------------------------------

namespace netlist
{

	plib::unique_ptr<plib::dynlib_base> callbacks_t:: static_solver_lib() const
	{
		return plib::make_unique<plib::dynlib_static>(nullptr);
	}

	// ----------------------------------------------------------------------------------------
	// logic_family_ttl_t
	// ----------------------------------------------------------------------------------------

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, modernize-use-equals-default)
	logic_family_desc_t::logic_family_desc_t()
	{
	}

	class logic_family_ttl_t : public logic_family_desc_t
	{
	public:
		logic_family_ttl_t() : logic_family_desc_t()
		{
			m_low_thresh_PCNT = nlconst::magic(0.8 / 5.0);
			m_high_thresh_PCNT = nlconst::magic(2.0 / 5.0);
			// m_low_V  - these depend on sinked/sourced current. Values should be suitable for typical applications.
			m_low_VO = nlconst::magic(0.1);
			m_high_VO = nlconst::magic(1.0); // 4.0
			m_R_low = nlconst::magic(1.0);
			m_R_high = nlconst::magic(130.0);
		}
		unique_pool_ptr<devices::nld_base_d_to_a_proxy> create_d_a_proxy(netlist_state_t &anetlist, const pstring &name, logic_output_t *proxied) const override;
		unique_pool_ptr<devices::nld_base_a_to_d_proxy> create_a_d_proxy(netlist_state_t &anetlist, const pstring &name, logic_input_t *proxied) const override;
	};

	unique_pool_ptr<devices::nld_base_d_to_a_proxy> logic_family_ttl_t::create_d_a_proxy(netlist_state_t &anetlist, const pstring &name, logic_output_t *proxied) const
	{
		return anetlist.make_object<devices::nld_d_to_a_proxy>(anetlist, name, proxied);
	}
	unique_pool_ptr<devices::nld_base_a_to_d_proxy> logic_family_ttl_t::create_a_d_proxy(netlist_state_t &anetlist, const pstring &name, logic_input_t *proxied) const
	{
		return anetlist.make_object<devices::nld_a_to_d_proxy>(anetlist, name, proxied);
	}

	class logic_family_cd4xxx_t : public logic_family_desc_t
	{
	public:
		logic_family_cd4xxx_t() : logic_family_desc_t()
		{
			m_low_thresh_PCNT = nlconst::magic(1.5 / 5.0);
			m_high_thresh_PCNT = nlconst::magic(3.5 / 5.0);
			// m_low_V  - these depend on sinked/sourced current. Values should be suitable for typical applications.
			m_low_VO = nlconst::magic(0.05);
			m_high_VO = nlconst::magic(0.05); // 4.95
			// https://www.classe.cornell.edu/~ib38/teaching/p360/lectures/wk09/l26/EE2301Exp3F10.pdf
			// typical CMOS may sink 0.4mA while output stays <= 0.4V
			m_R_low = nlconst::magic(500.0);
			m_R_high = nlconst::magic(500.0);
		}
		unique_pool_ptr<devices::nld_base_d_to_a_proxy> create_d_a_proxy(netlist_state_t &anetlist, const pstring &name, logic_output_t *proxied) const override;
		unique_pool_ptr<devices::nld_base_a_to_d_proxy> create_a_d_proxy(netlist_state_t &anetlist, const pstring &name, logic_input_t *proxied) const override;
	};

	unique_pool_ptr<devices::nld_base_d_to_a_proxy> logic_family_cd4xxx_t::create_d_a_proxy(netlist_state_t &anetlist, const pstring &name, logic_output_t *proxied) const
	{
		return anetlist.make_object<devices::nld_d_to_a_proxy>(anetlist, name, proxied);
	}

	unique_pool_ptr<devices::nld_base_a_to_d_proxy> logic_family_cd4xxx_t::create_a_d_proxy(netlist_state_t &anetlist, const pstring &name, logic_input_t *proxied) const
	{
		return anetlist.make_object<devices::nld_a_to_d_proxy>(anetlist, name, proxied);
	}

	const logic_family_desc_t *family_TTL()
	{
		static logic_family_ttl_t obj;
		return &obj;
	}
	const logic_family_desc_t *family_CD4XXX()
	{
		static logic_family_cd4xxx_t obj;
		return &obj;
	}


	// ----------------------------------------------------------------------------------------
	// queue_t
	// ----------------------------------------------------------------------------------------

	detail::queue_t::queue_t(netlist_t &nl)
		: timed_queue<plib::pqentry_t<net_t *, netlist_time_ext>, false>(512)
		, netlist_object_t(nl, nl.nlstate().name() + ".queue")
		, m_qsize(0)
		, m_times(512)
		, m_net_ids(512)
	{
	}

	void detail::queue_t::register_state(plib::state_manager_t &manager, const pstring &module)
	{
		//state().log().debug("register_state\n");
		manager.save_item(this, m_qsize, module + "." + "qsize");
		manager.save_item(this, &m_times[0], module + "." + "times", m_times.size());
		manager.save_item(this, &m_net_ids[0], module + "." + "names", m_net_ids.size());
	}

	void detail::queue_t::on_pre_save(plib::state_manager_t &manager)
	{
		plib::unused_var(manager);
		m_qsize = this->size();
		for (std::size_t i = 0; i < m_qsize; i++ )
		{
			m_times[i] =  this->listptr()[i].exec_time().as_raw();
			m_net_ids[i] = state().find_net_id(this->listptr()[i].object());
		}
	}

	void detail::queue_t::on_post_load(plib::state_manager_t &manager)
	{
		plib::unused_var(manager);
		this->clear();
		for (std::size_t i = 0; i < m_qsize; i++ )
		{
			detail::net_t *n = state().nets()[m_net_ids[i]].get();
			this->push<false>(queue_t::entry_t(netlist_time_ext::from_raw(m_times[i]),n));
		}
	}

#if 0
	// ----------------------------------------------------------------------------------------
	// netlist_ref_t
	// ----------------------------------------------------------------------------------------

	detail::netlist_ref::netlist_ref(netlist_t &nl)
	: m_netlist(nl) { }
#endif
	// ----------------------------------------------------------------------------------------
	// device_object_t
	// ----------------------------------------------------------------------------------------

	detail::device_object_t::device_object_t(core_device_t &dev, const pstring &aname)
	: object_t(aname)
	, m_device(dev)
	{
	}

	detail::terminal_type detail::core_terminal_t::type() const noexcept(false)
	{
		if (dynamic_cast<const terminal_t *>(this) != nullptr)
			return terminal_type::TERMINAL;
		if (dynamic_cast<const logic_input_t *>(this) != nullptr
			|| dynamic_cast<const analog_input_t *>(this) != nullptr)
			return terminal_type::INPUT;
		if (dynamic_cast<const logic_output_t *>(this) != nullptr
			|| dynamic_cast<const analog_output_t *>(this) != nullptr)
			return terminal_type::OUTPUT;

		state().log().fatal(MF_UNKNOWN_TYPE_FOR_OBJECT(name()));
		throw nl_exception(MF_UNKNOWN_TYPE_FOR_OBJECT(name()));
		//return terminal_type::TERMINAL; // please compiler
	}

	// ----------------------------------------------------------------------------------------
	// netlist_t
	// ----------------------------------------------------------------------------------------

	netlist_t::netlist_t(netlist_state_t &state)
		: m_state(state)
		, m_solver(nullptr)
		, m_time(netlist_time_ext::zero())
		, m_mainclock(nullptr)
		, m_queue(*this)
		, m_use_stats(false)
	{
		state.run_state_manager().save_item(this, static_cast<plib::state_manager_t::callback_t &>(m_queue), "m_queue");
		state.run_state_manager().save_item(this, m_time, "m_time");
	}

	// ----------------------------------------------------------------------------------------
	// netlist_t
	// ----------------------------------------------------------------------------------------

	netlist_state_t::netlist_state_t(const pstring &aname,
		plib::unique_ptr<callbacks_t> &&callbacks)
	: m_name(aname)
	, m_callbacks(std::move(callbacks)) // Order is important here
	, m_log(*m_callbacks)
	, m_extended_validation(false)
	, m_dummy_version(1)
	{

		m_lib = m_callbacks->static_solver_lib();

		m_setup = plib::make_unique<setup_t>(*this);
		// create the run interface
		m_netlist = m_pool.make_unique<netlist_t>(*this);

		// Make sure save states are invalidated when a new version is deployed

		m_state.save_item(this, m_dummy_version, pstring("V") + version());

		// Initialize factory
		devices::initialize_factory(m_setup->factory());

		// Add default include file
		using a = plib::psource_str_t<plib::psource_t>;
		const pstring content =
		"#define RES_R(res) (res)            \n"
		"#define RES_K(res) ((res) * 1e3)    \n"
		"#define RES_M(res) ((res) * 1e6)    \n"
		"#define CAP_U(cap) ((cap) * 1e-6)   \n"
		"#define CAP_N(cap) ((cap) * 1e-9)   \n"
		"#define CAP_P(cap) ((cap) * 1e-12)  \n"
		"#define IND_U(ind) ((ind) * 1e-6)   \n"
		"#define IND_N(ind) ((ind) * 1e-9)   \n"
		"#define IND_P(ind) ((ind) * 1e-12)  \n";
		setup().add_include<a>("netlist/devices/net_lib.h", content);
		NETLIST_NAME(base)(*m_setup);
	}


	void netlist_t::stop()
	{
		log().debug("Printing statistics ...\n");
		print_stats();
		log().debug("Stopping solver device ...\n");
		if (m_solver != nullptr)
			m_solver->stop();
	}

	detail::net_t *netlist_state_t::find_net(const pstring &name) const
	{
		for (const auto & net : m_nets)
			if (net->name() == name)
				return net.get();

		return nullptr;
	}

	std::size_t netlist_state_t::find_net_id(const detail::net_t *net) const
	{
		for (std::size_t i = 0; i < m_nets.size(); i++)
			if (m_nets[i].get() == net)
				return i;
		return std::numeric_limits<std::size_t>::max();
	}

	void netlist_state_t::rebuild_lists()
	{
		for (auto & net : m_nets)
			net->rebuild_list();
	}

	void netlist_state_t::compile_defines(std::vector<std::pair<pstring, pstring>> &defs)
	{
	#define ENTRY(x) if (pstring(#x) != PSTRINGIFY(x)) defs.emplace_back(std::pair<pstring, pstring>(#x, PSTRINGIFY(x)));
		ENTRY(NL_VERSION_MAJOR)
		ENTRY(NL_VERSION_MINOR)
		ENTRY(NL_VERSION_PATCHLEVEL)

		ENTRY(PUSE_ACCURATE_STATS)
		ENTRY(PHAS_INT128)
		ENTRY(PUSE_ALIGNED_OPTIMIZATIONS)
		ENTRY(PHAS_OPENMP)
		ENTRY(PUSE_OPENMP)
		ENTRY(PUSE_FLOAT128)
		ENTRY(PPMF_TYPE)
		ENTRY(PHAS_PMF_INTERNAL)
		ENTRY(NL_USE_MEMPOOL)
		ENTRY(NL_USE_QUEUE_STATS)
		ENTRY(NL_USE_COPY_INSTEAD_OF_REFERENCE)
		ENTRY(NL_USE_TRUTHTABLE_7448)
		ENTRY(NL_AUTO_DEVICES)
		ENTRY(NL_USE_FLOAT128)
		ENTRY(NL_USE_FLOAT_MATRIX)
		ENTRY(NL_USE_LONG_DOUBLE_MATRIX)
		ENTRY(NL_DEBUG)
		ENTRY(NVCCBUILD)

		ENTRY(__cplusplus)
		ENTRY(__VERSION__)

		ENTRY(__GNUC__)
		ENTRY(__GNUC_MINOR__)
		ENTRY(__GNUC_PATCHLEVEL__)

		ENTRY(__clang__)
		ENTRY(__clang_major__)
		ENTRY(__clang_minor__)
		ENTRY(__clang_patchlevel__)
		ENTRY(__clang_version__)

		ENTRY(OPENMP )
		ENTRY(_OPENMP )

		ENTRY(__x86_64__ )
		ENTRY(__i386__)
		ENTRY(_WIN32)
		ENTRY(_MSC_VER)
		ENTRY(__APPLE__)
		ENTRY(__unix__)
		ENTRY(__linux__)

	#undef ENTRY
	}

	pstring netlist_state_t::version()
	{
		return plib::pfmt("{1}.{2}")(NL_VERSION_MAJOR, NL_VERSION_MINOR);
	}

	pstring netlist_state_t::version_patchlevel()
	{
		return plib::pfmt("{1}.{2}.{3}")(NL_VERSION_MAJOR, NL_VERSION_MINOR, NL_VERSION_PATCHLEVEL);
	}

	void netlist_t::reset()
	{
		log().debug("Searching for mainclock\n");
		m_mainclock = m_state.get_single_device<devices::NETLIB_NAME(mainclock)>("mainclock");

		log().debug("Searching for solver\n");
		m_solver = m_state.get_single_device<devices::NETLIB_NAME(solver)>("solver");

		m_time = netlist_time_ext::zero();
		m_queue.clear();
		if (m_mainclock != nullptr)
			m_mainclock->m_Q.net().set_next_scheduled_time(netlist_time_ext::zero());
		//if (m_solver != nullptr)
		//  m_solver->reset();

		m_state.reset();
	}

	void netlist_state_t::reset()
	{
		//FIXME: never used ???
		std::unordered_map<core_device_t *, bool> m;

		// Reset all nets once !
		log().verbose("Call reset on all nets:");
		for (auto & n : nets())
			n->reset();

		// Reset all devices once !
		log().verbose("Call reset on all devices:");
		for (auto & dev : m_devices)
			dev.second->reset();

		// Make sure everything depending on parameters is set
		// Currently analog input and logic input also
		// push their outputs to queue.

		log().verbose("Call update_param on all devices:");
		for (auto & dev : m_devices)
			dev.second->update_param();

		// Step all devices once !
		//
		// INFO: The order here affects power up of e.g. breakout. However, such
		// variations are explicitly stated in the breakout manual.

		auto *netlist_params = get_single_device<devices::NETLIB_NAME(netlistparams)>("parameter");

		switch (netlist_params->m_startup_strategy())
		{
			case 0:
			{
				std::vector<core_device_t *> d;
				std::vector<nldelegate *> t;
				log().verbose("Using default startup strategy");
				for (auto &n : m_nets)
					for (auto & term : n->core_terms())
						if (term->delegate().has_object())
						{
							if (!plib::container::contains(t, &term->delegate()))
							{
								t.push_back(&term->delegate());
								term->run_delegate();
							}
							auto *dev = reinterpret_cast<core_device_t *>(term->delegate().object());
							if (!plib::container::contains(d, dev))
								d.push_back(dev);
						}
				log().verbose("Devices not yet updated:");
				for (auto &dev : m_devices)
					if (!plib::container::contains(d, dev.second.get()))
					{
						log().verbose("\t ...{1}", dev.second->name());
						dev.second->update();
					}
			}
			break;
			case 1:     // brute force backward
			{
				log().verbose("Using brute force backward startup strategy");

				for (auto &n : m_nets)  // only used if USE_COPY_INSTEAD_OF_REFERENCE == 1
					n->update_inputs();

				std::size_t i = m_devices.size();
				while (i>0)
					m_devices[--i].second->update();

				for (auto &n : m_nets)  // only used if USE_COPY_INSTEAD_OF_REFERENCE == 1
					n->update_inputs();

			}
			break;
			case 2:     // brute force forward
			{
				log().verbose("Using brute force forward startup strategy");
				for (auto &d : m_devices)
					d.second->update();
			}
			break;
		}

	#if 1
		// the above may screw up m_active and the list
		rebuild_lists();
	#endif
	}


	void netlist_t::print_stats() const
	{
		if (m_use_stats)
		{
			std::vector<size_t> index;
			for (size_t i=0; i < m_state.m_devices.size(); i++)
				index.push_back(i);

			std::sort(index.begin(), index.end(),
					[&](size_t i1, size_t i2) { return m_state.m_devices[i1].second->m_stats->m_stat_total_time.total() < m_state.m_devices[i2].second->m_stats->m_stat_total_time.total(); });

			plib::pperftime_t<true>::type total_time(0);
			plib::pperftime_t<true>::ctype total_count(0);

			for (auto & j : index)
			{
				auto *entry = m_state.m_devices[j].second.get();
				auto *stats = entry->m_stats.get();
				log().verbose("Device {1:20} : {2:12} {3:12} {4:15} {5:12}", entry->name(),
						stats->m_stat_call_count(), stats->m_stat_total_time.count(),
						stats->m_stat_total_time.total(), stats->m_stat_inc_active());
				total_time += stats->m_stat_total_time.total();
				total_count += stats->m_stat_total_time.count();
			}

			log().verbose("Total calls : {1:12} {2:12} {3:12}", total_count,
				total_time, total_time / static_cast<decltype(total_time)>(total_count ? total_count : 1));

			log().verbose("Total loop     {1:15}", m_stat_mainloop());
			log().verbose("Total time     {1:15}", total_time);

			// FIXME: clang complains about unreachable code without
			const auto clang_workaround_unreachable_code = NL_USE_QUEUE_STATS;
			if (clang_workaround_unreachable_code)
			{
				// Only one serialization should be counted in total time
				// But two are contained in m_stat_mainloop
				plib::pperftime_t<true> overhead;
				plib::pperftime_t<true> test;
				{
					auto overhead_guard(overhead.guard());
					for (int j=0; j<100000;j++)
					{
						auto test_guard(test.guard());
					}
				}

				plib::pperftime_t<true>::type total_overhead = overhead()
						* static_cast<plib::pperftime_t<true>::type>(total_count)
						/ static_cast<plib::pperftime_t<true>::type>(200000);

				log().verbose("Queue Pushes   {1:15}", m_queue.m_prof_call());
				log().verbose("Queue Moves    {1:15}", m_queue.m_prof_sortmove());
				log().verbose("Queue Removes  {1:15}", m_queue.m_prof_remove());
				log().verbose("Queue Retimes  {1:15}", m_queue.m_prof_retime());
				log().verbose("");

				log().verbose("Take the next lines with a grain of salt. They depend on the measurement implementation.");
				log().verbose("Total overhead {1:15}", total_overhead);
				plib::pperftime_t<true>::type overhead_per_pop = (m_stat_mainloop()-2*total_overhead - (total_time - total_overhead))
						/ static_cast<plib::pperftime_t<true>::type>(m_queue.m_prof_call());
				log().verbose("Overhead per pop  {1:11}", overhead_per_pop );
				log().verbose("");
			}

			auto trigger = total_count * 200 / 1000000; // 200 ppm
			for (auto &entry : m_state.m_devices)
			{
				auto *ep = entry.second.get();
				auto *stats = ep->m_stats.get();
				// Factor of 3 offers best performace increase
				if (stats->m_stat_inc_active() > 3 * stats->m_stat_total_time.count()
					&& stats->m_stat_inc_active() > trigger)
					log().verbose("HINT({}, NO_DEACTIVATE) // {} {} {}", ep->name(),
						static_cast<nl_fptype>(stats->m_stat_inc_active()) / static_cast<nl_fptype>(stats->m_stat_total_time.count()),
						stats->m_stat_inc_active(), stats->m_stat_total_time.count());
			}
			log().verbose("");
		}
		log().verbose("Current pool memory allocated: {1:12} kB", nlstate().pool().cur_alloc() >> 10);
		log().verbose("Maximum pool memory allocated: {1:12} kB", nlstate().pool().max_alloc() >> 10);
	}

	core_device_t *netlist_state_t::get_single_device(const pstring &classname, bool (*cc)(core_device_t *)) const
	{
		core_device_t *ret = nullptr;
		for (const auto &d : m_devices)
		{
			if (cc(d.second.get()))
			{
				if (ret != nullptr)
				{
					m_log.fatal(MF_MORE_THAN_ONE_1_DEVICE_FOUND(classname));
					throw nl_exception(MF_MORE_THAN_ONE_1_DEVICE_FOUND(classname));
				}
				ret = d.second.get();
			}
		}
		return ret;
	}


	// ----------------------------------------------------------------------------------------
	// core_device_t
	// ----------------------------------------------------------------------------------------

	core_device_t::core_device_t(netlist_state_t &owner, const pstring &name)
		: netlist_object_t(owner.exec(), name)
		, m_hint_deactivate(false)
		, m_active_outputs(*this, "m_active_outputs", 1)
	{
		// FIXME: logic_family should always be nullptr here
		if (logic_family() == nullptr)
			set_logic_family(family_TTL());
		if (exec().stats_enabled())
			m_stats = owner.make_object<stats_t>();
	}

	core_device_t::core_device_t(core_device_t &owner, const pstring &name)
		: netlist_object_t(owner.state().exec(), owner.name() + "." + name)
		, m_hint_deactivate(false)
		, m_active_outputs(*this, "m_active_outputs", 1)
	{
		set_logic_family(owner.logic_family());
		//printf("%s %f %f\n", this->name().c_str(), logic_family()->R_low(), logic_family()->R_high());
		if (logic_family() == nullptr)
			set_logic_family(family_TTL());
		owner.state().register_device(this->name(), owned_pool_ptr<core_device_t>(this, false));
		if (exec().stats_enabled())
			m_stats = owner.state().make_object<stats_t>();
	}

	void core_device_t::set_default_delegate(detail::core_terminal_t &term)
	{
		if (!term.delegate().is_set())
			term.set_delegate(nldelegate(&core_device_t::update, this));
	}

	log_type & core_device_t::log()
	{
		return state().log();
	}

	// ----------------------------------------------------------------------------------------
	// device_t
	// ----------------------------------------------------------------------------------------

	device_t::device_t(netlist_state_t &owner, const pstring &name)
	: core_device_t(owner, name)
	{
	}

	device_t::device_t(core_device_t &owner, const pstring &name)
	: core_device_t(owner, name)
	{
	}

	void device_t::register_subalias(const pstring &name, detail::core_terminal_t &term)
	{
		pstring alias = this->name() + "." + name;

		// everything already fully qualified
		state().setup().register_alias_nofqn(alias, term.name());
	}

	void device_t::register_subalias(const pstring &name, const pstring &aliased)
	{
		pstring alias = this->name() + "." + name;
		pstring aliased_fqn = this->name() + "." + aliased;

		// everything already fully qualified
		state().setup().register_alias_nofqn(alias, aliased_fqn);
	}

	void device_t::connect(const detail::core_terminal_t &t1, const detail::core_terminal_t &t2)
	{
		state().setup().register_link_fqn(t1.name(), t2.name());
	}

	void device_t::connect(const pstring &t1, const pstring &t2)
	{
		state().setup().register_link_fqn(name() + "." + t1, name() + "." + t2);
	}

	// FIXME: this is only used by solver code since matrix solvers are started in
	//        post_start.
	void device_t::connect_post_start(detail::core_terminal_t &t1, detail::core_terminal_t &t2)
	{
		if (!state().setup().connect(t1, t2))
		{
			log().fatal(MF_ERROR_CONNECTING_1_TO_2(t1.name(), t2.name()));
			throw nl_exception(MF_ERROR_CONNECTING_1_TO_2(t1.name(), t2.name()));
		}
	}


	// -----------------------------------------------------------------------------
	// family_setter_t
	// -----------------------------------------------------------------------------

	// NOLINTNEXTLINE(modernize-use-equals-default)
	detail::family_setter_t::family_setter_t()
	{
	}

	detail::family_setter_t::family_setter_t(core_device_t &dev, const pstring &desc)
	{
		dev.set_logic_family(dev.state().setup().family_from_model(desc));
	}

	detail::family_setter_t::family_setter_t(core_device_t &dev, const logic_family_desc_t &desc)
	{
		dev.set_logic_family(&desc);
	}

	// ----------------------------------------------------------------------------------------
	// net_t
	// ----------------------------------------------------------------------------------------

	detail::net_t::net_t(netlist_state_t &nl, const pstring &aname, core_terminal_t *railterminal)
		: netlist_object_t(nl.exec(), aname)
		, m_new_Q(*this, "m_new_Q", 0)
		, m_cur_Q (*this, "m_cur_Q", 0)
		, m_in_queue(*this, "m_in_queue", queue_status::DELIVERED)
		, m_next_scheduled_time(*this, "m_time", netlist_time_ext::zero())
		, m_railterminal(railterminal)
	{
		props::add(this, props::value_type());
	}

	void detail::net_t::rebuild_list()
	{
		// rebuild m_list

		m_list_active.clear();
		for (auto & term : core_terms())
			if (term->terminal_state() != logic_t::STATE_INP_PASSIVE)
			{
				m_list_active.push_back(term);
				term->set_copied_input(m_cur_Q);
			}
	}


	void detail::net_t::reset() noexcept
	{
		m_next_scheduled_time = netlist_time_ext::zero();
		m_in_queue = queue_status::DELIVERED;

		m_new_Q = 0;
		m_cur_Q = 0;

		auto *p = dynamic_cast<analog_net_t *>(this);

		if (p != nullptr)
			p->m_cur_Analog = nlconst::zero();

		// rebuild m_list and reset terminals to active or analog out state

		m_list_active.clear();
		for (core_terminal_t *ct : core_terms())
		{
			ct->reset();
			if (ct->terminal_state() != logic_t::STATE_INP_PASSIVE)
				m_list_active.push_back(ct);
			ct->set_copied_input(m_cur_Q);
		}
	}

	void detail::net_t::add_terminal(detail::core_terminal_t &terminal) noexcept(false)
	{
		for (auto &t : core_terms())
			if (t == &terminal)
			{
				state().log().fatal(MF_NET_1_DUPLICATE_TERMINAL_2(name(), t->name()));
				throw nl_exception(MF_NET_1_DUPLICATE_TERMINAL_2(name(), t->name()));
			}

		terminal.set_net(this);

		core_terms().push_back(&terminal);
	}

	void detail::net_t::remove_terminal(detail::core_terminal_t &terminal) noexcept(false)
	{
		if (plib::container::contains(core_terms(), &terminal))
		{
			terminal.set_net(nullptr);
			plib::container::remove(core_terms(), &terminal);
		}
		else
		{
			state().log().fatal(MF_REMOVE_TERMINAL_1_FROM_NET_2(terminal.name(), this->name()));
			throw nl_exception(MF_REMOVE_TERMINAL_1_FROM_NET_2(terminal.name(), this->name()));
		}
	}

	void detail::net_t::move_connections(detail::net_t &dest_net)
	{
		for (auto &ct : core_terms())
			dest_net.add_terminal(*ct);
		core_terms().clear();
	}

	// ----------------------------------------------------------------------------------------
	// logic_net_t
	// ----------------------------------------------------------------------------------------

	logic_net_t::logic_net_t(netlist_state_t &nl, const pstring &aname, detail::core_terminal_t *railterminal)
		: net_t(nl, aname, railterminal)
	{
	}

	// ----------------------------------------------------------------------------------------
	// analog_net_t
	// ----------------------------------------------------------------------------------------

	analog_net_t::analog_net_t(netlist_state_t &nl, const pstring &aname, detail::core_terminal_t *railterminal)
		: net_t(nl, aname, railterminal)
		, m_cur_Analog(*this, "m_cur_Analog", nlconst::zero())
		, m_solver(nullptr)
	{
	}

	// ----------------------------------------------------------------------------------------
	// core_terminal_t
	// ----------------------------------------------------------------------------------------

	detail::core_terminal_t::core_terminal_t(core_device_t &dev, const pstring &aname,
			const state_e state, nldelegate delegate)
	: device_object_t(dev, dev.name() + "." + aname)
	#if NL_USE_COPY_INSTEAD_OF_REFERENCE
	, m_Q(*this, "m_Q", 0)
	#endif
	, m_delegate(delegate)
	, m_net(nullptr)
	, m_state(*this, "m_state", state)
	{
	}

	analog_t::analog_t(core_device_t &dev, const pstring &aname, const state_e state,
		nldelegate delegate)
	: core_terminal_t(dev, aname, state, delegate)
	{
	}

	logic_t::logic_t(core_device_t &dev, const pstring &aname, const state_e state,
			nldelegate delegate)
		: core_terminal_t(dev, aname, state, delegate)
	{
	}

	// ----------------------------------------------------------------------------------------
	// terminal_t
	// ----------------------------------------------------------------------------------------

	terminal_t::terminal_t(core_device_t &dev, const pstring &aname, terminal_t *otherterm)
	: analog_t(dev, aname, STATE_BIDIR)
	, m_Idr(nullptr)
	, m_go(nullptr)
	, m_gt(nullptr)
	{
		state().setup().register_term(*this, *otherterm);
	}

	void terminal_t::solve_now()
	{
		const auto *solv(solver());
		// Nets may belong to railnets which do not have a solver attached
		if (solv)
				solver()->solve_now();
	}

	// ----------------------------------------------------------------------------------------
	// net_input_t
	// ----------------------------------------------------------------------------------------

	// ----------------------------------------------------------------------------------------
	// net_output_t
	// ----------------------------------------------------------------------------------------

	// ----------------------------------------------------------------------------------------
	// logic_output_t
	// ----------------------------------------------------------------------------------------

	logic_output_t::logic_output_t(core_device_t &dev, const pstring &aname)
		: logic_t(dev, aname, STATE_OUT)
		, m_my_net(dev.state(), name() + ".net", this)
	{
		this->set_net(&m_my_net);
		state().register_net(owned_pool_ptr<logic_net_t>(&m_my_net, false));
		set_logic_family(dev.logic_family());
		state().setup().register_term(*this);
	}

	void logic_output_t::initial(const netlist_sig_t val) noexcept
	{
		if (has_net())
			net().initial(val);
	}

	// ----------------------------------------------------------------------------------------
	// analog_input_t
	// ----------------------------------------------------------------------------------------

	analog_input_t::analog_input_t(core_device_t &dev, const pstring &aname,
		nldelegate delegate)
	: analog_t(dev, aname, STATE_INP_ACTIVE, delegate)
	{
		state().setup().register_term(*this);
	}

	// ----------------------------------------------------------------------------------------
	// analog_output_t
	// ----------------------------------------------------------------------------------------

	analog_output_t::analog_output_t(core_device_t &dev, const pstring &aname)
		: analog_t(dev, aname, STATE_OUT)
		, m_my_net(dev.state(), name() + ".net", this)
	{
		state().register_net(owned_pool_ptr<analog_net_t>(&m_my_net, false));
		this->set_net(&m_my_net);

		//net().m_cur_Analog = NL_FCONST(0.0);
		state().setup().register_term(*this);
	}

	void analog_output_t::initial(nl_fptype val) noexcept
	{
		net().set_Q_Analog(val);
	}

	// -----------------------------------------------------------------------------
	// logic_input_t
	// -----------------------------------------------------------------------------

	logic_input_t::logic_input_t(core_device_t &dev, const pstring &aname,
			nldelegate delegate)
			: logic_t(dev, aname, STATE_INP_ACTIVE, delegate)
	{
		set_logic_family(dev.logic_family());
		state().setup().register_term(*this);
	}

	// ----------------------------------------------------------------------------------------
	// Parameters ...
	// ----------------------------------------------------------------------------------------

	param_t::param_t(device_t &device, const pstring &name)
		: device_object_t(device, device.name() + "." + name)
	{
		device.state().setup().register_param_t(this->name(), *this);
	}

	param_t::param_type_t param_t::param_type() const noexcept(false)
	{
		if (dynamic_cast<const param_str_t *>(this) != nullptr)
			return STRING;
		if (dynamic_cast<const param_fp_t *>(this) != nullptr)
			return DOUBLE;
		if (dynamic_cast<const param_int_t *>(this) != nullptr)
			return INTEGER;
		if (dynamic_cast<const param_logic_t *>(this) != nullptr)
			return LOGIC;
		if (dynamic_cast<const param_ptr_t *>(this) != nullptr)
			return POINTER;

		state().log().fatal(MF_UNKNOWN_PARAM_TYPE(name()));
		throw nl_exception(MF_UNKNOWN_PARAM_TYPE(name()));
	}


	pstring param_t::get_initial(const device_t &dev, bool *found) const
	{
		pstring res = dev.state().setup().get_initial_param_val(this->name(), "");
		*found = (res != "");
		return res;
	}

	pstring param_model_t::type()
	{
		return state().setup().models().type(str());
	}

	param_str_t::param_str_t(device_t &device, const pstring &name, const pstring &val)
	: param_t(device, name)
	{
		m_param = device.state().setup().get_initial_param_val(this->name(),val);
	}

	void param_str_t::changed() noexcept
	{
	}

	param_ptr_t::param_ptr_t(device_t &device, const pstring &name, uint8_t * val)
	: param_t(device, name)
	{
		m_param = val; //device.setup().get_initial_param_val(this->name(),val);
		//netlist().save(*this, m_param, "m_param");
	}

	void param_model_t::changed() noexcept
	{
	}

	pstring param_model_t::value_str(const pstring &entity)
	{
		return state().setup().models().value_str(str(), entity);
	}

	nl_fptype param_model_t::value(const pstring &entity)
	{
		return state().setup().models().value(str(), entity);
	}


	plib::unique_ptr<std::istream> param_data_t::stream()
	{
		return device().state().setup().get_data_stream(str());
	}

	bool detail::core_terminal_t::is_logic() const noexcept
	{
		return dynamic_cast<const logic_t *>(this) != nullptr;
	}

	bool detail::core_terminal_t::is_logic_input() const noexcept
	{
		return dynamic_cast<const logic_input_t *>(this) != nullptr;
	}

	bool detail::core_terminal_t::is_logic_output() const noexcept
	{
		return dynamic_cast<const logic_output_t *>(this) != nullptr;
	}

	bool detail::core_terminal_t::is_analog() const noexcept
	{
		return dynamic_cast<const analog_t *>(this) != nullptr;
	}

	bool detail::core_terminal_t::is_analog_input() const noexcept
	{
		return dynamic_cast<const analog_input_t *>(this) != nullptr;
	}

	bool detail::core_terminal_t::is_analog_output() const noexcept
	{
		return dynamic_cast<const analog_output_t *>(this) != nullptr;
	}


	bool detail::net_t::is_logic() const noexcept
	{
		return dynamic_cast<const logic_net_t *>(this) != nullptr;
	}

	bool detail::net_t::is_analog() const noexcept
	{
		return dynamic_cast<const analog_net_t *>(this) != nullptr;
	}

	// ----------------------------------------------------------------------------------------
	// netlist_state_t
	// ----------------------------------------------------------------------------------------

} // namespace netlist
