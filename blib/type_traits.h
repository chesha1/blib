

namespace blib {

#define _DEFINE_SPEC_0_HELPER                          \
  template<>

#define _DEFINE_SPEC_1_HELPER                          \
  template<typename _Tp>

#define _DEFINE_SPEC_2_HELPER                          \
  template<typename _Tp, typename _Cp>

    /// integral_constant
    template<typename Tp, Tp v>
    struct integral_constant {
        static const Tp value = v;
        typedef Tp value_type;
        typedef integral_constant<Tp, v> type;
    };

    typedef integral_constant<bool, true> true_type;
    typedef integral_constant<bool, false> false_type;


#define _DEFINE_SPEC(_Order, _Trait, _Type, _Value)    \
  _DEFINE_SPEC_##_Order##_HELPER                       \
    struct _Trait<_Type>                               \
    : public integral_constant<bool, _Value> { };




    template<typename>
    struct remove_cv;

    template<typename>
    struct __is_void_helper
            : public false_type {
    };
    _DEFINE_SPEC(0, __is_void_helper, void, true)

    template<typename Tp>
    struct is_void
            : public integral_constant<bool, (__is_void_helper<typename
            remove_cv<Tp>::type>::value)> {
    };

    constexpr inline bool
    __is_constant_evaluated() noexcept {
#if __cpp_if_consteval >= 202106L
# define _GLIBCXX_HAVE_IS_CONSTANT_EVALUATED 1
        if consteval { return true; } else { return false; }
#else
        return false;
#endif
    }

    /// is_const
    template<typename>
    struct is_const
            : public false_type { };

    template<typename _Tp>
    struct is_const<_Tp const>
            : public true_type { };

    template<bool __v>
    using __bool_constant = integral_constant<bool, __v>;

    /// is_function
    template<typename _Tp>
    struct is_function
            : public __bool_constant<!is_const<const _Tp>::value> { };


    /// is_reference
    template<typename _Tp>
    struct is_reference
            : public false_type
    { };

    template<typename _Tp>
    struct is_reference<_Tp&>
            : public true_type
    { };

    template<typename _Tp>
    struct is_reference<_Tp&&>
            : public true_type
    { };


    template<typename _Tp>
    struct is_function<_Tp&>
            : public false_type { };

    template<typename _Tp>
    struct is_function<_Tp&&>
            : public false_type { };

    template<bool, typename _Tp = void>
    struct enable_if
    { };

    // Partial specialization for true.
    template<typename _Tp>
    struct enable_if<true, _Tp>
    { typedef _Tp type; };

    // __enable_if_t (std::enable_if_t for C++11)
    template<bool _Cond, typename _Tp = void>
    using __enable_if_t = typename enable_if<_Cond, _Tp>::type;

    template<typename _Tp, typename...>
    using __first_t = _Tp;


    namespace __detail
    {
        // A variadic alias template that resolves to its first argument.
        template<typename _Tp, typename...>
        using __first_t = _Tp;

        // These are deliberately not defined.
        template<typename... _Bn>
        auto __or_fn(int) -> __first_t<false_type,
                __enable_if_t<!bool(_Bn::value)>...>;

        template<typename... _Bn>
        auto __or_fn(...) -> true_type;

        template<typename... _Bn>
        auto __and_fn(int) -> __first_t<true_type,
                __enable_if_t<bool(_Bn::value)>...>;

        template<typename... _Bn>
        auto __and_fn(...) -> false_type;
    } // namespace detail

    template<typename... _Bn>
    struct __or_
            : decltype(__detail::__or_fn<_Bn...>(0))
    { };

    template<typename _Pp>
    struct __not_
            : __bool_constant<!bool(_Pp::value)>
    { };

    /// is_object
    template<typename _Tp>
    struct is_object
            : public __not_<__or_<is_function<_Tp>, is_reference<_Tp>,
              is_void<_Tp>>>::type
{ };

    template <typename _Tp>
    inline constexpr bool is_object_v = is_object<_Tp>::value;
}
