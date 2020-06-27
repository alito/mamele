// license:GPL-2.0+
// copyright-holders:Couriersud

#include "plib/palloc.h"
#include "analog/nld_twoterm.h"
#include "devices/nlid_proxy.h"
#include "devices/nlid_system.h"
#include "devices/nlid_truthtable.h"
#include "nl_base.h"
#include "nl_factory.h"
#include "nl_parser.h"
#include "nl_setup.h"
#include "plib/penum.h"
#include "plib/putil.h"

#include "solver/nld_solver.h"

namespace netlist
{
	// ----------------------------------------------------------------------------------------
	// nl_parse_t
	// ----------------------------------------------------------------------------------------

	nlparse_t::nlparse_t(log_type &log, abstract_t &abstract)
	: m_factory(log)
	, m_abstract(abstract)
	, m_log(log)
	, m_frontier_cnt(0)
	{ }

	void nlparse_t::register_alias(const pstring &alias, const pstring &out)
	{
		pstring alias_fqn = build_fqn(alias);
		pstring out_fqn = build_fqn(out);
		register_alias_nofqn(alias_fqn, out_fqn);
	}

	void nlparse_t::register_dip_alias_arr(const pstring &terms)
	{
		std::vector<pstring> list(plib::psplit(terms,", "));
		if (list.empty() || (list.size() % 2) == 1)
		{
			log().fatal(MF_DIP_PINS_MUST_BE_AN_EQUAL_NUMBER_OF_PINS_1(build_fqn("")));
			throw nl_exception(MF_DIP_PINS_MUST_BE_AN_EQUAL_NUMBER_OF_PINS_1(build_fqn("")));
		}
		std::size_t n = list.size();
		for (std::size_t i = 0; i < n / 2; i++)
		{
			register_alias(plib::pfmt("{1}")(i+1), list[i * 2]);
			register_alias(plib::pfmt("{1}")(n-i), list[i * 2 + 1]);
		}
	}

	void nlparse_t::register_dev(const pstring &classname,
		std::initializer_list<const char *> more_parameters)
	{
		std::vector<pstring> params;
		const auto *i(more_parameters.begin());
		pstring name(*i);
		++i;
		for (; i != more_parameters.end(); ++i)
		{
			params.emplace_back(*i);
		}
		register_dev(classname, name, params);
	}

	void nlparse_t::register_dev(const pstring &classname, const pstring &name,
		const std::vector<pstring> &params_and_connections,
		factory::element_t **felem)
	{

		auto *f = m_factory.factory_by_name(classname);

		// make sure we parse macro library entries
		// FIXME: this could be done here if e.g. f
		//        would have an indication that this is macro element.
		if (f->type() == factory::element_type::MACRO)
		{
			namespace_push(name);
			include(f->name());
			namespace_pop();
		}

		pstring key = build_fqn(name);
		if (device_exists(key))
		{
			log().fatal(MF_DEVICE_ALREADY_EXISTS_1(name));
			throw nl_exception(MF_DEVICE_ALREADY_EXISTS_1(name));
		}

		m_abstract.m_device_factory.insert(m_abstract.m_device_factory.end(), {key, f});

		auto paramlist = plib::psplit(f->param_desc(), ",");

		if (!params_and_connections.empty())
		{
			auto ptok(params_and_connections.begin());
			auto ptok_end(params_and_connections.end());

			for (const pstring &tp : paramlist)
			{
				if (plib::startsWith(tp, "+"))
				{
					if (ptok == ptok_end)
					{
						auto err = MF_PARAM_COUNT_MISMATCH_2(name, params_and_connections.size());
						log().fatal(err);
						throw nl_exception(err);
						//break;
					}
					pstring output_name = *ptok;
					log().debug("Link: {1} {2}\n", tp, output_name);

					register_link(name + "." + tp.substr(1), output_name);
					++ptok;
				}
				else if (plib::startsWith(tp, "@"))
				{
					pstring term = tp.substr(1);
					log().debug("Link: {1} {2}\n", tp, term);

					register_link(name + "." + term, term);
				}
				else
				{
					if (ptok == params_and_connections.end())
					{
						auto err = MF_PARAM_COUNT_MISMATCH_2(name, params_and_connections.size());
						log().fatal(err);
						throw nl_exception(err);
					}
					pstring paramfq = name + "." + tp;

					log().debug("Defparam: {1}\n", paramfq);

					register_param(paramfq, *ptok);

					++ptok;
				}
			}
			if (ptok != params_and_connections.end())
			{
				MF_PARAM_COUNT_EXCEEDED_2 err(name, params_and_connections.size());
				log().fatal(err);
				throw nl_exception(err);
			}
		}
		if (felem != nullptr)
			*felem = f;
	}

	void nlparse_t::register_link(const pstring &sin, const pstring &sout)
	{
		register_link_fqn(build_fqn(sin), build_fqn(sout));
	}

	void nlparse_t::register_link_arr(const pstring &terms)
	{
		std::vector<pstring> list(plib::psplit(terms,", "));
		if (list.size() < 2)
		{
			log().fatal(MF_NET_C_NEEDS_AT_LEAST_2_TERMINAL());
			throw nl_exception(MF_NET_C_NEEDS_AT_LEAST_2_TERMINAL());
		}
		for (std::size_t i = 1; i < list.size(); i++)
		{
			register_link(list[0], list[i]);
		}
	}

	void nlparse_t::include(const pstring &netlist_name)
	{
		if (m_sources.for_all<source_netlist_t>([this, &netlist_name] (source_netlist_t *src)
			{
				return src->parse(*this, netlist_name);
			}))
			return;
		log().fatal(MF_NOT_FOUND_IN_SOURCE_COLLECTION(netlist_name));
		throw nl_exception(MF_NOT_FOUND_IN_SOURCE_COLLECTION(netlist_name));
	}

	void nlparse_t::namespace_push(const pstring &aname)
	{
		if (m_namespace_stack.empty())
			//m_namespace_stack.push(netlist().name() + "." + aname);
			m_namespace_stack.push(aname);
		else
			m_namespace_stack.push(m_namespace_stack.top() + "." + aname);
	}

	void nlparse_t::namespace_pop()
	{
		m_namespace_stack.pop();
	}

	void nlparse_t::register_param(const pstring &param, const nl_fptype value)
	{
		if (plib::abs(value - plib::floor(value)) > nlconst::magic(1e-30)
			|| plib::abs(value) > nlconst::magic(1e9))
			register_param(param, plib::pfmt("{1:.9}").e(value));
		else
			register_param(param, plib::pfmt("{1}")(gsl::narrow<long>(value)));
	}

	void nlparse_t::register_param(const pstring &param, const pstring &value)
	{
		pstring fqn = build_fqn(param);
		pstring val(value);

		// strip " from stringified strings
		if (plib::startsWith(value, "\"") && plib::endsWith(value, "\""))
			val = value.substr(1, value.length() - 2);

		// Replace "@." with the current namespace
		val = plib::replace_all(val, "@.", namespace_prefix());
		auto idx = m_abstract.m_param_values.find(fqn);
		if (idx == m_abstract.m_param_values.end())
		{
			if (!m_abstract.m_param_values.insert({fqn, val}).second)
			{
				log().fatal(MF_ADDING_PARAMETER_1_TO_PARAMETER_LIST(param));
				throw nl_exception(MF_ADDING_PARAMETER_1_TO_PARAMETER_LIST(param));
			}
		}
		else
		{
			if (idx->second.find("$(") == pstring::npos)
			{
				//* There may be reason ... so make it an INFO
				log().info(MI_OVERWRITING_PARAM_1_OLD_2_NEW_3(fqn,
					idx->second, val));
			}
			m_abstract.m_param_values[fqn] = val;
		}
	}

	void nlparse_t::defparam(const pstring &name, const pstring &def)
	{
		// strip " from stringified strings
		pstring val(def);
		if (plib::startsWith(def, "\"") && plib::endsWith(def, "\""))
			val = def.substr(1, def.length() - 2);
		// Replace "@." with the current namespace
		val = plib::replace_all(val, "@.", namespace_prefix());
		m_abstract.m_defparams.emplace_back(namespace_prefix() + name, val);
	}

	void nlparse_t::register_lib_entry(const pstring &name, factory::properties &&props)
	{
		m_factory.add(plib::make_unique<factory::library_element_t, host_arena>(name, std::move(props)));
	}

	void nlparse_t::register_frontier(const pstring &attach, const pstring &r_IN,
		const pstring &r_OUT)
	{
		pstring frontier_name = plib::pfmt("frontier_{1}")(m_frontier_cnt);
		m_frontier_cnt++;
		register_dev("FRONTIER_DEV", frontier_name);
		register_param(frontier_name + ".RIN", r_IN);
		register_param(frontier_name + ".ROUT", r_OUT);
		register_link(frontier_name + ".G", "GND");
		pstring attfn = build_fqn(attach);
		pstring front_fqn = build_fqn(frontier_name);
		bool found = false;
		for (auto & link  : m_abstract.m_links)
		{
			if (link.first == attfn)
			{
				link.first = front_fqn + ".I";
				found = true;
			}
			else if (link.second == attfn)
			{
				link.second = front_fqn + ".I";
				found = true;
			}
		}
		if (!found)
		{
			log().fatal(MF_FOUND_NO_OCCURRENCE_OF_1(attach));
			throw nl_exception(MF_FOUND_NO_OCCURRENCE_OF_1(attach));
		}
		register_link(attach, frontier_name + ".Q");
	}

	void nlparse_t::truthtable_create(tt_desc &desc, factory::properties &&props)
	{
		auto fac = factory::truthtable_create(desc, std::move(props));
		m_factory.add(std::move(fac));
	}

	pstring nlparse_t::namespace_prefix() const
	{
		return (m_namespace_stack.empty() ? "" : m_namespace_stack.top() + ".");
	}

	pstring nlparse_t::build_fqn(const pstring &obj_name) const
	{
		return namespace_prefix() + obj_name;
	}

	void nlparse_t::register_alias_nofqn(const pstring &alias, const pstring &out)
	{
		if (!m_abstract.m_alias.insert({alias, out}).second)
		{
			log().fatal(MF_ADDING_ALI1_TO_ALIAS_LIST(alias));
			throw nl_exception(MF_ADDING_ALI1_TO_ALIAS_LIST(alias));
		}

	}

	void nlparse_t::register_link_fqn(const pstring &sin, const pstring &sout)
	{
		link_t temp = link_t(sin, sout);
		log().debug("link {1} <== {2}", sin, sout);
		m_abstract.m_links.push_back(temp);
	}

	bool nlparse_t::device_exists(const pstring &name) const
	{
		for (const auto &d : m_abstract.m_device_factory)
			if (d.first == name)
				return true;
		return false;
	}

	bool nlparse_t::parse_stream(plib::psource_t::stream_ptr &&istrm, const pstring &name)
	{
		auto y = std::make_unique<plib::ppreprocessor>(m_includes, &m_defines);
		y->process(std::move(istrm));
		return parser_t(std::move(y), *this).parse(name);
		//return parser_t(std::move(plib::ppreprocessor(&m_defines).process(std::move(istrm))), *this).parse(name);
	}

	void nlparse_t::add_define(const pstring &defstr)
	{
		auto p = defstr.find('=');
		if (p != pstring::npos)
			add_define(plib::left(defstr, p), defstr.substr(p+1));
		else
			add_define(defstr, "1");
	}

	void nlparse_t::register_dynamic_log_devices(const std::vector<pstring> &loglist)
	{
		log().debug("Creating dynamic logs ...");
		for (const pstring &ll : loglist)
		{
			pstring name = "log_" + ll;

			register_dev("LOG", name);
			register_link(name + ".I", ll);
		}
	}

	void nlparse_t::remove_connections(const pstring &pin)
	{
		// FIXME: check use_cases - remove_connections may be dead
		pstring pinfn = build_fqn(pin);
		bool found = false;

		for (auto link = m_abstract.m_links.begin(); link != m_abstract.m_links.end(); )
		{
			if ((link->first == pinfn) || (link->second == pinfn))
			{
				log().verbose("removing connection: {1} <==> {2}\n", link->first, link->second);
				link = m_abstract.m_links.erase(link);
				found = true;
			}
			else
				link++;
		}
		if (!found)
		{
			log().fatal(MF_FOUND_NO_OCCURRENCE_OF_1(pin));
			throw nl_exception(MF_FOUND_NO_OCCURRENCE_OF_1(pin));
		}
	}

	void nlparse_t::register_model(const pstring &model_in)
	{
		auto pos = model_in.find(' ');
		if (pos == pstring::npos)
			throw nl_exception(MF_UNABLE_TO_PARSE_MODEL_1(model_in));
		pstring model = plib::ucase(plib::trim(plib::left(model_in, pos)));
		pstring def = plib::trim(model_in.substr(pos + 1));
		if (!m_abstract.m_models.insert({model, def}).second)
			throw nl_exception(MF_MODEL_ALREADY_EXISTS_1(model_in));
	}


	// ----------------------------------------------------------------------------------------
	// Sources
	// ----------------------------------------------------------------------------------------

	plib::psource_t::stream_ptr nlparse_t::get_data_stream(const pstring &name)
	{
		auto strm = m_sources.get_stream<source_data_t>(name);
		if (strm)
			return strm;
		log().warning(MW_DATA_1_NOT_FOUND(name));
		return plib::psource_t::stream_ptr(nullptr);
	}

	// ----------------------------------------------------------------------------------------
	// setup_t
	// ----------------------------------------------------------------------------------------


setup_t::setup_t(netlist_state_t &nlstate)
	: m_abstract()
	, m_parser(nlstate.log(), m_abstract)
	, m_nlstate(nlstate)
	, m_models(m_abstract.m_models) // FIXME : parse abstract_t only
	, m_netlist_params(nullptr)
	, m_proxy_cnt(0)
{
}

pstring setup_t::termtype_as_str(detail::core_terminal_t &in)
{
	switch (in.type())
	{
		case detail::terminal_type::TERMINAL:
			return "TERMINAL";
		case detail::terminal_type::INPUT:
			return "INPUT";
		case detail::terminal_type::OUTPUT:
			return "OUTPUT";
	}
	return "Error"; // Tease gcc
}

pstring setup_t::get_initial_param_val(const pstring &name, const pstring &def) const
{
	auto i = m_abstract.m_param_values.find(name);
	auto found_pat(false);
	pstring v = (i == m_abstract.m_param_values.end()) ? def : i->second;

	do
	{
		found_pat = false;
		auto sp(plib::psplit(v, std::vector<pstring>({"$(", ")"})));
		std::size_t p(0);
		v = "";
		while (p < sp.size())
		{
			if (sp[p] == "$(")
			{
				p++;
				pstring r;
				while (p < sp.size() && sp[p] != ")")
					r += sp[p++];
				p++;
				auto k = m_params.find(r);
				if (k != m_params.end())
				{
					v = v + k->second.param().valstr();
					found_pat = true;
				}
				else
				{
					// pass - on
					v = v + "$(" + r + ")";
				}
			}
			else
				v += sp[p++];
		}
	} while (found_pat);

	return v;
}

void setup_t::register_term(detail::core_terminal_t &term)
{
	log().debug("{1} {2}\n", termtype_as_str(term), term.name());
	if (!m_terminals.insert({term.name(), &term}).second)
	{
		log().fatal(MF_ADDING_1_2_TO_TERMINAL_LIST(termtype_as_str(term), term.name()));
		throw nl_exception(MF_ADDING_1_2_TO_TERMINAL_LIST(termtype_as_str(term), term.name()));
	}
}

void setup_t::register_term(terminal_t &term, terminal_t &other_term)
{
	this->register_term(term);
	m_connected_terminals.insert({&term, &other_term});
}

void setup_t::register_param_t(param_t &param)
{
	if (!m_params.insert({param.name(), param_ref_t(param.device(), param)}).second)
	{
		log().fatal(MF_ADDING_PARAMETER_1_TO_PARAMETER_LIST(param.name()));
		throw nl_exception(MF_ADDING_PARAMETER_1_TO_PARAMETER_LIST(param.name()));
	}
}

pstring setup_t::resolve_alias(const pstring &name) const
{
	pstring temp = name;
	pstring ret;

	// FIXME: Detect endless loop
	do {
		ret = temp;
		auto p = m_abstract.m_alias.find(ret);
		temp = (p != m_abstract.m_alias.end() ? p->second : "");
	} while (!temp.empty() && temp != ret);

	log().debug("{1}==>{2}\n", name, ret);
	return ret;
}

pstring setup_t::de_alias(const pstring &alias) const
{
	pstring temp = alias;
	pstring ret;

	// FIXME: Detect endless loop
	do {
		ret = temp;
		temp = "";
		for (const auto &e : m_abstract.m_alias)
		{
			// FIXME: this will resolve first one found
			if (e.second == ret)
			{
				temp = e.first;
				break;
			}
		}
	} while (!temp.empty() && temp != ret);

	log().debug("{1}==>{2}\n", alias, ret);
	return ret;
}

std::vector<pstring> setup_t::get_terminals_for_device_name(const pstring &devname) const
{
	std::vector<pstring> terms;
	for (const auto & t : m_terminals)
	{
		if (plib::startsWith(t.second->name(), devname))
		{
			pstring tn(t.second->name().substr(devname.length()+1));
			if (tn.find('.') == pstring::npos)
				terms.push_back(tn);
		}
	}

	for (const auto & t : m_abstract.m_alias)
	{
		if (plib::startsWith(t.first, devname))
		{
			pstring tn(t.first.substr(devname.length()+1));
			if (tn.find('.') == pstring::npos)
			{
				terms.push_back(tn);
				pstring resolved = resolve_alias(t.first);
				if (resolved != t.first)
				{
					auto found = std::find(terms.begin(), terms.end(), resolved.substr(devname.length()+1));
					if (found!=terms.end())
						terms.erase(found);
				}
			}
		}
	}
	return terms;
}

detail::core_terminal_t *setup_t::find_terminal(const pstring &terminal_in, bool required) const
{
	const pstring &tname = resolve_alias(terminal_in);
	auto ret = m_terminals.find(tname);
	// look for default
	if (ret == m_terminals.end())
	{
		// look for ".Q" std output
		ret = m_terminals.find(tname + ".Q");
	}

	detail::core_terminal_t *term = (ret == m_terminals.end() ? nullptr : ret->second);

	if (term == nullptr && required)
	{
		log().fatal(MF_TERMINAL_1_2_NOT_FOUND(terminal_in, tname));
		throw nl_exception(MF_TERMINAL_1_2_NOT_FOUND(terminal_in, tname));
	}
	if (term != nullptr)
	{
		log().debug("Found input {1}\n", tname);
	}

	return term;
}

detail::core_terminal_t *setup_t::find_terminal(const pstring &terminal_in,
		detail::terminal_type atype, bool required) const
{
	const pstring &tname = resolve_alias(terminal_in);
	auto ret = m_terminals.find(tname);
	// look for default
	if (ret == m_terminals.end() && atype == detail::terminal_type::OUTPUT)
	{
		// look for ".Q" std output
		ret = m_terminals.find(tname + ".Q");
	}
	if (ret == m_terminals.end() && required)
	{
		log().fatal(MF_TERMINAL_1_2_NOT_FOUND(terminal_in, tname));
		throw nl_exception(MF_TERMINAL_1_2_NOT_FOUND(terminal_in, tname));
	}
	detail::core_terminal_t *term = (ret == m_terminals.end() ? nullptr : ret->second);

	if (term != nullptr && term->type() != atype)
	{
		if (required)
		{
			log().fatal(MF_OBJECT_1_2_WRONG_TYPE(terminal_in, tname));
			throw nl_exception(MF_OBJECT_1_2_WRONG_TYPE(terminal_in, tname));
		}

		term = nullptr;
	}
	if (term != nullptr)
		log().debug("Found input {1}\n", tname);

	return term;
}

param_ref_t setup_t::find_param(const pstring &param_in) const
{
	const pstring outname(resolve_alias(param_in));
	auto ret(m_params.find(outname));
	if (ret == m_params.end())
	{
		log().fatal(MF_PARAMETER_1_2_NOT_FOUND(param_in, outname));
		throw nl_exception(MF_PARAMETER_1_2_NOT_FOUND(param_in, outname));
	}
	return ret->second;
}


//NOLINTNEXTLINE(misc-no-recursion)
devices::nld_base_proxy *setup_t::get_d_a_proxy(const detail::core_terminal_t &out)
{
	nl_assert(out.is_logic());

	const auto &out_cast = dynamic_cast<const logic_output_t &>(out);
	auto iter_proxy(m_proxies.find(&out));

	if (iter_proxy != m_proxies.end())
		return iter_proxy->second;

	// create a new one ...
	pstring x = plib::pfmt("proxy_da_{1}_{2}")(out.name())(m_proxy_cnt);
	auto new_proxy =
			out_cast.logic_family()->create_d_a_proxy(m_nlstate, x, &out_cast);
	m_proxy_cnt++;
	// connect all existing terminals to new net

	for (auto & p : out.net().core_terms())
	{
		p->clear_net(); // de-link from all nets ...
		if (!connect(new_proxy->proxy_term(), *p))
		{
			log().fatal(MF_CONNECTING_1_TO_2(
					new_proxy->proxy_term().name(), (*p).name()));
			throw nl_exception(MF_CONNECTING_1_TO_2(
					new_proxy->proxy_term().name(), (*p).name()));
		}
	}
	out.net().core_terms().clear();

	add_terminal(out.net(), new_proxy->in());

	auto *proxy(new_proxy.get());
	if (!m_proxies.insert({&out, proxy}).second)
		throw nl_exception(MF_DUPLICATE_PROXY_1(out.name()));

	m_nlstate.register_device(new_proxy->name(), std::move(new_proxy));
	return proxy;

}


//NOLINTNEXTLINE(misc-no-recursion)
devices::nld_base_proxy *setup_t::get_a_d_proxy(detail::core_terminal_t &inp)
{
	nl_assert(inp.is_logic());

	const auto &incast = dynamic_cast<const logic_input_t &>(inp);

	auto iter_proxy(m_proxies.find(&inp));

	if (iter_proxy != m_proxies.end())
		return iter_proxy->second;

	log().debug("connect_terminal_input: connecting proxy\n");
	pstring x = plib::pfmt("proxy_ad_{1}_{2}")(inp.name())(m_proxy_cnt);
	auto new_proxy = incast.logic_family()->create_a_d_proxy(m_nlstate, x, &incast);
	//auto new_proxy = plib::owned_ptr<devices::nld_a_to_d_proxy>::Create(netlist(), x, &incast);

	auto *ret(new_proxy.get());

	if (!m_proxies.insert({&inp, ret}).second)
		throw nl_exception(MF_DUPLICATE_PROXY_1(inp.name()));

	m_proxy_cnt++;

	// connect all existing terminals to new net

	if (inp.has_net())
	{
		for (auto & p : inp.net().core_terms())
		{
			p->clear_net(); // de-link from all nets ...
			if (!connect(ret->proxy_term(), *p))
			{
				log().fatal(MF_CONNECTING_1_TO_2(
						ret->proxy_term().name(), (*p).name()));
				throw nl_exception(MF_CONNECTING_1_TO_2(
						ret->proxy_term().name(), (*p).name()));

			}
		}
		inp.net().core_terms().clear(); // clear the list
	}
	add_terminal(ret->out().net(), inp);
	m_nlstate.register_device(new_proxy->name(), std::move(new_proxy));
	return ret;
}

detail::core_terminal_t &setup_t::resolve_proxy(detail::core_terminal_t &term)
{
	if (term.is_logic())
	{
		const auto &out = dynamic_cast<const logic_t &>(term);
		auto iter_proxy(m_proxies.find(&out));
		if (iter_proxy != m_proxies.end())
			return iter_proxy->second->proxy_term();
	}
	return term;
}


//NOLINTNEXTLINE(misc-no-recursion)
void setup_t::merge_nets(detail::net_t &thisnet, detail::net_t &othernet)
{
	log().debug("merging nets ...\n");
	if (&othernet == &thisnet)
	{
		log().warning(MW_CONNECTING_1_TO_ITSELF(thisnet.name()));
		return; // Nothing to do
	}

	if (thisnet.is_rail_net() && othernet.is_rail_net())
	{
		log().fatal(MF_MERGE_RAIL_NETS_1_AND_2(thisnet.name(), othernet.name()));
		throw nl_exception(MF_MERGE_RAIL_NETS_1_AND_2(thisnet.name(), othernet.name()));
	}

	if (othernet.is_rail_net())
	{
		log().debug("othernet is railnet\n");
		merge_nets(othernet, thisnet);
	}
	else
	{
		move_connections(othernet, thisnet);
	}
}


//NOLINTNEXTLINE(misc-no-recursion)
void setup_t::connect_input_output(detail::core_terminal_t &in, detail::core_terminal_t &out)
{
	if (out.is_analog() && in.is_logic())
	{
		auto *proxy = get_a_d_proxy(in);

		add_terminal(out.net(), proxy->proxy_term());
	}
	else if (out.is_logic() && in.is_analog())
	{
		devices::nld_base_proxy *proxy = get_d_a_proxy(out);

		connect_terminals(proxy->proxy_term(), in);
		//proxy->out().net().register_con(in);
	}
	else
	{
		if (in.has_net())
			merge_nets(out.net(), in.net());
		else
			add_terminal(out.net(), in);
	}
}


//NOLINTNEXTLINE(misc-no-recursion)
void setup_t::connect_terminal_input(terminal_t &term, detail::core_terminal_t &inp)
{
	if (inp.is_analog())
	{
		connect_terminals(inp, term);
	}
	else if (inp.is_logic())
	{
		log().verbose("connect terminal {1} (in, {2}) to {3}\n", inp.name(),
				inp.is_analog() ? "analog" : inp.is_logic() ? "logic" : "?", term.name());
		auto *proxy = get_a_d_proxy(inp);

		//out.net().register_con(proxy->proxy_term());
		connect_terminals(term, proxy->proxy_term());

	}
	else
	{
		log().fatal(MF_OBJECT_INPUT_TYPE_1(inp.name()));
		throw nl_exception(MF_OBJECT_INPUT_TYPE_1(inp.name()));
	}
}


//NOLINTNEXTLINE(misc-no-recursion)
void setup_t::connect_terminal_output(terminal_t &in, detail::core_terminal_t &out)
{
	if (out.is_analog())
	{
		log().debug("connect_terminal_output: {1} {2}\n", in.name(), out.name());
		// no proxy needed, just merge existing terminal net
		if (in.has_net())
		{
			if (&out.net() == &in.net())
				log().warning(MW_CONNECTING_1_TO_2_SAME_NET(in.name(), out.name(), in.net().name()));
			merge_nets(out.net(), in.net());
		}
		else
			add_terminal(out.net(), in);
	}
	else if (out.is_logic())
	{
		log().debug("connect_terminal_output: connecting proxy\n");
		devices::nld_base_proxy *proxy = get_d_a_proxy(out);

		connect_terminals(proxy->proxy_term(), in);
	}
	else
	{
		log().fatal(MF_OBJECT_OUTPUT_TYPE_1(out.name()));
		throw nl_exception(MF_OBJECT_OUTPUT_TYPE_1(out.name()));
	}
}

void setup_t::connect_terminals(detail::core_terminal_t &t1,detail::core_terminal_t &t2)
{
	if (t1.has_net() && t2.has_net())
	{
		log().debug("T2 and T1 have net\n");
		merge_nets(t1.net(), t2.net());
	}
	else if (t2.has_net())
	{
		log().debug("T2 has net\n");
		add_terminal(t2.net(), t1);
	}
	else if (t1.has_net())
	{
		log().debug("T1 has net\n");
		add_terminal(t1.net(), t2);
	}
	else
	{
		log().debug("adding analog net ...\n");
		// FIXME: Nets should have a unique name
		auto anet = plib::make_owned<analog_net_t>(nlstate().pool(), m_nlstate,"net." + t1.name());
		auto *anetp = anet.get();
		m_nlstate.register_net(std::move(anet));
		t1.set_net(anetp);
		add_terminal(*anetp, t2);
		add_terminal(*anetp, t1);
	}
}

//NOLINTNEXTLINE(misc-no-recursion)
bool setup_t::connect_input_input(detail::core_terminal_t &t1, detail::core_terminal_t &t2)
{
	bool ret = false;
	if (t1.has_net())
	{
		if (t1.net().is_rail_net())
			ret = connect(t2, t1.net().railterminal());
		if (!ret)
		{
			for (auto & t : t1.net().core_terms())
			{
				if (t->is_type(detail::terminal_type::TERMINAL))
					ret = connect(t2, *t);
				if (ret)
					break;
			}
		}
	}
	if (!ret && t2.has_net())
	{
		if (t2.net().is_rail_net())
			ret = connect(t1, t2.net().railterminal());
		if (!ret)
		{
			for (auto & t : t2.net().core_terms())
			{
				if (t->is_type(detail::terminal_type::TERMINAL))
					ret = connect(t1, *t);
				if (ret)
					break;
			}
		}
	}
	return ret;
}


//NOLINTNEXTLINE(misc-no-recursion)
bool setup_t::connect(detail::core_terminal_t &t1_in, detail::core_terminal_t &t2_in)
{
	log().debug("Connecting {1} to {2}\n", t1_in.name(), t2_in.name());
	detail::core_terminal_t &t1 = resolve_proxy(t1_in);
	detail::core_terminal_t &t2 = resolve_proxy(t2_in);
	bool ret = true;

	if (t1.is_type(detail::terminal_type::OUTPUT) && t2.is_type(detail::terminal_type::INPUT))
	{
		if (t2.has_net() && t2.net().is_rail_net())
		{
			log().fatal(MF_INPUT_1_ALREADY_CONNECTED(t2.name()));
			throw nl_exception(MF_INPUT_1_ALREADY_CONNECTED(t2.name()));
		}
		connect_input_output(t2, t1);
	}
	else if (t1.is_type(detail::terminal_type::INPUT) && t2.is_type(detail::terminal_type::OUTPUT))
	{
		if (t1.has_net() && t1.net().is_rail_net())
		{
			log().fatal(MF_INPUT_1_ALREADY_CONNECTED(t1.name()));
			throw nl_exception(MF_INPUT_1_ALREADY_CONNECTED(t1.name()));
		}
		connect_input_output(t1, t2);
	}
	else if (t1.is_type(detail::terminal_type::OUTPUT) && t2.is_type(detail::terminal_type::TERMINAL))
	{
		connect_terminal_output(dynamic_cast<terminal_t &>(t2), t1);
	}
	else if (t1.is_type(detail::terminal_type::TERMINAL) && t2.is_type(detail::terminal_type::OUTPUT))
	{
		connect_terminal_output(dynamic_cast<terminal_t &>(t1), t2);
	}
	else if (t1.is_type(detail::terminal_type::INPUT) && t2.is_type(detail::terminal_type::TERMINAL))
	{
		connect_terminal_input(dynamic_cast<terminal_t &>(t2), t1);
	}
	else if (t1.is_type(detail::terminal_type::TERMINAL) && t2.is_type(detail::terminal_type::INPUT))
	{
		connect_terminal_input(dynamic_cast<terminal_t &>(t1), t2);
	}
	else if (t1.is_type(detail::terminal_type::TERMINAL) && t2.is_type(detail::terminal_type::TERMINAL))
	{
		connect_terminals(dynamic_cast<terminal_t &>(t1), dynamic_cast<terminal_t &>(t2));
	}
	else if (t1.is_type(detail::terminal_type::INPUT) && t2.is_type(detail::terminal_type::INPUT))
	{
		ret = connect_input_input(t1, t2);
	}
	else
		ret = false;
	return ret;
}

void setup_t::resolve_inputs()
{
	log().verbose("Resolving inputs ...");

	// Netlist can directly connect input to input.
	// We therefore first park connecting inputs and retry
	// after all other terminals were connected.

	unsigned tries = m_netlist_params->m_max_link_loops();
#if 0
	// This code fails for some netlists when the element at position 0
	// is deleted. It will fail somewhere deep in std::pair releasing
	// std::string called from erase.
	//
	// One example is the this netlist:
	//
	// #include "netlist/devices/net_lib.h"
	// NETLIST_START(charge_discharge)
	//     SOLVER(solver, 48000) // Fixed frequency solver
	//     CLOCK(I, 200) // 200 Hz  clock as input, TTL logic output
	//     RES(R, RES_K(1))
	//     CAP(C, CAP_U(1))
	//
	//     NET_C(I.Q, R.1)
	//     NET_C(R.2, C.1)
	//     NET_C(C.2, GND)
	//
	//     ALIAS(O, R.2) // Output O == C.1 == R.2
	// // NETLIST_END()
	//
	// Just save the net list as /tmp/test1.cpp, run
	// ./nltool --cmd=run -t 0.05 -l O -l I /tmp/test1.cpp
	// and see it crash with this code enabled.
	//
	// g++-7 (Ubuntu 7.4.0-1ubuntu1~16.04~ppa1) 7.4.0
	//
	while (!m_links.empty() && tries >  0)
	{
		auto li = m_links.begin();
		while (li != m_links.end())
		{
			const pstring t1s = li->first;
			const pstring t2s = li->second;
			detail::core_terminal_t *t1 = find_terminal(t1s);
			detail::core_terminal_t *t2 = find_terminal(t2s);

			if (connect(*t1, *t2))
				li = m_links.erase(li);
			else
				li++;
		}
		tries--;
	}
#else
	while (!m_abstract.m_links.empty() && tries > 0)
	{
		for (std::size_t i = 0; i < m_abstract.m_links.size(); )
		{
			const pstring t1s(m_abstract.m_links[i].first);
			const pstring t2s(m_abstract.m_links[i].second);
			detail::core_terminal_t *t1 = find_terminal(t1s);
			detail::core_terminal_t *t2 = find_terminal(t2s);
			if (connect(*t1, *t2))
				m_abstract.m_links.erase(m_abstract.m_links.begin() + plib::narrow_cast<std::ptrdiff_t>(i));
			else
				i++;
		}
		tries--;
	}
#endif
	if (tries == 0)
	{
		for (auto & link : m_abstract.m_links)
			log().warning(MF_CONNECTING_1_TO_2(de_alias(link.first), de_alias(link.second)));

		log().fatal(MF_LINK_TRIES_EXCEEDED(m_netlist_params->m_max_link_loops()));
		throw nl_exception(MF_LINK_TRIES_EXCEEDED(m_netlist_params->m_max_link_loops()));
	}

	log().verbose("deleting empty nets ...");

	// delete empty nets

	delete_empty_nets();

	bool err(false);

	log().verbose("looking for terminals not connected ...");
	for (auto & i : m_terminals)
	{
		detail::core_terminal_t *term = i.second;
		bool is_nc(dynamic_cast< devices::NETLIB_NAME(nc_pin) *>(&term->device()) != nullptr);
		if (term->has_net() && is_nc)
		{
			log().error(ME_NC_PIN_1_WITH_CONNECTIONS(term->name()));
			err = true;
		}
		else if (is_nc)
		{
			/* ignore */
		}
		else if (!term->has_net())
		{
			log().error(ME_TERMINAL_1_WITHOUT_NET(de_alias(term->name())));
			err = true;
		}
		else if (!term->net().has_connections())
		{
			if (term->is_logic_input())
				log().warning(MW_LOGIC_INPUT_1_WITHOUT_CONNECTIONS(term->name()));
			else if (term->is_logic_output())
				log().info(MI_LOGIC_OUTPUT_1_WITHOUT_CONNECTIONS(term->name()));
			else if (term->is_analog_output())
				log().info(MI_ANALOG_OUTPUT_1_WITHOUT_CONNECTIONS(term->name()));
			else
				log().warning(MW_TERMINAL_1_WITHOUT_CONNECTIONS(term->name()));
		}
	}
	log().verbose("checking tristate consistency  ...");
	for (auto & i : m_terminals)
	{
		detail::core_terminal_t *term = i.second;
		if (term->is_tristate_output())
		{
			const auto &tri(dynamic_cast<tristate_output_t &>(*term));
			// check if we are connected to a proxy
			const auto iter_proxy(m_proxies.find(&tri));

			if (iter_proxy == m_proxies.end() && !tri.is_force_logic())
			{
				log().error(ME_TRISTATE_NO_PROXY_FOUND_2(term->name(), term->device().name()));
				err = true;
			}
			else if (iter_proxy != m_proxies.end() && tri.is_force_logic())
			{
				log().error(ME_TRISTATE_PROXY_FOUND_2(term->name(), term->device().name()));
				err = true;
			}
		}
	}
	if (err)
	{
		log().fatal(MF_TERMINALS_WITHOUT_NET());
		throw nl_exception(MF_TERMINALS_WITHOUT_NET());
	}

}

void setup_t::add_terminal(detail::net_t &net, detail::core_terminal_t &terminal) noexcept(false)
{
	for (auto &t : net.core_terms())
		if (t == &terminal)
		{
			log().fatal(MF_NET_1_DUPLICATE_TERMINAL_2(net.name(), t->name()));
			throw nl_exception(MF_NET_1_DUPLICATE_TERMINAL_2(net.name(), t->name()));
		}

	terminal.set_net(&net);

	net.core_terms().push_back(&terminal);
}

void setup_t::remove_terminal(detail::net_t &net, detail::core_terminal_t &terminal) noexcept(false)
{
	if (plib::container::contains(net.core_terms(), &terminal))
	{
		terminal.set_net(nullptr);
		plib::container::remove(net.core_terms(), &terminal);
	}
	else
	{
		log().fatal(MF_REMOVE_TERMINAL_1_FROM_NET_2(terminal.name(), net.name()));
		throw nl_exception(MF_REMOVE_TERMINAL_1_FROM_NET_2(terminal.name(), net.name()));
	}
}

void setup_t::move_connections(detail::net_t &net, detail::net_t &dest_net)
{
	for (auto &ct : net.core_terms())
		add_terminal(dest_net, *ct);
	net.core_terms().clear();
}



log_type &setup_t::log() noexcept { return m_nlstate.log(); }
const log_type &setup_t::log() const noexcept { return m_nlstate.log(); }

// ----------------------------------------------------------------------------------------
// Models
// ----------------------------------------------------------------------------------------


//NOLINTNEXTLINE(misc-no-recursion)
void models_t::model_parse(const pstring &model_in, map_t &map)
{
	pstring model = model_in;
	std::size_t pos = 0;
	pstring key;

	while (true)
	{
		pos = model.find('(');
		if (pos != pstring::npos) break;

		key = plib::ucase(model);
		auto i = m_models.find(key);
		if (i == m_models.end())
			throw nl_exception(MF_MODEL_NOT_FOUND("xx" + model));
		model = i->second;
	}
	pstring xmodel = plib::left(model, pos);

	if (xmodel == "_")
		map["COREMODEL"] = key;
	else
	{
		auto i = m_models.find(xmodel);
		if (i != m_models.end())
			model_parse(xmodel, map);
		else
			throw nl_exception(MF_MODEL_NOT_FOUND(model_in));
	}

	pstring remainder = plib::trim(model.substr(pos + 1));
	if (!plib::endsWith(remainder, ")"))
		throw nl_exception(MF_MODEL_ERROR_1(model));
	// FIMXE: Not optimal
	remainder = plib::left(remainder, remainder.size() - 1);

	std::vector<pstring> pairs(plib::psplit(remainder," ", true));
	for (const pstring &pe : pairs)
	{
		auto pose = pe.find('=');
		if (pose == pstring::npos)
			throw nl_exception(MF_MODEL_ERROR_ON_PAIR_1(model));
		map[plib::ucase(plib::left(pe, pose))] = pe.substr(pose + 1);
	}
}

pstring models_t::model_t::model_string(const map_t &map)
{
	// operator [] has no const implementation
	pstring ret = map.at("COREMODEL") + "(";
	for (const auto & i : map)
		ret += (i.first + '=' + i.second + ' ');

	return ret + ")";
}

models_t::model_t models_t::get_model(const pstring &model)
{
	map_t &map = m_cache[model];

	if (map.empty())
		model_parse(model , map);

	return model_t(model, map);
}

pstring models_t::model_t::value_str(const pstring &entity) const
{
	if (entity != plib::ucase(entity))
		throw nl_exception(MF_MODEL_PARAMETERS_NOT_UPPERCASE_1_2(entity, model_string(m_map)));
	const auto it(m_map.find(entity));
	if ( it == m_map.end())
		throw nl_exception(MF_ENTITY_1_NOT_FOUND_IN_MODEL_2(entity, model_string(m_map)));

	return it->second;
}

nl_fptype models_t::model_t::value(const pstring &entity) const
{
	pstring tmp = value_str(entity);

	nl_fptype factor = nlconst::one();
	auto p = std::next(tmp.begin(), plib::narrow_cast<pstring::difference_type>(tmp.size() - 1));
	switch (*p)
	{
		case 'M': factor = nlconst::magic(1e6); break; // NOLINT
		case 'k':
		case 'K': factor = nlconst::magic(1e3); break; // NOLINT
		case 'm': factor = nlconst::magic(1e-3); break; // NOLINT
		case 'u': factor = nlconst::magic(1e-6); break; // NOLINT
		case 'n': factor = nlconst::magic(1e-9); break; // NOLINT
		case 'p': factor = nlconst::magic(1e-12); break; // NOLINT
		case 'f': factor = nlconst::magic(1e-15); break; // NOLINT
		case 'a': factor = nlconst::magic(1e-18); break; // NOLINT
		default:
			if (*p < '0' || *p > '9')
				throw nl_exception(MF_UNKNOWN_NUMBER_FACTOR_IN_2(m_model, entity));
	}
	if (factor != nlconst::one())
		tmp = plib::left(tmp, tmp.size() - 1);
	// FIXME: check for errors
	bool err(false);
	auto val = plib::pstonum_ne<nl_fptype>(tmp, err);
	if (err)
		throw nl_exception(MF_MODEL_NUMBER_CONVERSION_ERROR(entity, tmp, "double", m_model));
	return val * factor;
}


// FIXME: all this belongs elsewhere

PENUM(family_type,
	CUSTOM,
	TTL,
	MOS,
	CMOS,
	NMOS,
	PMOS)

class logic_family_std_proxy_t : public logic_family_desc_t
{
public:
	logic_family_std_proxy_t(family_type ft)
	: m_family_type(ft)
	{
	}

	// FIXME: create proxies based on family type (far future)
	device_arena::unique_ptr<devices::nld_base_d_to_a_proxy> create_d_a_proxy(netlist_state_t &anetlist,
			const pstring &name, const logic_output_t *proxied) const override
	{
		switch(m_family_type)
		{
			case family_type::CUSTOM:
			case family_type::TTL:
			case family_type::MOS:
			case family_type::CMOS:
			case family_type::NMOS:
			case family_type::PMOS:
				return anetlist.make_pool_object<devices::nld_d_to_a_proxy>(anetlist, name, proxied);
		}
		return anetlist.make_pool_object<devices::nld_d_to_a_proxy>(anetlist, name, proxied);
	}

	device_arena::unique_ptr<devices::nld_base_a_to_d_proxy> create_a_d_proxy(netlist_state_t &anetlist, const pstring &name, const logic_input_t *proxied) const override
	{
		switch(m_family_type)
		{
			case family_type::CUSTOM:
			case family_type::TTL:
			case family_type::MOS:
			case family_type::CMOS:
			case family_type::NMOS:
			case family_type::PMOS:
				return anetlist.make_pool_object<devices::nld_a_to_d_proxy>(anetlist, name, proxied);
		}
		return anetlist.make_pool_object<devices::nld_a_to_d_proxy>(anetlist, name, proxied);
	}
private:
	family_type m_family_type;
};


/// \brief Class representing the logic families.
///
///  This is the model representation of the logic families. This is a
///  netlist specific model. Examples give values for TTL family
///
//
///   |NL? |name  |parameter                                                  |units| TTL   |
///   |:--:|:-----|:----------------------------------------------------------|:----|------:|
///   | Y  |IVL   |Input voltage low threshold relative to supply voltage     |     |1.0e-14|
///   | Y  |IVH   |Input voltage high threshold relative to supply voltage    |     |      0|
///   | Y  |OVL   |Output voltage minimum voltage relative to supply voltage  |     |1.0e-14|
///   | Y  |OVL   |Output voltage maximum voltage relative to supply voltage  |     |1.0e-14|
///   | Y  |ORL   |Output output resistance for logic 0                       |     |      0|
///   | Y  |ORH   |Output output resistance for logic 1                       |     |      0|
///
class family_model_t
{
public:
	template <typename P>
	family_model_t(P &model)
	: m_TYPE(model, "TYPE")
	, m_IVL(model, "IVL")
	, m_IVH(model, "IVH")
	, m_OVL(model, "OVL")
	, m_OVH(model, "OVH")
	, m_ORL(model, "ORL")
	, m_ORH(model, "ORH")
	{}

	param_model_t::value_str_t m_TYPE; //!< Family type (TTL, CMOS, ...)
	param_model_t::value_t m_IVL;      //!< Input voltage low threshold relative to supply voltage
	param_model_t::value_t m_IVH;      //!< Input voltage high threshold relative to supply voltage
	param_model_t::value_t m_OVL;      //!< Output voltage minimum voltage relative to supply voltage
	param_model_t::value_t m_OVH;      //!< Output voltage maximum voltage relative to supply voltage
	param_model_t::value_t m_ORL;      //!< Output output resistance for logic 0
	param_model_t::value_t m_ORH;      //!< Output output resistance for logic 1
};


const logic_family_desc_t *setup_t::family_from_model(const pstring &model)
{
	family_type ft(family_type::CUSTOM);

	auto mod(m_models.get_model(model));
	family_model_t modv(mod);

	if (!ft.set_from_string(modv.m_TYPE()))
		throw nl_exception(MF_UNKNOWN_FAMILY_TYPE_1(modv.m_TYPE(), model));

	auto it = m_nlstate.family_cache().find(model);
	if (it != m_nlstate.family_cache().end())
		return it->second.get();

	auto ret = plib::make_unique<logic_family_std_proxy_t, host_arena>(ft);

	ret->m_low_thresh_PCNT = modv.m_IVL();
	ret->m_high_thresh_PCNT = modv.m_IVH();
	ret->m_low_VO = modv.m_OVL();
	ret->m_high_VO = modv.m_OVH();
	ret->m_R_low = modv.m_ORL();
	ret->m_R_high = modv.m_ORH();

	auto *retp = ret.get();

	m_nlstate.family_cache().emplace(model, std::move(ret));

	return retp;
}

// ----------------------------------------------------------------------------------------
// Device handling
// ----------------------------------------------------------------------------------------

void setup_t::delete_empty_nets()
{
	m_nlstate.nets().erase(
		std::remove_if(m_nlstate.nets().begin(), m_nlstate.nets().end(),
			[](device_arena::owned_ptr<detail::net_t> &net)
			{
				if (!net->has_connections())
				{
					net->state().log().verbose("Deleting net {1} ...", net->name());
					net->state().run_state_manager().remove_save_items(net.get());
					return true;
				}
				return false;
			}), m_nlstate.nets().end());
}

// ----------------------------------------------------------------------------------------
// Run preparation
// ----------------------------------------------------------------------------------------

void setup_t::prepare_to_run()
{
	pstring envlog = plib::util::environment("NL_LOGS", "");

	if (!envlog.empty())
	{
		std::vector<pstring> loglist(plib::psplit(envlog, ":"));
		m_parser.register_dynamic_log_devices(loglist);
	}

	// create defparams!

	for (auto & e : m_abstract.m_defparams)
	{
		auto param(plib::make_unique<param_str_t, host_arena>(nlstate(), e.first, e.second));
		register_param_t(*param);
		m_defparam_lifetime.push_back(std::move(param));
	}

	// make sure the solver and parameters are started first!

	for (auto & e : m_abstract.m_device_factory)
	{
		if ( m_parser.factory().is_class<devices::NETLIB_NAME(solver)>(e.second)
				|| m_parser.factory().is_class<devices::NETLIB_NAME(netlistparams)>(e.second))
		{
			m_nlstate.register_device(e.first, e.second->make_device(nlstate().pool(), m_nlstate, e.first));
		}
	}

	log().debug("Searching for solver and parameters ...\n");

	auto *solver = m_nlstate.get_single_device<devices::NETLIB_NAME(solver)>("solver");
	m_netlist_params = m_nlstate.get_single_device<devices::NETLIB_NAME(netlistparams)>("parameter");

	// set default model parameters

	// FIXME: this is not optimal
	m_parser.register_model(plib::pfmt("NMOS_DEFAULT _(CAPMOD={1})")(m_netlist_params->m_mos_capmodel()));
	m_parser.register_model(plib::pfmt("PMOS_DEFAULT _(CAPMOD={1})")(m_netlist_params->m_mos_capmodel()));

	// create devices

	log().debug("Creating devices ...\n");
	for (auto & e : m_abstract.m_device_factory)
	{
		if ( !m_parser.factory().is_class<devices::NETLIB_NAME(solver)>(e.second)
				&& !m_parser.factory().is_class<devices::NETLIB_NAME(netlistparams)>(e.second))
		{
			auto dev = e.second->make_device(m_nlstate.pool(), m_nlstate, e.first);
			m_nlstate.register_device(dev->name(), std::move(dev));
		}
	}

	int errcnt(0);
	log().debug("Looking for unknown parameters ...\n");
	for (auto &p : m_abstract.m_param_values)
	{
		auto f = m_params.find(p.first);
		if (f == m_params.end())
		{
			if (plib::endsWith(p.first, sHINT_NO_DEACTIVATE))
			{
				// FIXME: get device name, check for device
				auto *dev = m_nlstate.find_device(plib::replace_all(p.first, sHINT_NO_DEACTIVATE, ""));
				if (dev == nullptr)
				{
					log().error(ME_DEVICE_NOT_FOUND_FOR_HINT(p.first));
					errcnt++;
				}
			}
			else
			{
				log().error(ME_UNKNOWN_PARAMETER(p.first));
				errcnt++;
			}
		}
	}

	const bool use_deactivate = m_netlist_params->m_use_deactivate();

	for (auto &d : m_nlstate.devices())
	{
		if (use_deactivate)
		{
			auto p = m_abstract.m_param_values.find(d.second->name() + sHINT_NO_DEACTIVATE);
			if (p != m_abstract.m_param_values.end())
			{
				//FIXME: check for errors ...
				bool err(false);
				auto v = plib::pstonum_ne<nl_fptype>(p->second, err);
				if (err || plib::abs(v - plib::floor(v)) > nlconst::magic(1e-6) )
				{
					log().error(ME_HND_VAL_NOT_SUPPORTED(p->second));
					errcnt++;
				}
				else
				{
					// FIXME comparison with zero
					d.second->set_hint_deactivate(v == nlconst::zero());
				}
			}
		}
		else
			d.second->set_hint_deactivate(false);
	}

	if (errcnt > 0)
	{
		log().fatal(MF_ERRORS_FOUND(errcnt));
		throw nl_exception(MF_ERRORS_FOUND(errcnt));
	}

	// resolve inputs
	resolve_inputs();

	log().verbose("looking for two terms connected to rail nets ...");
	for (auto & t : m_nlstate.get_device_list<analog::NETLIB_NAME(twoterm)>())
	{
		if (t->N().net().is_rail_net() && t->P().net().is_rail_net())
		{
			log().info(MI_REMOVE_DEVICE_1_CONNECTED_ONLY_TO_RAILS_2_3(
				t->name(), t->N().net().name(), t->P().net().name()));
			remove_terminal(t->setup_N().net(), t->setup_N());
			remove_terminal(t->setup_P().net(), t->setup_P());
			m_nlstate.remove_device(t);
		}
	}

	log().verbose("initialize solver ...\n");

	if (solver == nullptr)
	{
		for (auto &p : m_nlstate.nets())
			if (p->is_analog())
			{
				log().fatal(MF_NO_SOLVER());
				throw nl_exception(MF_NO_SOLVER());
			}
	}
	else
		solver->post_start();

	for (auto &n : m_nlstate.nets())
		for (auto & term : n->core_terms())
		{
			core_device_t *dev = &term->device();
			dev->set_default_delegate(*term);
		}

}

// ----------------------------------------------------------------------------------------
// base sources
// ----------------------------------------------------------------------------------------

bool source_netlist_t::parse(nlparse_t &setup, const pstring &name)
{
	auto strm(stream(name));
	return (strm) ? setup.parse_stream(std::move(strm), name) : false;
}

source_string_t::stream_ptr source_string_t::stream(const pstring &name)
{
	plib::unused_var(name);
	source_string_t::stream_ptr ret(std::make_unique<std::istringstream>(m_str));
	ret->imbue(std::locale::classic());
	return ret;
}

source_mem_t::stream_ptr source_mem_t::stream(const pstring &name)
{
	plib::unused_var(name);
	source_mem_t::stream_ptr ret(std::make_unique<std::istringstream>(m_str, std::ios_base::binary));
	ret->imbue(std::locale::classic());
	return ret;
}

source_file_t::stream_ptr source_file_t::stream(const pstring &name)
{
	plib::unused_var(name);
	auto ret(std::make_unique<plib::ifstream>(plib::filesystem::u8path(m_filename)));
	return (ret->is_open()) ? std::move(ret) : stream_ptr(nullptr);
}

bool source_proc_t::parse(nlparse_t &setup, const pstring &name)
{
	if (name == m_setup_func_name)
	{
		m_setup_func(setup);
		return true;
	}

	return false;
}

source_proc_t::stream_ptr source_proc_t::stream(const pstring &name)
{
	plib::unused_var(name);
	stream_ptr p(nullptr);
	return p;
}

} // namespace netlist

