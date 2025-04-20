// license:BSD-3-Clause
// copyright-holders:Couriersud

#ifndef PGSL_H_
#define PGSL_H_

///
/// \file pgsl.h
///
/// This core guidelines gsl implementation currently provides only enough
/// functionality to syntactically support a very limited number of the
/// gsl specification.
///
/// Going forward this approach may be extended.
///

#include "pconfig.h"
#include "pexception.h"
#include "ptypes.h"

#include <exception>
#include <optional>
#include <type_traits>
#include <utility>

#if defined(__has_builtin) // clang and gcc 10
	#if __has_builtin(__builtin_unreachable)
		#define gsl_Expects(e) ((e) ? static_cast<void>(0) : __builtin_unreachable())
	#else
		#define gsl_Expects(e) ((e) ? static_cast<void>(0) : static_cast<void>(0))
	#endif
#elif defined(__GNUC__) && !(defined( __CUDACC__ ) && defined( __CUDA_ARCH__ ))
	//#define gsl_Expects(e) ((e) ? static_cast<void>(0) : __builtin_unreachable())
	#define gsl_Expects(e) (__builtin_expect(!!(e), 1) ? static_cast<void>(0) : __builtin_unreachable())
#elif defined(_MSC_VER)
	#define gsl_Expects(e) __assume(e)
#else
	#define gsl_Expects(e) ((e) ? static_cast<void>(0) : static_cast<void>(0))
#endif

#if 1
// FIXME: __builtin_unreachable contrary to google sources does not seem to be of
// any use and decreases performance slightly. Convert gsl_Expects to a noop
// and revisit in the future.
#undef gsl_Expects
#define gsl_Expects(e) do {} while (0)

#if 0
// Alternative and c++ style implementation. Suffers from the same drawbacks
// like __builtin_unreachable
static constexpr inline void gsl_Expects(const bool &e)
{
	if (!e)
		__builtin_unreachable();
}
#endif
#endif

#define gsl_Ensures(e) gsl_Expects(e)

namespace plib {
	namespace pgsl {

		struct narrowing_error : public std::exception {};

		template <typename T>
		using owner = T;

		template <typename T>
		using not_null = T;

		/// \brief perform a narrowing cast without checks
		///
		template <typename T, typename O>
		constexpr T narrow_cast(O && v) noexcept
		{
			static_assert(plib::is_arithmetic<T>::value && std::is_convertible<std::remove_reference_t<O>, T>::value, "narrow cast expects conversion between arithmetic types");
			return static_cast<T>(std::forward<O>(v));
		}

		/// \brief perform a narrowing cast terminating if loss of precision
		///
		/// The c++ core guidelines require the narrow function to raise an error
		/// This will make narrow noexcept(false). This has shown to have a
		/// measurable impact on performance and thus we deviate from
		/// the standard here.
		///
		template <typename T, typename O>
		inline T narrow(O && v) noexcept
		{
			static_assert(plib::is_arithmetic<T>::value && std::is_convertible<std::remove_reference_t<O>, T>::value, "narrow cast expects conversion between arithmetic types");

			const auto val = static_cast<T>(std::forward<O>(v));
			if( v == static_cast<std::remove_reference_t<O>>(val))
			{
				return val;
			}

			plib::terminate("narrowing_error");
			// throw narrowing_error();
		}

	} // namespace pgsl

	/// \brief dynamic downcast from base type pointer to derived type pointer
	///
	/// This is a `noexcept` dynamic_cast implementation. It will return the cast
	/// wrapped into a std::optional type forcing the caller to
	/// examine if the conversion was successful.
	///
	/// \tparam D Derived type
	/// \tparam B Base type
	/// \param  base pointer to type B
	/// \returns return_success_t
	///
	template <typename D, typename B>
	std::enable_if_t<std::is_pointer_v<D> && std::is_pointer_v<std::remove_reference_t<B>>,
		std::optional<D>>
	dynamic_downcast(B base) noexcept
	{
		D ret = dynamic_cast<D>(base);
		if (ret != nullptr)
			return ret;
		return std::nullopt;
	}

	/// \brief dynamic downcast from base type reference to derived type pointer
	///
	/// This is a `noexcept` dynamic_cast implementation. It will return the
	/// pointer cast wrapped into a std::optional type forcing the caller to
	/// examine if the conversion was successful.
	///
	/// The return value is a pointer cast since there is no std::optional for
	/// references. Such a construct is considered ill-formed according to
	/// the std::optional specification (Section 5.2, p1).
	///
	/// \tparam D Derived type
	/// \tparam B Base type
	/// \param  base reference of type B
	/// \returns return_success_t
	///
	template <typename D, typename B>
	std::enable_if_t<std::is_pointer_v<D> &&!std::is_pointer_v<std::remove_reference_t<B>> ,
		std::optional<D>>
	dynamic_downcast(B &base) noexcept
	{
		D ret = dynamic_cast<D>(&base);
		if (ret != nullptr)
			return ret;
		return std::nullopt;
	}

	/// \brief downcast from base type to derived type
	///
	/// The cpp core guidelines require a very careful use of static cast
	/// for downcast operations. This template is used to identify these uses
	/// and later for debug builds use dynamic_cast.
	///
	template <typename D, typename B>
	constexpr D downcast(B && b) noexcept
	{
		static_assert((std::is_pointer<D>::value && std::is_pointer<B>::value)
			|| (std::is_reference<D>::value && std::is_reference<B>::value), "downcast only supports pointers or reference for derived");
		return static_cast<D>(std::forward<B>(b));
	}

	using pgsl::narrow_cast;

	/// \brief cast to void *
	///
	/// The purpose of void_ptr_cast is to help identify casts to void in the code.
	///
	template <typename T>
	constexpr void * void_ptr_cast(T *ptr) noexcept { return static_cast<void *>(ptr); }

} // namespace plib

//FIXME: This is the place to use more complete implementations
namespace gsl = plib::pgsl;

#endif // PGSL_H_
