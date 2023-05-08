

namespace blib{

    /// integral_constant
    template<typename _Tp, _Tp __v>
    struct integral_constant
    {
        static const _Tp                      value = __v;
        typedef _Tp                           value_type;
        typedef integral_constant<_Tp, __v>   type;
    };

    typedef integral_constant<bool, true>     true_type;

    constexpr inline bool
    __is_constant_evaluated() noexcept
{
#if __cpp_if_consteval >= 202106L
    # define _GLIBCXX_HAVE_IS_CONSTANT_EVALUATED 1
    if consteval { return true; } else { return false; }
#else
    return false;
#endif
}
}
