// license:GPL-2.0+
// copyright-holders:Couriersud

///
/// \file nltypes.h
///

/// \note never change the name to nl_types.h. This creates a conflict
///       with nl_types.h file provided by libc++ (clang, macosx)
///

#ifndef NLTYPES_H_
#define NLTYPES_H_

#include "nl_config.h"

#include "plib/pmempool.h"
#include "plib/ppmf.h"
#include "plib/pstring.h"
#include "plib/ptime.h"
#include "plib/ptypes.h"

#include <memory>

// FIXME: Move to ptypes
namespace plib
{
	// FORWARD declarations
	template <typename BASEARENA, std::size_t MINALIGN>
	class mempool_arena;

	struct aligned_arena;
	class dynlib_base;

	template<class T, bool debug_enabled>
	class plog_base;

	struct plog_level;
} // namespace plib

namespace netlist
{
	// -----------------------------------------------------------------------------
	// forward definitions
	// -----------------------------------------------------------------------------

	class logic_output_t;
	class tristate_output_t;
	class logic_input_t;
	class analog_net_t;
	class logic_net_t;
	class setup_t;
	class nlparse_t;
	class netlist_t;
	class netlist_state_t;
	class core_device_t;
	class device_t;
	class netlist_state_t;
	class param_t;
	class logic_family_desc_t;
	class terminal_t;

	class models_t;

	namespace devices
	{
		class nld_solver;
		class nld_mainclock;
		class nld_base_proxy;
		class nld_base_d_to_a_proxy;
		class nld_base_a_to_d_proxy;
		class nld_netlistparams;
	} // namespace devices

	namespace solver
	{
		class matrix_solver_t;
	} // namespace solver

	namespace detail
	{
		struct abstract_t;
		class core_terminal_t;
		class net_t;
		class device_object_t;
	} // namespace detail

	namespace factory
	{
		class list_t;
		class element_t;
		struct properties;
	} // namespace factory

	template <class CX>
	class delegator_t : public CX
	{
	protected:
		using base_type = delegator_t<CX>;
		using delegated_type = CX;
		using delegated_type::delegated_type;
	};

} // namespace netlist


namespace netlist
{

	/// \brief Constants and const calculations for the library
	///
	template<typename T>
	struct nlconst_base : public plib::constants<T>
	{
		using BC = plib::constants<T>;

		static inline constexpr T np_VT(T n=BC::one(), T temp=BC::T0()) noexcept
		{ return n * temp * BC::k_b() / BC::Q_e(); }

		static inline constexpr T np_Is() noexcept { return static_cast<T>(1e-15); } // NOLINT

		/// \brief constant startup gmin
		///
		/// This should be used during object creation to initialize
		/// conductivities with a reasonable value.
		/// During reset, the object should than make use of exec().gmin()
		/// to use the actual gmin() value.
		static inline constexpr T cgmin() noexcept { return BC::magic(1e-9); } // NOLINT

		// FIXME: Some objects used 1e-15 for initial gmin. Needs to be
		//        aligned with cgmin
		static inline constexpr T cgminalt() noexcept { return BC::magic(1e-15); } // NOLINT

		/// \brief Multiplier applied to VT in np diode models to determine range for constant model
		///
		static inline constexpr T diode_min_cutoff_mult() noexcept { return BC::magic(-5.0); } // NOLINT

		/// \brief Startup voltage used by np diode models
		///
		static inline constexpr T diode_start_voltage() noexcept { return BC::magic(0.7); } // NOLINT

	};

	/// \brief nlconst_base struct specialized for nl_fptype.
	///
	struct nlconst : public nlconst_base<nl_fptype>
	{
	};

	/// \brief netlist_sig_t is the type used for logic signals.
	///
	/// This may be any of bool, uint8_t, uint16_t, uin32_t and uint64_t.
	/// The choice has little to no impact on performance.
	///
	using netlist_sig_t = std::uint32_t;

	/// \brief The memory pool for netlist objects
	///
	/// \note This is not the right location yet.
	///

	using device_arena = std::conditional_t<config::use_mempool::value,
		plib::mempool_arena<plib::aligned_arena, NL_MEMPOOL_ALIGN>,
		plib::aligned_arena>;
	using host_arena   = plib::aligned_arena;

	/// \brief Interface definition for netlist callbacks into calling code
	///
	/// A class inheriting from netlist_callbacks_t has to be passed to the netlist_t
	/// constructor. Netlist does processing during construction and thus needs
	/// the object passed completely constructed.
	///
	class callbacks_t
	{
	public:

		callbacks_t() = default;
		virtual ~callbacks_t() = default;

		PCOPYASSIGNMOVE(callbacks_t, default)

		/// \brief logging callback.
		///
		virtual void vlog(const plib::plog_level &l, const pstring &ls) const noexcept = 0;

		/// \brief provide library with static solver implementations.
		///
		/// By default no static solvers are provided since these are
		/// determined by the specific use case. It is up to the implementor
		/// of a callbacks_t implementation to optionally provide such a collection
		/// of symbols.
		///
		virtual std::unique_ptr<plib::dynlib_base> static_solver_lib() const;
	};

	using log_type =  plib::plog_base<callbacks_t, NL_DEBUG>;

	//============================================================
	//  Types needed by various includes
	//============================================================

	/// \brief Timestep type.
	///
	/// May be either FORWARD or RESTORE
	///
	enum class timestep_type
	{
		FORWARD,  ///< forward time
		RESTORE   ///< restore state before last forward
	};

	/// \brief Delegate type for device notification.
	///
	using nldelegate = plib::pmfp<void>;
	using nldelegate_ts = plib::pmfp<void, timestep_type, nl_fptype>;
	using nldelegate_dyn = plib::pmfp<void>;

	namespace detail {

		/// \brief Enum specifying the type of object
		///
		enum class terminal_type {
			TERMINAL = 0, ///< object is an analog terminal
			INPUT    = 1, ///< object is an input
			OUTPUT   = 2, ///< object is an output
		};

	} // namespace detail

	using netlist_time = plib::ptime<std::int64_t, config::INTERNAL_RES::value>;
	using netlist_time_ext = plib::ptime<std::conditional<NL_PREFER_INT128 && plib::compile_info::has_int128::value, INT128, std::int64_t>::type, config::INTERNAL_RES::value>;

	static_assert(noexcept(netlist_time::from_nsec(1)), "Not evaluated as constexpr");

	//============================================================
	//  MACROS
	//============================================================

	template <typename T> inline constexpr netlist_time NLTIME_FROM_NS(T &&t) noexcept { return netlist_time::from_nsec(t); }
	template <typename T> inline constexpr netlist_time NLTIME_FROM_US(T &&t) noexcept { return netlist_time::from_usec(t); }
	template <typename T> inline constexpr netlist_time NLTIME_FROM_MS(T &&t) noexcept { return netlist_time::from_msec(t); }

	struct desc_base
	{
		/// \brief: used to hold one static netlist_time value
		///
		template<netlist_time::internal_type value0>
		struct times_ns1
		{
			static constexpr netlist_time value(std::size_t N = 0)
			{
				plib::unused_var(N);
				return NLTIME_FROM_NS(value0);
			}
		};

		template <netlist_time::internal_type value0>
		using time_ns = times_ns1<value0>;

		/// \brief: used to hold two static netlist_time values
		///
		template<netlist_time::internal_type value0,
			netlist_time::internal_type  value1>
		struct times_ns2
		{
			static constexpr netlist_time value(std::size_t N)
			{
				return NLTIME_FROM_NS(N == 0 ? value0 : value1);
			}
		};

		/// \brief: used to hold three static netlist_time values
		///
		template<netlist_time::internal_type value0,
			netlist_time::internal_type value1,
			netlist_time::internal_type value2>
		struct times_ns3
		{
			static constexpr netlist_time value(std::size_t N)
			{
				return NLTIME_FROM_NS(N == 0 ? value0 :
						N == 1 ? value1 :
								 value2);
			}
		};

		/// \brief: used to define a constant in device description struct
		///
		/// See the 74125 implementation
		///
		template <std::size_t V>
		using desc_const =  std::integral_constant<const std::size_t, V>;

		template <typename T, T V>
		using desc_const_t =  std::integral_constant<const T, V>;
	};

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

} // namespace netlist


#endif // NLTYPES_H_
