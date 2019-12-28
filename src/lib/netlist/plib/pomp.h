// license:GPL-2.0+
// copyright-holders:Couriersud

#ifndef POMP_H_
#define POMP_H_

///
/// \file pomp.h
///
/// Wrap all OPENMP stuff here in a hopefully c++ compliant way.
///

#include "pconfig.h"
#include "ptypes.h"

#include <cstdint>

#if PHAS_OPENMP
#include "omp.h"
#endif

namespace plib {
namespace omp {

template <typename I, class T>
void for_static(std::size_t numops, const I start, const I end, const T &what)  noexcept(noexcept(what))
{
	if (numops>1000)
	{
	#if PHAS_OPENMP && PUSE_OPENMP
		#pragma omp parallel for schedule(static)
	#endif
		for (I i = start; i <  end; i++)
			what(i);
	}
	else
		for (I i = start; i <  end; i++)
			what(i);
}

template <typename I, class T>
void for_static(const I start, const I end, const T &what) noexcept(noexcept(what))
{
#if PHAS_OPENMP && PUSE_OPENMP
	#pragma omp parallel for schedule(static)
#endif
	for (I i = start; i <  end; i++)
		what(i);
}

template <typename I, class T>
void for_static_np(const I start, const I end, const T &what) noexcept(noexcept(what))
{
	for (I i = start; i <  end; i++)
		what(i);
}


inline void set_num_threads(const std::size_t threads) noexcept
{
#if PHAS_OPENMP && PUSE_OPENMP
	omp_set_num_threads(threads);
#else
	plib::unused_var(threads);
#endif
}

inline std::size_t get_max_threads() noexcept
{
#if PHAS_OPENMP && PUSE_OPENMP
	return omp_get_max_threads();
#else
	return 1;
#endif
}


// ----------------------------------------------------------------------------------------
// pdynlib: dynamic loading of libraries  ...
// ----------------------------------------------------------------------------------------

} // namespace omp
} // namespace plib

#endif // PSTRING_H_
