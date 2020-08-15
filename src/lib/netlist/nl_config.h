// license:GPL-2.0+
// copyright-holders:Couriersud

/// \file nl_config.h
///

#ifndef NLCONFIG_H_
#define NLCONFIG_H_

#include "plib/pconfig.h"
#include "plib/pexception.h"

#include <type_traits>

///
/// \brief Version - Major.
///
#define NL_VERSION_MAJOR           0
///
/// \brief Version - Minor.
///
#define NL_VERSION_MINOR           13
/// \brief Version - Patch level.
///
#define NL_VERSION_PATCHLEVEL      0

///
/// \addtogroup compiledefine
/// \{

//============================================================
//  GENERAL
//============================================================

/// \brief  Make use of a memory pool for performance related objects.
///
/// Set to 1 to compile netlist with memory allocations from a
/// linear memory pool. This is based of the assumption that
/// due to enhanced locality there will be less cache misses.
/// Your mileage may vary.
///

#ifndef NL_USE_MEMPOOL
#define NL_USE_MEMPOOL               (1)
#endif

/// brief default minimum alignment of mempool_arena
///
/// 256 is the best compromise between logic applications like MAME
/// TTL games (e.g. pong) and analog applications like e.g. kidnikik sound.
///
/// Best performance for pong is achieved with a value of 16, but this degrades
/// kidniki performance by ~10%.
///
/// More work is needed here.
#define NL_MEMPOOL_ALIGN            (16)

/// \brief  Enable queue statistics.
///
/// Queue statistics come at a performance cost. Although
/// the cost is low, we disable them here since they are
/// only needed during development.
///

#ifndef NL_USE_QUEUE_STATS
#define NL_USE_QUEUE_STATS             (0)
#endif

/// \brief  Compile in academic solvers
///
/// Set to 0 to disable compiling the following solvers:
///
/// Sherman-Morrison, Woodbury, SOR and GMRES
///
/// In addition, compilation of FLOAT, LONGDOUBLE and FLOATQ128 matrix
/// solvers will be disabled.
/// GMRES may be added to productive solvers in the future
/// again. Compiling in all solvers may increase compile
/// time significantly.
///

#ifndef NL_USE_ACADEMIC_SOLVERS
#define NL_USE_ACADEMIC_SOLVERS (1)
#endif

/// \brief  Store input values in logic_terminal_t.
///
/// Set to 1 to store values in logic_terminal_t instead of
/// accessing them indirectly by pointer from logic_net_t.
/// This approach is stricter and should identify bugs in
/// the netlist core faster.
/// By default it is disabled since it is not as fast as
/// the default approach. It is up to 20% slower.
///

#ifndef NL_USE_COPY_INSTEAD_OF_REFERENCE
#define NL_USE_COPY_INSTEAD_OF_REFERENCE (0)
#endif

/// \brief Use backward Euler integration
///
/// This will use backward Euler instead of trapezoidal integration.
///
/// FIXME: Longterm this will become a runtime setting. Only the capacitor model
/// currently has a trapezoidal version and there is no support currently for
/// variable capacitors.
/// The change will have impact on timings since trapezoidal improves timing
/// accuracy.

#ifndef NL_USE_BACKWARD_EULER
#define NL_USE_BACKWARD_EULER (1)
#endif

/// \brief  Use the truthtable implementation of 7448 instead of the coded device
///
/// FIXME: Using truthtable is a lot slower than the explicit device
///        in breakout. Performance drops by 20%. This can be fixed by
///        setting param USE_DEACTIVATE for the device.

#ifndef NL_USE_TRUTHTABLE_7448
#define NL_USE_TRUTHTABLE_7448 (0)
#endif

/// \brief  Use the truthtable implementation of 74107 instead of the coded device
///
/// FIXME: The truthtable implementation of 74107 (JK-Flipflop)
///        is included for educational purposes to demonstrate how
///        to implement state holding devices as truthtables.
///        It will completely nuke performance for pong.

#ifndef NL_USE_TRUTHTABLE_74107
#define NL_USE_TRUTHTABLE_74107 (0)
#endif

/// \brief  Use the __float128 type for matrix calculations.
///
/// Defaults to \ref PUSE_FLOAT128

#ifndef NL_USE_FLOAT128
#define NL_USE_FLOAT128 PUSE_FLOAT128
#endif

/// \brief Prefer 128bit int type for ptime if supported
///
/// Set this to one if you want to use 128 bit int for ptime.
/// This is about 10% slower on a skylake processor for pongf.
///
#ifndef NL_PREFER_INT128
#define NL_PREFER_INT128 (0)
#endif

/// \brief Support float type for matrix calculations.
///
/// Defaults to NL_USE_ACADEMIC_SOLVERS to provide faster build times

#ifndef NL_USE_FLOAT_MATRIX
#define NL_USE_FLOAT_MATRIX (NL_USE_ACADEMIC_SOLVERS)
//#define NL_USE_FLOAT_MATRIX 1
#endif

/// \brief Support long double type for matrix calculations.
///
/// Defaults to NL_USE_ACADEMIC_SOLVERS to provide faster build times

#ifndef NL_USE_LONG_DOUBLE_MATRIX
#define NL_USE_LONG_DOUBLE_MATRIX (NL_USE_ACADEMIC_SOLVERS)
//#define NL_USE_LONG_DOUBLE_MATRIX 1
#endif

//============================================================
//  DEBUGGING
//============================================================

/// \brief Enable compile time debugging code
///

#ifndef NL_DEBUG
#define NL_DEBUG                    (false)
//#define NL_DEBUG                    (true)
#endif

///
/// \}
///

namespace netlist
{
	// FIXME: need a better solution for global constants.
	struct config
	{
		//============================================================
		// Time resolution
		//============================================================

		/// \brief Resolution as clocks per second for timing
		///
		/// Uses 100 pico second resolution. This is aligned to MAME's
		/// attotime resolution.
		///
		/// The table below shows the maximum run times depending on
		/// time type size and resolution.
		///
		///  | Bits |               Res |       Seconds |   Days | Years |
		///  | ====-|               ===-|       =======-|   ====-| =====-|
		///  |  63  |     1,000,000,000 | 9,223,372,037 | 106,752| 292.3 |
		///  |  63  |    10,000,000,000 |   922,337,204 |  10,675|  29.2 |
		///  |  63  |   100,000,000,000 |    92,233,720 |   1,068|   2.9 |
		///  |  63  | 1,000,000,000,000 |     9,223,372 |     107|   0.3 |
		///
		using INTERNAL_RES = std::integral_constant<long long int, 10'000'000'000LL>; // NOLINT

		/// \brief Recommended clock to be used
		///
		/// This is the recommended clock to be used in fixed clock applications limited
		/// to 32 bit clock resolution. The MAME code (netlist.cpp) contains code
		/// illustrating how to deal with remainders if \ref NETLIST_INTERNAL_RES is
		/// bigger than NETLIST_CLOCK.
		using DEFAULT_CLOCK = std::integral_constant<int, 1'000'000'000>; // NOLINT

		/// \brief Default logic family
		///
		static constexpr const char *DEFAULT_LOGIC_FAMILY() { return "74XX"; }

		/// \brief Maximum queue size
		///
		using MAX_QUEUE_SIZE = std::integral_constant<std::size_t, 512>; // NOLINT

		/// \brief Maximum queue size for solvers
		///
		using MAX_SOLVER_QUEUE_SIZE = std::integral_constant<std::size_t, 64>; // NOLINT

		using use_float_matrix = std::integral_constant<bool, NL_USE_FLOAT_MATRIX>;
		using use_long_double_matrix = std::integral_constant<bool, NL_USE_LONG_DOUBLE_MATRIX>;
		using use_float128_matrix = std::integral_constant<bool, NL_USE_FLOAT128>;

		using use_mempool = std::integral_constant<bool, NL_USE_MEMPOOL>;

		/// \brief  Floating point types used
		///
		/// nl_fptype is the floating point type used throughout the
		/// netlist core.
		///
		///  Don't change this! Simple analog circuits like pong
		///  work with float. Kidniki just doesn't work at all.
		///
		///  FIXME: More work needed. Review magic numbers.
		///
		using fptype = double;
	};

	using nl_fptype = config::fptype;

	/// \brief  Specific constants depending on floating type
	///
	/// \tparam FT floating point type: double/float
	///
	template <typename FT>
	struct fp_constants
	{ };

	/// \brief  Specific constants for long double floating point type
	///
	template <>
	struct fp_constants<long double>
	{
		static inline constexpr long double DIODE_MAXDIFF() noexcept { return  1e100L; } // NOLINT
		static inline constexpr long double DIODE_MAXVOLT() noexcept { return  300.0L; } // NOLINT

		static inline constexpr long double TIMESTEP_MAXDIFF() noexcept { return  1e100L; } // NOLINT
		static inline constexpr long double TIMESTEP_MINDIV() noexcept { return  1e-60L; } // NOLINT

		static inline constexpr const char * name() noexcept { return "long double"; }
		static inline constexpr const char * suffix() noexcept { return "L"; }
	};

	/// \brief  Specific constants for double floating point type
	///
	template <>
	struct fp_constants<double>
	{
		static inline constexpr double DIODE_MAXDIFF() noexcept { return  1e100; } // NOLINT
		static inline constexpr double DIODE_MAXVOLT() noexcept { return  300.0; } // NOLINT

		static inline constexpr double TIMESTEP_MAXDIFF() noexcept { return  1e100; } // NOLINT
		static inline constexpr double TIMESTEP_MINDIV() noexcept { return  1e-60; } // NOLINT

		static inline constexpr const char * name() noexcept { return "double"; }
		static inline constexpr const char * suffix() noexcept { return ""; }
	};

	/// \brief  Specific constants for float floating point type
	///
	template <>
	struct fp_constants<float>
	{
		static inline constexpr float DIODE_MAXDIFF() noexcept { return  1e20F; } // NOLINT
		static inline constexpr float DIODE_MAXVOLT() noexcept { return  90.0F; } // NOLINT

		static inline constexpr float TIMESTEP_MAXDIFF() noexcept { return  1e30F; } // NOLINT
		static inline constexpr float TIMESTEP_MINDIV() noexcept { return  1e-8F; } // NOLINT

		static inline constexpr const char * name() noexcept { return "float"; }
		static inline constexpr const char * suffix() noexcept { return "f"; }
	};

#if (NL_USE_FLOAT128)
	/// \brief  Specific constants for FLOAT128 floating point type
	///
	template <>
	struct fp_constants<FLOAT128>
	{
#if 0
		// MAME compile doesn't support Q
		static inline constexpr FLOAT128 DIODE_MAXDIFF() noexcept { return  1e100Q; }
		static inline constexpr FLOAT128 DIODE_MAXVOLT() noexcept { return  300.0Q; }

		static inline constexpr FLOAT128 TIMESTEP_MAXDIFF() noexcept { return  1e100Q; }
		static inline constexpr FLOAT128 TIMESTEP_MINDIV() noexcept { return  1e-60Q; }

		static inline constexpr const char * name() noexcept { return "FLOAT128"; }
		static inline constexpr const char * suffix() noexcept { return "Q"; }
#else
	static inline constexpr FLOAT128 DIODE_MAXDIFF() noexcept { return static_cast<FLOAT128>(1e100L); }
	static inline constexpr FLOAT128 DIODE_MAXVOLT() noexcept { return static_cast<FLOAT128>(300.0L); }

	static inline constexpr FLOAT128 TIMESTEP_MAXDIFF() noexcept { return static_cast<FLOAT128>(1e100L); }
	static inline constexpr FLOAT128 TIMESTEP_MINDIV() noexcept { return static_cast<FLOAT128>(1e-60L); }

	static inline constexpr const char * name() noexcept { return "__float128"; }
	static inline constexpr const char * suffix() noexcept { return "Q"; }

#endif
	};
#endif
} // namespace netlist

//============================================================
//  Asserts
//============================================================

#if defined(MAME_DEBUG) || (NL_DEBUG == true)
#define nl_assert(x)    passert_always(x)
#else
#define nl_assert(x)    do { } while (0)
#endif
#define nl_assert_always(x, msg) passert_always_msg(x, msg)

#endif // NLCONFIG_H_
