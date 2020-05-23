// license:GPL-2.0+
// copyright-holders:Couriersud

///
/// \file nl_factory.h
///

#ifndef NLFACTORY_H_
#define NLFACTORY_H_

#include "nltypes.h"
#include "plib/palloc.h"
#include "plib/ptypes.h"

#include <vector>

#define NETLIB_DEVICE_IMPL_ALIAS(p_alias, chip, p_name, p_def_param) \
	NETLIB_DEVICE_IMPL_BASE(devices, p_alias, chip, p_name, p_def_param) \

#define NETLIB_DEVICE_IMPL(chip, p_name, p_def_param) \
	NETLIB_DEVICE_IMPL_NS(devices, chip, p_name, p_def_param)

#define NETLIB_DEVICE_IMPL_NS(ns, chip, p_name, p_def_param) \
	NETLIB_DEVICE_IMPL_BASE(ns, chip, chip, p_name, p_def_param) \

#define NETLIB_DEVICE_IMPL_BASE(ns, p_alias, chip, p_name, p_def_param) \
	static plib::unique_ptr<factory::element_t> NETLIB_NAME(p_alias ## _c) \
			(const pstring &classname) \
	{ \
		using devtype = factory::device_element_t<ns :: NETLIB_NAME(chip)>; \
		return devtype::create(p_name, classname, p_def_param, __FILE__); \
	} \
	\
	factory::constructor_ptr_t decl_ ## p_alias = NETLIB_NAME(p_alias ## _c);

namespace netlist {
	class device_t;
	class nlparse_t;
	class setup_t;
	class netlist_state_t;

namespace factory {
	// -----------------------------------------------------------------------------
	// net_dev class factory
	// -----------------------------------------------------------------------------

	class element_t
	{
	public:
		element_t(const pstring &name, const pstring &classname,
				const pstring &def_param);
		element_t(const pstring &name, const pstring &classname,
				const pstring &def_param, const pstring &sourcefile);
		virtual ~element_t() = default;

		PCOPYASSIGNMOVE(element_t, default)

		virtual unique_pool_ptr<device_t> make_device(nlmempool &pool,
			netlist_state_t &anetlist,
			const pstring &name) = 0;

		virtual void macro_actions(nlparse_t &nparser, const pstring &name)
		{
			plib::unused_var(nparser);
			plib::unused_var(name);
		}

		const pstring &name() const noexcept { return m_name; }
		const pstring &classname() const noexcept { return m_classname; }
		const pstring &param_desc() const noexcept { return m_def_param; }
		const pstring &sourcefile() const noexcept { return m_sourcefile; }

	private:
		pstring m_name;                             ///< device name
		pstring m_classname;                        ///< device class name
		pstring m_def_param;                        ///< default parameter
		pstring m_sourcefile;                       ///< source file
	};

	template <class C>
	class device_element_t : public element_t
	{
	public:
		device_element_t(const pstring &name, const pstring &classname,
				const pstring &def_param)
		: element_t(name, classname, def_param) { }
		device_element_t(const pstring &name, const pstring &classname,
				const pstring &def_param, const pstring &sourcefile)
		: element_t(name, classname, def_param, sourcefile) { }

		unique_pool_ptr<device_t> make_device(nlmempool &pool,
			netlist_state_t &anetlist,
			const pstring &name) override
		{
			return pool.make_unique<C>(anetlist, name);
		}

		static plib::unique_ptr<device_element_t<C>> create(const pstring &name,
			const pstring &classname, const pstring &def_param,
			const pstring &sourcefile)
		{
			return plib::make_unique<device_element_t<C>>(name, classname, def_param, sourcefile);
		}
	};

	class list_t : public std::vector<plib::unique_ptr<element_t>>
	{
	public:
		explicit list_t(log_type &alog);
		~list_t() = default;

		PCOPYASSIGNMOVE(list_t, delete)

		template<class device_class>
		void register_device(const pstring &name, const pstring &classname,
			const pstring &def_param, const pstring &sourcefile)
		{
			register_device(device_element_t<device_class>::create(name, classname, def_param, sourcefile));
		}

		void register_device(plib::unique_ptr<element_t> &&factory) noexcept(false);

		element_t * factory_by_name(const pstring &devname) noexcept(false);

		template <class C>
		bool is_class(element_t *f) noexcept
		{
			return dynamic_cast<device_element_t<C> *>(f) != nullptr;
		}

	private:
		log_type &m_log;
	};

	// -----------------------------------------------------------------------------
	// factory_creator_ptr_t
	// -----------------------------------------------------------------------------

	using constructor_ptr_t = plib::unique_ptr<element_t> (*)(const pstring &classname);

	template <typename T>
	plib::unique_ptr<element_t> constructor_t(const pstring &name, const pstring &classname,
			const pstring &def_param)
	{
		return plib::make_unique<device_element_t<T>>(name, classname, def_param);
	}

	// -----------------------------------------------------------------------------
	// factory_lib_entry_t: factory class to wrap macro based chips/elements
	// -----------------------------------------------------------------------------

	class library_element_t : public element_t
	{
	public:

		library_element_t(const pstring &name, const pstring &classname,
				const pstring &def_param, const pstring &source)
		: element_t(name, classname, def_param, source)
		{
		}

		unique_pool_ptr<device_t> make_device(nlmempool &pool,
			netlist_state_t &anetlist,
			const pstring &name) override;

		void macro_actions(nlparse_t &nparser, const pstring &name) override;

	private:
	};

	} // namespace factory

	namespace devices {
		void initialize_factory(factory::list_t &factory);
	} // namespace devices
} // namespace netlist

#endif // NLFACTORY_H_
