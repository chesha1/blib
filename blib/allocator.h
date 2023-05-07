#include "config.h"

namespace blib {

    template<typename Tp>
    class new_allocator {
    public:
        typedef Tp value_type;
        //typedef _size_t size_type;
        typedef _size_t difference_type;

        typedef std::true_type propagate_on_container_move_assignment;

        new_allocator() noexcept = default;

        new_allocator(const new_allocator &) noexcept = default;

        template<typename Tp1>
        new_allocator(const new_allocator<Tp1> &) noexcept {};

        //分配内存，调用::operator new
        Tp *allocate(_size_t n, const void * = static_cast<const void *>(0)) {

            //异常处理，分配量太大抛出异常
            if (__builtin_expect(n > this->_M_max_size(), false)) {
                // _GLIBCXX_RESOLVE_LIB_DEFECTS
                // 3190. allocator::allocate sometimes returns too little storage
                if (n > (_size_t (-1) / sizeof(Tp)))
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


    private:

        _size_t _M_max_size() const {
#if __PTRDIFF_MAX__ < __SIZE_MAX__
            return _size_t(__PTRDIFF_MAX__) / sizeof(Tp);
#else
            return _size_t(-1) / sizeof(_Tp);
#endif
        }

    };
};


