// license:GPL-2.0+
// copyright-holders:Couriersud

///
/// \file param.h
///

#ifndef NL_CORE_BASE_OBJECTS_H_
#define NL_CORE_BASE_OBJECTS_H_

#include "state_var.h"

#include "../nltypes.h"

#include "../plib/palloc.h"
#include "../plib/pmempool.h"
#include "../plib/pchrono.h"
#include "../plib/pexception.h"
#include "../plib/plists.h"

#include <unordered_map>

namespace netlist
{

	//============================================================
	//  Exceptions
	//============================================================

	/// \brief Generic netlist exception.
	///  The exception is used in all events which are considered fatal.

	class nl_exception : public plib::pexception
	{
	public:
		/// \brief Constructor.
		///  Allows a descriptive text to be passed to the exception

		explicit nl_exception(const pstring &text //!< text to be passed
				)
		: plib::pexception(text) { }

		/// \brief Constructor.
		///  Allows to use \ref plib::pfmt logic to be used in exception

		template<typename... Args>
		explicit nl_exception(const pstring &fmt //!< format to be used
			, Args&&... args //!< arguments to be passed
			)
		: plib::pexception(plib::pfmt(fmt)(std::forward<Args>(args)...)) { }
	};

	namespace detail {

		template <typename C, typename T>
		struct property_store_t
		{
			using value_type = T;
			using key_type = const C *;
			static void add(key_type obj, const value_type &aname) noexcept
			{
				store().insert({obj, aname});
			}

			static value_type *get(key_type obj) noexcept
			{
				try
				{
					auto ret(store().find(obj));
					if (ret == store().end())
						return nullptr;
					return &ret->second;
				}
				catch (...)
				{
					plib::terminate("exception in property_store_t.get()");
					return static_cast<value_type *>(nullptr);
				}
			}

			static void remove(key_type obj) noexcept
			{
				store().erase(store().find(obj));
			}

			static std::unordered_map<key_type, value_type> &store() noexcept
			{
				static std::unordered_map<key_type, value_type> lstore;
				return lstore;
			}

		};

		// -----------------------------------------------------------------------------
		// object_t
		// -----------------------------------------------------------------------------

		/// \brief The base class for netlist devices, terminals and parameters.
		///
		///  This class serves as the base class for all device, terminal and
		///  objects.

		class object_t
		{
		public:

			/// \brief Constructor.
			/// Every class derived from the object_t class must have a name.
			///
			/// \param aname string containing name of the object

			explicit object_t(const pstring &aname)
			{
				props::add(this, aname);
			}

			PCOPYASSIGNMOVE(object_t, delete)
			/// \brief return name of the object
			///
			/// \returns name of the object.

			const pstring &name() const noexcept
			{
				return *props::get(this);
			}

		protected:

			using props = property_store_t<object_t, pstring>;

			// only childs should be destructible
			~object_t() noexcept
			{
				props::remove(this);
			}

		private:
		};

		/// \brief Base class for all objects bejng owned by a netlist
		///
		/// The object provides adds \ref netlist_state_t and \ref netlist_t
		/// accessors.
		///
		class netlist_object_t : public object_t
		{
		public:
			explicit netlist_object_t(netlist_t &nl, const pstring &name)
			: object_t(name)
			, m_netlist(nl)
			{ }

			~netlist_object_t() = default;

			PCOPYASSIGNMOVE(netlist_object_t, delete)

			netlist_state_t & state() noexcept;
			const netlist_state_t & state() const noexcept;

			netlist_t & exec() noexcept { return m_netlist; }
			const netlist_t & exec() const noexcept { return m_netlist; }

			// to ease template design
			template<typename T, typename... Args>
			device_arena::unique_ptr<T> make_pool_object(Args&&... args);

		private:
			netlist_t & m_netlist;

		};

		// -----------------------------------------------------------------------------
		// device_object_t
		// -----------------------------------------------------------------------------

		/// \brief Base class for all objects being owned by a device.
		///
		/// Serves as the base class of all objects being owned by a device.
		///
		/// The class also supports device-less objects. In this case,
		/// nullptr is passed in as the device object.
		///

		class device_object_t : public object_t
		{
		public:
			/// \brief Constructor.
			///
			/// \param dev  pointer to device owning the object.
			/// \param name string holding the name of the device

			device_object_t(core_device_t *dev, const pstring &name);

			/// \brief returns reference to owning device.
			/// \returns reference to owning device.

			core_device_t &device() noexcept { return *m_device; }
			const core_device_t &device() const noexcept { return *m_device; }

			/// \brief The netlist owning the owner of this object.
			/// \returns reference to netlist object.

			netlist_state_t &state() noexcept;
			const netlist_state_t &state() const noexcept;

			netlist_t &exec() noexcept;
			const netlist_t &exec() const noexcept;

		private:
			core_device_t * m_device;
		};

		// -----------------------------------------------------------------------------
		// core_terminal_t
		// -----------------------------------------------------------------------------

		/// \brief Base class for all terminals.
		///
		/// All terminals are derived from this class.
		///
		class core_terminal_t : public device_object_t,
								public plib::linkedlist_t<core_terminal_t>::element_t
		{
		public:
			/// \brief Number of signal bits
			///
			/// Going forward setting this to 8 will allow 8-bit signal
			/// busses to be used in netlist, e.g. for more complex memory
			/// arrangements.
			/// Mimimum value is 2 here to support tristate output on proxies.
			static constexpr const unsigned int INP_BITS = 2;

			static constexpr const unsigned int INP_MASK = (1 << INP_BITS) - 1;
			static constexpr const unsigned int INP_HL_SHIFT = 0;
			static constexpr const unsigned int INP_LH_SHIFT = INP_BITS;

			static constexpr netlist_sig_t OUT_TRISTATE() { return INP_MASK; }

			static_assert(INP_BITS * 2 <= sizeof(netlist_sig_t) * 8, "netlist_sig_t size not sufficient");

			enum state_e {
				STATE_INP_PASSIVE = 0,
				STATE_INP_HL      = (INP_MASK << INP_HL_SHIFT),
				STATE_INP_LH      = (INP_MASK << INP_LH_SHIFT),
				STATE_INP_ACTIVE  = STATE_INP_HL | STATE_INP_LH,
				STATE_OUT         = (1 << (2*INP_BITS)),
				STATE_BIDIR       = (1 << (2*INP_BITS + 1))
			};

			core_terminal_t(core_device_t &dev, const pstring &aname,
					state_e state, nldelegate delegate);
			virtual ~core_terminal_t() noexcept = default;

			PCOPYASSIGNMOVE(core_terminal_t, delete)

			/// \brief The object type.
			/// \returns type of the object
			terminal_type type() const noexcept(false);

			/// \brief Checks if object is of specified type.
			/// \param atype type to check object against.
			/// \returns true if object is of specified type else false.
			bool is_type(const terminal_type atype) const noexcept(false) { return (type() == atype); }

			void set_net(net_t *anet) noexcept { m_net = anet; }
			void clear_net() noexcept { m_net = nullptr; }
			bool has_net() const noexcept { return (m_net != nullptr); }

			net_t & net() const noexcept { return *m_net;}

			bool is_logic() const noexcept;
			bool is_logic_input() const noexcept;
			bool is_logic_output() const noexcept;
			bool is_tristate_output() const noexcept;
			bool is_analog() const noexcept;
			bool is_analog_input() const noexcept;
			bool is_analog_output() const noexcept;

			bool is_state(state_e astate) const noexcept { return (m_state == astate); }
			state_e terminal_state() const noexcept { return m_state; }
			void set_state(state_e astate) noexcept { m_state = astate; }

			void reset() noexcept { set_state(is_type(terminal_type::OUTPUT) ? STATE_OUT : STATE_INP_ACTIVE); }

	#if NL_USE_COPY_INSTEAD_OF_REFERENCE
			void set_copied_input(netlist_sig_t val) noexcept
			{
				m_Q = val;
			}

			state_var_sig m_Q;
	#else
			void set_copied_input(netlist_sig_t val) const noexcept { plib::unused_var(val); } // NOLINT: static means more message elsewhere
	#endif

			void set_delegate(const nldelegate &delegate) noexcept { m_delegate = delegate; }
			const nldelegate &delegate() const noexcept { return m_delegate; }
			inline void run_delegate() noexcept { return m_delegate(); }
		private:
			nldelegate m_delegate;
			net_t * m_net;
			state_var<state_e> m_state;
		};


	} // namespace detail

	// -----------------------------------------------------------------------------
	// core_device_t
	// -----------------------------------------------------------------------------
	// FIXME: belongs into detail namespace
	class core_device_t : public detail::netlist_object_t
	{
	public:
		core_device_t(netlist_state_t &owner, const pstring &name);
		core_device_t(core_device_t &owner, const pstring &name);

		PCOPYASSIGNMOVE(core_device_t, delete)

		virtual ~core_device_t() noexcept = default;

		void do_inc_active() noexcept
		{
			if (m_hint_deactivate)
			{
				if (++m_active_outputs == 1)
				{
					if (m_stats)
						m_stats->m_stat_inc_active.inc();
					inc_active();
				}
			}
		}

		void do_dec_active() noexcept
		{
			if (m_hint_deactivate)
				if (--m_active_outputs == 0)
				{
					dec_active();
				}
		}

		void set_hint_deactivate(bool v) noexcept { m_hint_deactivate = v; }
		bool get_hint_deactivate() const noexcept { return m_hint_deactivate; }
		// Has to be set in device reset
		void set_active_outputs(int n) noexcept { m_active_outputs = n; }

		// stats
		struct stats_t
		{
			// NL_KEEP_STATISTICS
			plib::pperftime_t<true>  m_stat_total_time;
			plib::pperfcount_t<true> m_stat_call_count;
			plib::pperfcount_t<true> m_stat_inc_active;
		};

		stats_t * stats() const noexcept { return m_stats.get(); }
#if 0
		virtual void update() noexcept { }
#endif
		virtual void reset() { }

	protected:

		virtual void inc_active() noexcept {  }
		virtual void dec_active() noexcept {  }

		log_type & log();

	public:
		virtual void timestep(timestep_type ts_type, nl_fptype st) noexcept { plib::unused_var(ts_type, st); }
		virtual void update_terminals() noexcept { }

		virtual void update_param() noexcept {}
		virtual bool is_dynamic() const noexcept { return false; }
		virtual bool is_timestep() const noexcept { return false; }

	private:
		bool            m_hint_deactivate;
		state_var_s32   m_active_outputs;
		device_arena::unique_ptr<stats_t> m_stats;
	};

} // namespace netlist


#endif // NL_CORE_BASE_OBJECTS_H_
