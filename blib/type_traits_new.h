#include "config.h"

namespace blib {

    /// integral_constant
    template<typename _Tp, _Tp __v>
    struct integral_constant {
        static constexpr _Tp value = __v;
        typedef _Tp value_type;
        typedef integral_constant<_Tp, __v> type;

        constexpr operator value_type() const noexcept { return value; }

#if __cplusplus > 201103L

#define __cpp_lib_integral_constant_callable 201304L

        constexpr value_type operator()() const noexcept { return value; }

#endif
    };

#if !__cpp_inline_variables
    template<typename _Tp, _Tp __v>
    constexpr _Tp integral_constant<_Tp, __v>::value;
#endif

    /// The type used as a compile-time boolean with true value.
    using true_type = integral_constant<bool, true>;

    /// The type used as a compile-time boolean with false value.
    using false_type = integral_constant<bool, false>;

    /// @cond undocumented
    /// bool_constant for C++11
    template<bool __v>
    using __bool_constant = integral_constant<bool, __v>;
    /// @endcond

#if __cplusplus >= 201703L
# define __cpp_lib_bool_constant 201505L
    /// Alias template for compile-time boolean constant types.
    /// @since C++17
    template<bool __v>
    using bool_constant = integral_constant<bool, __v>;
#endif

    //enable_if
    //todo here

    /// remove_cv
#if __has_builtin(__remove_cv)
    template<typename _Tp>
    struct remove_cv
    { using type = __remove_cv(_Tp); };
#else
    template<typename _Tp>
    struct remove_cv
    { using type = _Tp; };

    template<typename _Tp>
    struct remove_cv<const _Tp>
    { using type = _Tp; };

    template<typename _Tp>
    struct remove_cv<volatile _Tp>
    { using type = _Tp; };

    template<typename _Tp>
    struct remove_cv<const volatile _Tp>
    { using type = _Tp; };
#endif

    // __remove_cv_t (std::remove_cv_t for C++11).
    template<typename _Tp>
    using __remove_cv_t = typename remove_cv<_Tp>::type;

    /// Alias template for remove_cv
    template<typename _Tp>
    using remove_cv_t = typename remove_cv<_Tp>::type;


    /// is_void
    template<typename _Tp>
    struct is_void : public false_type {
    };

    template<>
    struct is_void<void> : public true_type {
    };

    template<>
    struct is_void<const void> : public true_type {
    };

    template<>
    struct is_void<volatile void> : public true_type {
    };

    template<>
    struct is_void<const volatile void> : public true_type {
    };

    template<typename>
    struct __is_integral_helper
            : public false_type { };

    template<>
    struct __is_integral_helper<bool>
            : public true_type { };

    template<>
    struct __is_integral_helper<char>
            : public true_type { };

    template<>
    struct __is_integral_helper<signed char>
            : public true_type { };

    template<>
    struct __is_integral_helper<unsigned char>
            : public true_type { };

    // We want is_integral<wchar_t> to be true (and make_signed/unsigned to work)
    // even when libc doesn't provide working <wchar.h> and related functions,
    // so don't check _GLIBCXX_USE_WCHAR_T here.
    template<>
    struct __is_integral_helper<wchar_t>
            : public true_type { };

#ifdef _GLIBCXX_USE_CHAR8_T
    template<>
    struct __is_integral_helper<char8_t>
    : public true_type { };
#endif

    template<>
    struct __is_integral_helper<char16_t>
            : public true_type { };

    template<>
    struct __is_integral_helper<char32_t>
            : public true_type { };

    template<>
    struct __is_integral_helper<short>
            : public true_type { };

    template<>
    struct __is_integral_helper<unsigned short>
            : public true_type { };

    template<>
    struct __is_integral_helper<int>
            : public true_type { };

    template<>
    struct __is_integral_helper<unsigned int>
            : public true_type { };

    template<>
    struct __is_integral_helper<long>
            : public true_type { };

    template<>
    struct __is_integral_helper<unsigned long>
            : public true_type { };

    template<>
    struct __is_integral_helper<long long>
            : public true_type { };

    template<>
    struct __is_integral_helper<unsigned long long>
            : public true_type { };

#if defined(__GLIBCXX_TYPE_INT_N_0)
    __extension__
    template<>
    struct __is_integral_helper<__GLIBCXX_TYPE_INT_N_0>
            : public true_type { };

    __extension__
    template<>
    struct __is_integral_helper<unsigned __GLIBCXX_TYPE_INT_N_0>
            : public true_type { };
#endif
#if defined(__GLIBCXX_TYPE_INT_N_1)
    __extension__
  template<>
    struct __is_integral_helper<__GLIBCXX_TYPE_INT_N_1>
    : public true_type { };

  __extension__
  template<>
    struct __is_integral_helper<unsigned __GLIBCXX_TYPE_INT_N_1>
    : public true_type { };
#endif
#if defined(__GLIBCXX_TYPE_INT_N_2)
    __extension__
  template<>
    struct __is_integral_helper<__GLIBCXX_TYPE_INT_N_2>
    : public true_type { };

  __extension__
  template<>
    struct __is_integral_helper<unsigned __GLIBCXX_TYPE_INT_N_2>
    : public true_type { };
#endif
#if defined(__GLIBCXX_TYPE_INT_N_3)
    __extension__
  template<>
    struct __is_integral_helper<__GLIBCXX_TYPE_INT_N_3>
    : public true_type { };

  __extension__
  template<>
    struct __is_integral_helper<unsigned __GLIBCXX_TYPE_INT_N_3>
    : public true_type { };
#endif

    /// is_integral
    template<typename _Tp>
    struct is_integral
: public __is_integral_helper<__remove_cv_t<_Tp>>::type
    { };

    template <typename _Tp>
    inline constexpr bool is_integral_v = is_integral<_Tp>::value;
}