#include "config.h"
#include "type_traits.h"

namespace blib {

    template<typename Tp>
    class new_allocator {
    public:
//        typedef Tp value_type;
//        typedef blib::ptrdiff_t difference_type;

//        typedef std::true_type propagate_on_container_move_assignment;

        new_allocator() noexcept = default;

        new_allocator(const new_allocator &) noexcept = default;

        template<typename Tp1>
        new_allocator(const new_allocator<Tp1> &) noexcept {};

        //分配内存，调用::operator new
        Tp *allocate(blib::size_t n, const void * = static_cast<const void *>(0)) {

            //异常处理，分配量太大抛出异常
            if (__builtin_expect(n > this->_M_max_size(), false)) {
                // _GLIBCXX_RESOLVE_LIB_DEFECTS
                // 3190. allocator::allocate sometimes returns too little storage
                if (n > (blib::size_t(-1) / sizeof(Tp)))
                    std::__throw_bad_array_new_length();
                std::__throw_bad_alloc();
            }

            //default 16，如果对齐量大就调用相应的::operator new
            if (alignof(Tp) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
                std::align_val_t __al = std::align_val_t(alignof(Tp));
                return static_cast<Tp *>(::operator new(n * sizeof(Tp), __al));
            }

            return static_cast<Tp *>(::operator new(n * sizeof(Tp)));
        }

        void
        deallocate(Tp *p, blib::size_t n) {
            if (alignof(Tp) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
//                _GLIBCXX_OPERATOR_DELETE(_GLIBCXX_SIZED_DEALLOC(p, n),
//                                         std::align_val_t(alignof(Tp)));
                ::operator delete(p, n * sizeof(Tp), std::align_val_t(alignof(Tp)));
                return;
            }

//            ::operator delete(_GLIBCXX_SIZED_DEALLOC(__p, __n));
            ::operator delete(p, n * sizeof(Tp));
        }


    private:

        blib::size_t _M_max_size() const {
#if __PTRDIFF_MAX__ < __SIZE_MAX__
            return blib::size_t(__PTRDIFF_MAX__) / sizeof(Tp);
#else
            return blib::size_t(-1) / sizeof(Tp);
#endif
        }

    };

    template<typename Tp>
    class malloc_allocator {
    public:
//        typedef Tp value_type;
//        typedef blib::ptrdiff_t difference_type;
//
//        typedef std::true_type propagate_on_container_move_assignment;

        malloc_allocator() noexcept {}

        malloc_allocator(const malloc_allocator &) noexcept {}

        template<typename Tp1>
        malloc_allocator(const malloc_allocator<Tp1> &) noexcept {}

        Tp *allocate(blib::size_t n, const void * = 0) {

            // _GLIBCXX_RESOLVE_LIB_DEFECTS
            // 3308. std::allocator<void>().allocate(n)
            static_assert(sizeof(Tp) != 0, "cannot allocate incomplete types");

            if (__builtin_expect(n > this->_M_max_size(), false)) {
                // _GLIBCXX_RESOLVE_LIB_DEFECTS
                // 3190. allocator::allocate sometimes returns too little storage
                if (n > (blib::size_t(-1) / sizeof(Tp)))
                    std::__throw_bad_array_new_length();
                std::__throw_bad_alloc();
            }

            Tp *ret = 0;

            if (alignof(Tp) > alignof(std::max_align_t)) {
                ret = static_cast<Tp *>(::aligned_alloc(alignof(Tp),
                                                        n * sizeof(Tp)));
            }


            if (!ret)
                ret = static_cast<Tp *>(std::malloc(n * sizeof(Tp)));
            if (!ret)
                std::__throw_bad_alloc();

            return ret;
        }

        void deallocate(Tp *p, blib::size_t) {
            std::free(static_cast<void *>(p));
        }

    private:
        blib::size_t _M_max_size() const noexcept {
#if __PTRDIFF_MAX__ < __SIZE_MAX__
            return blib::size_t(__PTRDIFF_MAX__) / sizeof(Tp);
#else
            return blib::size_t(-1) / sizeof(Tp);
#endif
        }
    };

    template<typename Tp>
    using base_allocator = new_allocator<Tp>;

    template<typename Tp>
    class allocator : public base_allocator<Tp>
    {
    public:
        typedef Tp        value_type;
        typedef blib::size_t    size_type;
        typedef blib::ptrdiff_t  difference_type;



        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 2103. std::allocator propagate_on_container_move_assignment
        using propagate_on_container_move_assignment = blib::true_type;

        using is_always_equal = blib::true_type;


        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 3035. std::allocator's constructors should be constexpr
        allocator() { }


        allocator(const allocator& a): base_allocator<Tp>(a) { }


        // Avoid implicit deprecation.
        allocator& operator=(const allocator&) = default;


        template<typename Tp1>
        allocator(const allocator<Tp1>&) { }



        constexpr
        ~allocator() { }

#if __cplusplus > 201703L
        [[nodiscard,__gnu__::__always_inline__]]
        constexpr Tp*
        allocate(size_t __n)
        {
            if (blib::__is_constant_evaluated())
            {
                if (__builtin_mul_overflow(__n, sizeof(Tp), &__n))
                    std::__throw_bad_array_new_length();
                return static_cast<Tp*>(::operator new(__n));
            }

            return base_allocator<Tp>::allocate(__n, 0);
        }


        constexpr void
        deallocate(Tp* p, size_t n)
        {
            if (blib::__is_constant_evaluated())
            {
                ::operator delete(p);
                return;
            }
            base_allocator<Tp>::deallocate(p, n);
        }
#endif // C++20

        friend __attribute__((__always_inline__))
        bool
        operator==(const allocator&, const allocator&)
        { return true; }

#if __cpp_impl_three_way_comparison < 201907L
        friend __attribute__((__always_inline__)) _GLIBCXX20_CONSTEXPR
      bool
      operator!=(const allocator&, const allocator&) _GLIBCXX_NOTHROW
      { return false; }
#endif

        // Inherit everything else.
    };


};


