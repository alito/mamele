// license:GPL-2.0+
// copyright-holders:Couriersud

///
/// \file nl_setup.h
///

#ifndef NLSETUP_H_
#define NLSETUP_H_

#define NL_AUTO_DEVICES 0

#include "plib/ppreprocessor.h"
#include "plib/pstream.h"
#include "plib/pstring.h"
#include "plib/putil.h"

#include "nl_config.h"
#include "nltypes.h"
#include "nl_parser.h"
// FIXME: avoid including factory
//#include "nl_factory.h"

#include <initializer_list>
#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>

//============================================================
//  MACROS / inline netlist definitions
//============================================================

#define NET_STR(x) # x

#define NET_MODEL(model)                                                       \
	setup.register_model(model);

#define ALIAS(alias, name)                                                     \
	setup.register_alias(# alias, # name);

#define DIPPINS(pin1, ...)                                                     \
		setup.register_dip_alias_arr( # pin1 ", " # __VA_ARGS__);

// to be used to reference new library truthtable devices
#define NET_REGISTER_DEV(type, name)                                           \
		setup.register_dev(# type, # name);

// name is first element so that __VA_ARGS__ always has one element
#define NET_REGISTER_DEVEXT(type, ...)                                   \
		setup.register_dev(# type, { PSTRINGIFY_VA(__VA_ARGS__) });

#define NET_CONNECT(name, input, output)                                       \
		setup.register_link(# name "." # input, # output);

#define NET_C(term1, ...)                                                      \
		setup.register_link_arr( # term1 ", " # __VA_ARGS__);

#define PARAM(name, val)                                                       \
		setup.register_param(NET_STR(name), NET_STR(val));

#define DEFPARAM(name, val)                                                       \
		setup.defparam(NET_STR(name), NET_STR(val));

#define HINT(name, val)                                                        \
		setup.register_hint(# name , ".HINT_" # val);

#define NETDEV_PARAMI(name, param, val)                                        \
		setup.register_param(# name "." # param, val);

#define NETLIST_NAME(name) netlist ## _ ## name

#define NETLIST_EXTERNAL(name)                                                 \
		void NETLIST_NAME(name)(netlist::nlparse_t &setup);

#define NETLIST_START(name)                                                    \
void NETLIST_NAME(name)(netlist::nlparse_t &setup)                             \
{                                                                              \
	plib::unused_var(setup);

#define NETLIST_END()  }

#define LOCAL_SOURCE(name)                                                     \
		setup.register_source_proc(# name, &NETLIST_NAME(name));

#define EXTERNAL_SOURCE(name)                                                  \
		setup.register_source_proc(# name, &NETLIST_NAME(name));

// FIXME: Need to pass in parameter definition
#define LOCAL_LIB_ENTRY_1(name)                                                \
		LOCAL_SOURCE(name)                                                     \
		setup.register_lib_entry(# name, "", PSOURCELOC());

#define LOCAL_LIB_ENTRY_2(name, param_spec)                                    \
		LOCAL_SOURCE(name)                                                     \
		setup.register_lib_entry(# name, param_spec, PSOURCELOC());

#define LOCAL_LIB_ENTRY(...) PCALLVARARG(LOCAL_LIB_ENTRY_, __VA_ARGS__)

#define INCLUDE(name)                                                          \
		setup.include(# name);

#define SUBMODEL(model, name)                                                  \
		setup.namespace_push(# name);                                          \
		setup.include(# model);                                                \
		setup.namespace_pop();

#define OPTIMIZE_FRONTIER(attach, r_in, r_out)                                 \
		setup.register_frontier(# attach, PSTRINGIFY_VA(r_in), PSTRINGIFY_VA(r_out));

// -----------------------------------------------------------------------------
// truthtable defines
// -----------------------------------------------------------------------------

#define TRUTHTABLE_START(cname, in, out, pdef_params)                          \
	{ \
		netlist::tt_desc desc;                                                 \
		desc.name = #cname ;                                                   \
		desc.ni = in;                                                          \
		desc.no = out;                                                         \
		desc.family = "";                                                      \
		auto sloc = PSOURCELOC();                                              \
		const pstring def_params = pdef_params;

#define TT_HEAD(x) \
		desc.desc.emplace_back(x);

#define TT_LINE(x) \
		desc.desc.emplace_back(x);

#define TT_FAMILY(x) \
		desc.family = x;

#define TRUTHTABLE_END() \
		setup.truthtable_create(desc, def_params, std::move(sloc)); \
	}

namespace netlist
{

	// -----------------------------------------------------------------------------
	// truthtable desc
	// -----------------------------------------------------------------------------

	struct tt_desc
	{
		tt_desc() : ni(0), no(0) { }
		pstring name;
		unsigned long ni;
		unsigned long no;
		std::vector<pstring> desc;
		pstring family;
	};

	// ----------------------------------------------------------------------------------------
	// static compiled netlist.
	// ----------------------------------------------------------------------------------------

	using nlsetup_func = void (*)(nlparse_t &);

	// ----------------------------------------------------------------------------------------
	// nlparse_t
	// ----------------------------------------------------------------------------------------

	class nlparse_t
	{
	public:
		nlparse_t(log_type &log, detail::abstract_t &abstract);

		void register_model(const pstring &model_in);
		void register_alias(const pstring &alias, const pstring &out);
		void register_alias_nofqn(const pstring &alias, const pstring &out);
		void register_dip_alias_arr(const pstring &terms);

		// last argument only needed by nltool
		void register_dev(const pstring &classname, const pstring &name,
			const std::vector<pstring> &params_and_connections,
			factory::element_t **felem = nullptr);
		void register_dev(const pstring &classname, std::initializer_list<const char *> more_parameters);
		void register_dev(const pstring &classname, const pstring &name)
		{
			register_dev(classname, name, std::vector<pstring>());
		}

		void register_hint(const pstring &objname, const pstring &hintname);

		void register_link(const pstring &sin, const pstring &sout);
		void register_link_arr(const pstring &terms);
		// also called from devices for latebinding connected terminals
		void register_link_fqn(const pstring &sin, const pstring &sout);

		void register_param(const pstring &param, const pstring &value);
		void register_param(const pstring &param, nl_fptype value);

		template <typename T>
		std::enable_if_t<plib::is_arithmetic<T>::value>
		register_param_val(const pstring &param, T value)
		{
			register_param(param, plib::narrow_cast<nl_fptype>(value));
		}

		void register_lib_entry(const pstring &name, const pstring &def_params, plib::source_location &&loc);

		void register_frontier(const pstring &attach, const pstring &r_IN, const pstring &r_OUT);

		// register a source
		template <typename S, typename... Args>
		void register_source(Args&&... args)
		{
			static_assert(std::is_base_of<plib::psource_t, S>::value, "S must inherit from plib::psource_t");

			auto src(std::make_unique<S>(std::forward<Args>(args)...));
			m_sources.add_source(std::move(src));
		}

		void register_source_proc(const pstring &name, nlsetup_func func);

		void truthtable_create(tt_desc &desc, const pstring &def_params, plib::source_location &&loc);

		// handle namespace

		void namespace_push(const pstring &aname);
		void namespace_pop();
		pstring namespace_prefix() const;
		pstring build_fqn(const pstring &obj_name) const;

		// include other files

		void include(const pstring &netlist_name);

		// used from netlist.cpp (mame)
		bool device_exists(const pstring &name) const;

		// FIXME: used by source_t - need a different approach at some time
		bool parse_stream(plib::psource_t::stream_ptr &&istrm, const pstring &name);
		bool parse_tokens(const parser_t::token_store &tokens, const pstring &name);

		template <typename S, typename... Args>
		void add_include(Args&&... args)
		{
			static_assert(std::is_base_of<plib::psource_t, S>::value, "S must inherit from plib::psource_t");

			auto src(std::make_unique<S>(std::forward<Args>(args)...));
			m_includes.add_source(std::move(src));
		}

		void add_define(const pstring &def, const pstring &val)
		{
			m_defines.insert({ def, plib::ppreprocessor::define_t(def, val)});
		}

		void add_define(const pstring &defstr);

		// DEFPARAM support
		void defparam(const pstring &name, const pstring &def);

		// register a list of logs
		void register_dynamic_log_devices(const std::vector<pstring> &loglist);

		factory::list_t &factory() noexcept;
		const factory::list_t &factory() const noexcept;

		log_type &log() noexcept { return m_log; }
		const log_type &log() const noexcept { return m_log; }

		plib::psource_t::stream_ptr get_data_stream(const pstring &name);

	private:
		// FIXME: stale? - remove later
		void remove_connections(const pstring &pin);

		plib::ppreprocessor::defines_map_type       m_defines;
		plib::psource_collection_t<>                m_includes;
		std::stack<pstring>                         m_namespace_stack;
		plib::psource_collection_t<>                m_sources;
		detail::abstract_t &                        m_abstract;

		std::unordered_map<pstring, parser_t::token_store>    m_source_cache;
		log_type &m_log;
		unsigned m_frontier_cnt;
	};

	// -----------------------------------------------------------------------------
	// inline implementations
	// -----------------------------------------------------------------------------

} // namespace netlist


#endif // NLSETUP_H_
