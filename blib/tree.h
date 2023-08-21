#ifndef BLIB_TREE_H
#define BLIB_TREE_H

#  define _GLIBCXX_NOEXCEPT_IF(...) noexcept(__VA_ARGS__)

#include <cstddef>
#include <memory>

namespace blib {

    template<typename _Alloc, typename = typename _Alloc::value_type>
    struct __alloc_traits: std::allocator_traits<_Alloc>

    {
        typedef _Alloc allocator_type;
        typedef std::allocator_traits<_Alloc>           _Base_type;
        typedef typename _Base_type::value_type         value_type;
        typedef typename _Base_type::pointer            pointer;
        typedef typename _Base_type::const_pointer      const_pointer;
        typedef typename _Base_type::size_type          size_type;
        typedef typename _Base_type::difference_type    difference_type;
        // C++11 allocators do not define reference or const_reference
        typedef value_type&                             reference;
        typedef const value_type&                       const_reference;
        using _Base_type::allocate;
        using _Base_type::deallocate;
        using _Base_type::construct;
        using _Base_type::destroy;
        using _Base_type::max_size;

    private:
        template<typename _Ptr>
        using __is_custom_pointer
                = std::__and_<std::is_same<pointer, _Ptr>,
                std::__not_<std::is_pointer<_Ptr>>>;

    public:
        // overload construct for non-standard pointer types
        template<typename _Ptr, typename... _Args>
        [[__gnu__::__always_inline__]]
        static _GLIBCXX14_CONSTEXPR
        std::__enable_if_t<__is_custom_pointer<_Ptr>::value>
        construct(_Alloc& __a, _Ptr __p, _Args&&... __args)
        noexcept(noexcept(_Base_type::construct(__a, std::__to_address(__p),
                                                std::forward<_Args>(__args)...)))
        {
            _Base_type::construct(__a, std::__to_address(__p),
                                  std::forward<_Args>(__args)...);
        }

        // overload destroy for non-standard pointer types
        template<typename _Ptr>
        [[__gnu__::__always_inline__]]
        static _GLIBCXX14_CONSTEXPR
        std::__enable_if_t<__is_custom_pointer<_Ptr>::value>
        destroy(_Alloc& __a, _Ptr __p)
        noexcept(noexcept(_Base_type::destroy(__a, std::__to_address(__p))))
        { _Base_type::destroy(__a, std::__to_address(__p)); }

        [[__gnu__::__always_inline__]]
        static constexpr _Alloc _S_select_on_copy(const _Alloc& __a)
        { return _Base_type::select_on_container_copy_construction(__a); }

        [[__gnu__::__always_inline__]]
        static _GLIBCXX14_CONSTEXPR void _S_on_swap(_Alloc& __a, _Alloc& __b)
        { std::__alloc_on_swap(__a, __b); }

        [[__gnu__::__always_inline__]]
        static constexpr bool _S_propagate_on_copy_assign()
        { return _Base_type::propagate_on_container_copy_assignment::value; }

        [[__gnu__::__always_inline__]]
        static constexpr bool _S_propagate_on_move_assign()
        { return _Base_type::propagate_on_container_move_assignment::value; }

        [[__gnu__::__always_inline__]]
        static constexpr bool _S_propagate_on_swap()
        { return _Base_type::propagate_on_container_swap::value; }

        [[__gnu__::__always_inline__]]
        static constexpr bool _S_always_equal()
        { return _Base_type::is_always_equal::value; }

        __attribute__((__always_inline__))
        static constexpr bool _S_nothrow_move()
        { return _S_propagate_on_move_assign() || _S_always_equal(); }

        template<typename _Tp>
        struct rebind
        { typedef typename _Base_type::template rebind_alloc<_Tp> other; };

    };


    template<typename _Tp>
    struct __aligned_membuf
    {
        // Target macro ADJUST_FIELD_ALIGN can produce different alignment for
        // types when used as class members. __aligned_membuf is intended
        // for use as a class member, so align the buffer as for a class member.
        // Since GCC 8 we could just use alignof(_Tp) instead, but older
        // versions of non-GNU compilers might still need this trick.
        struct _Tp2 { _Tp _M_t; };

        alignas(__alignof__(_Tp2::_M_t)) unsigned char _M_storage[sizeof(_Tp)];

        __aligned_membuf() = default;

        // Can be used to avoid value-initialization zeroing _M_storage.
        __aligned_membuf(std::nullptr_t) { }

        void*
        _M_addr() noexcept
        { return static_cast<void*>(&_M_storage); }

        const void*
        _M_addr() const noexcept
        { return static_cast<const void*>(&_M_storage); }

        _Tp*
        _M_ptr() noexcept
        { return static_cast<_Tp*>(_M_addr()); }

        const _Tp*
        _M_ptr() const noexcept
        { return static_cast<const _Tp*>(_M_addr()); }
    };

    enum _Rb_tree_color {
        _S_red = false, _S_black = true
    };

    struct _Rb_tree_node_base {
        typedef _Rb_tree_node_base *_Base_ptr;
        typedef const _Rb_tree_node_base *_Const_Base_ptr;

        _Rb_tree_color _M_color;
        _Base_ptr _M_parent;
        _Base_ptr _M_left;
        _Base_ptr _M_right;

        static _Base_ptr
        _S_minimum(_Base_ptr __x) noexcept {
            while (__x->_M_left != 0) __x = __x->_M_left;
            return __x;
        }

        static _Const_Base_ptr
        _S_minimum(_Const_Base_ptr __x) noexcept {
            while (__x->_M_left != 0) __x = __x->_M_left;
            return __x;
        }

        static _Base_ptr
        _S_maximum(_Base_ptr __x) noexcept {
            while (__x->_M_right != 0) __x = __x->_M_right;
            return __x;
        }

        static _Const_Base_ptr
        _S_maximum(_Const_Base_ptr __x) noexcept {
            while (__x->_M_right != 0) __x = __x->_M_right;
            return __x;
        }
    };


    template<typename _Val>
    struct _Rb_tree_node : public _Rb_tree_node_base
    {
        typedef _Rb_tree_node<_Val>* _Link_type;

#if __cplusplus < 201103L
        _Val _M_value_field;

      _Val*
      _M_valptr()
      { return std::__addressof(_M_value_field); }

      const _Val*
      _M_valptr() const
      { return std::__addressof(_M_value_field); }
#else
        __aligned_membuf<_Val> _M_storage;

        _Val*
        _M_valptr()
        { return _M_storage._M_ptr(); }

        const _Val*
        _M_valptr() const
        { return _M_storage._M_ptr(); }
#endif
    };

    // Helper type to manage default initialization of node count and header.
    struct _Rb_tree_header
    {
        _Rb_tree_node_base	_M_header;
        size_t		_M_node_count; // Keeps track of size of tree.

        _Rb_tree_header() _GLIBCXX_NOEXCEPT
        {
            _M_header._M_color = _S_red;
            _M_reset();
        }

#if __cplusplus >= 201103L
        _Rb_tree_header(_Rb_tree_header&& __x) noexcept
        {
            if (__x._M_header._M_parent != nullptr)
                _M_move_data(__x);
            else
            {
                _M_header._M_color = _S_red;
                _M_reset();
            }
        }
#endif

        void
        _M_move_data(_Rb_tree_header& __from)
        {
            _M_header._M_color = __from._M_header._M_color;
            _M_header._M_parent = __from._M_header._M_parent;
            _M_header._M_left = __from._M_header._M_left;
            _M_header._M_right = __from._M_header._M_right;
            _M_header._M_parent->_M_parent = &_M_header;
            _M_node_count = __from._M_node_count;

            __from._M_reset();
        }

        void
        _M_reset()
        {
            _M_header._M_parent = 0;
            _M_header._M_left = &_M_header;
            _M_header._M_right = &_M_header;
            _M_node_count = 0;
        }
    };


    // Helper type offering value initialization guarantee on the compare functor.
    template<typename _Key_compare>
    struct _Rb_tree_key_compare
    {
        _Key_compare		_M_key_compare;

        _Rb_tree_key_compare()
        _GLIBCXX_NOEXCEPT_IF(
                std::is_nothrow_default_constructible<_Key_compare>::value)
                : _M_key_compare()
        { }

        _Rb_tree_key_compare(const _Key_compare& __comp)
                : _M_key_compare(__comp)
        { }

#if __cplusplus >= 201103L
        // Copy constructor added for consistency with C++98 mode.
        _Rb_tree_key_compare(const _Rb_tree_key_compare&) = default;

        _Rb_tree_key_compare(_Rb_tree_key_compare&& __x)
        noexcept(std::is_nothrow_copy_constructible<_Key_compare>::value)
                : _M_key_compare(__x._M_key_compare)
        { }
#endif
    };


//    template<typename _Key, typename _Val, typename _KeyOfValue,
//            typename _Compare, typename _Alloc = std::allocator<_Val> >
//    class _Rb_tree
//    {
//        typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template
//        rebind<_Rb_tree_node<_Val> >::other _Node_allocator;
//
//        typedef __gnu_cxx::__alloc_traits<_Node_allocator> _Alloc_traits;
//
//    protected:
//        typedef _Rb_tree_node_base* 		_Base_ptr;
//        typedef const _Rb_tree_node_base* 	_Const_Base_ptr;
//        typedef _Rb_tree_node<_Val>* 		_Link_type;
//        typedef const _Rb_tree_node<_Val>*	_Const_Link_type;
//
//    private:
//        // Functor recycling a pool of nodes and using allocation once the pool
//        // is empty.
//        struct _Reuse_or_alloc_node
//        {
//            _Reuse_or_alloc_node(_Rb_tree& __t)
//                    : _M_root(__t._M_root()), _M_nodes(__t._M_rightmost()), _M_t(__t)
//            {
//                if (_M_root)
//                {
//                    _M_root->_M_parent = 0;
//
//                    if (_M_nodes->_M_left)
//                        _M_nodes = _M_nodes->_M_left;
//                }
//                else
//                    _M_nodes = 0;
//            }
//
//
//            _Reuse_or_alloc_node(const _Reuse_or_alloc_node&) = delete;
//
//
//            ~_Reuse_or_alloc_node()
//            { _M_t._M_erase(static_cast<_Link_type>(_M_root)); }
//
//            template<typename _Arg>
//            _Link_type
//            operator()(_GLIBCXX_FWDREF(_Arg) __arg)
//            {
//                _Link_type __node = static_cast<_Link_type>(_M_extract());
//                if (__node)
//                {
//                    _M_t._M_destroy_node(__node);
//                    _M_t._M_construct_node(__node, _GLIBCXX_FORWARD(_Arg, __arg));
//                    return __node;
//                }
//
//                return _M_t._M_create_node(_GLIBCXX_FORWARD(_Arg, __arg));
//            }
//
//        private:
//            _Base_ptr
//            _M_extract()
//            {
//                if (!_M_nodes)
//                    return _M_nodes;
//
//                _Base_ptr __node = _M_nodes;
//                _M_nodes = _M_nodes->_M_parent;
//                if (_M_nodes)
//                {
//                    if (_M_nodes->_M_right == __node)
//                    {
//                        _M_nodes->_M_right = 0;
//
//                        if (_M_nodes->_M_left)
//                        {
//                            _M_nodes = _M_nodes->_M_left;
//
//                            while (_M_nodes->_M_right)
//                                _M_nodes = _M_nodes->_M_right;
//
//                            if (_M_nodes->_M_left)
//                                _M_nodes = _M_nodes->_M_left;
//                        }
//                    }
//                    else // __node is on the left.
//                        _M_nodes->_M_left = 0;
//                }
//                else
//                    _M_root = 0;
//
//                return __node;
//            }
//
//            _Base_ptr _M_root;
//            _Base_ptr _M_nodes;
//            _Rb_tree& _M_t;
//        };
//
//        // Functor similar to the previous one but without any pool of nodes to
//        // recycle.
//        struct _Alloc_node
//        {
//            _Alloc_node(_Rb_tree& __t)
//                    : _M_t(__t) { }
//
//            template<typename _Arg>
//            _Link_type
//            operator()(_GLIBCXX_FWDREF(_Arg) __arg) const
//            { return _M_t._M_create_node(_GLIBCXX_FORWARD(_Arg, __arg)); }
//
//        private:
//            _Rb_tree& _M_t;
//        };
//
//    public:
//        typedef _Key 				key_type;
//        typedef _Val 				value_type;
//        typedef value_type* 			pointer;
//        typedef const value_type* 		const_pointer;
//        typedef value_type& 			reference;
//        typedef const value_type& 		const_reference;
//        typedef size_t 				size_type;
//        typedef ptrdiff_t 			difference_type;
//        typedef _Alloc 				allocator_type;
//
//        _Node_allocator&
//        _M_get_Node_allocator() _GLIBCXX_NOEXCEPT
//        { return this->_M_impl; }
//
//        const _Node_allocator&
//        _M_get_Node_allocator() const _GLIBCXX_NOEXCEPT
//        { return this->_M_impl; }
//
//        allocator_type
//        get_allocator() const _GLIBCXX_NOEXCEPT
//        { return allocator_type(_M_get_Node_allocator()); }
//
//    protected:
//        _Link_type
//        _M_get_node()
//        { return _Alloc_traits::allocate(_M_get_Node_allocator(), 1); }
//
//        void
//        _M_put_node(_Link_type __p) _GLIBCXX_NOEXCEPT
//        { _Alloc_traits::deallocate(_M_get_Node_allocator(), __p, 1); }
//
//
//        template<typename... _Args>
//        void
//        _M_construct_node(_Link_type __node, _Args&&... __args)
//        {
//            __try
//            {
//                ::new(__node) _Rb_tree_node<_Val>;
//                _Alloc_traits::construct(_M_get_Node_allocator(),
//                                         __node->_M_valptr(),
//                                         std::forward<_Args>(__args)...);
//            }
//            __catch(...)
//            {
//                __node->~_Rb_tree_node<_Val>();
//                _M_put_node(__node);
//                __throw_exception_again;
//            }
//        }
//
//        template<typename... _Args>
//        _Link_type
//        _M_create_node(_Args&&... __args)
//        {
//            _Link_type __tmp = _M_get_node();
//            _M_construct_node(__tmp, std::forward<_Args>(__args)...);
//            return __tmp;
//        }
//
//        void
//        _M_destroy_node(_Link_type __p) _GLIBCXX_NOEXCEPT
//        {
//
//            _Alloc_traits::destroy(_M_get_Node_allocator(), __p->_M_valptr());
//            __p->~_Rb_tree_node<_Val>();
//
//        }
//
//        void
//        _M_drop_node(_Link_type __p) _GLIBCXX_NOEXCEPT
//        {
//            _M_destroy_node(__p);
//            _M_put_node(__p);
//        }
//
//        template<bool _MoveValue, typename _NodeGen>
//        _Link_type
//        _M_clone_node(_Link_type __x, _NodeGen& __node_gen)
//        {
//
//            using _Vp = std::__conditional_t<_MoveValue,
//                    value_type&&,
//                    const value_type&>;
//
//            _Link_type __tmp
//                    = __node_gen(_GLIBCXX_FORWARD(_Vp, *__x->_M_valptr()));
//            __tmp->_M_color = __x->_M_color;
//            __tmp->_M_left = 0;
//            __tmp->_M_right = 0;
//            return __tmp;
//        }
//
//    protected:
//#if _GLIBCXX_INLINE_VERSION
//        template<typename _Key_compare>
//#else
//        // Unused _Is_pod_comparator is kept as it is part of mangled name.
//        template<typename _Key_compare,
//                bool /* _Is_pod_comparator */ = __is_pod(_Key_compare)>
//#endif
//        struct _Rb_tree_impl
//                : public _Node_allocator
//                        , public _Rb_tree_key_compare<_Key_compare>
//                        , public _Rb_tree_header
//        {
//            typedef _Rb_tree_key_compare<_Key_compare> _Base_key_compare;
//
//            _Rb_tree_impl()
//            _GLIBCXX_NOEXCEPT_IF(
//                    std::is_nothrow_default_constructible<_Node_allocator>::value
//                    && std::is_nothrow_default_constructible<_Base_key_compare>::value )
//                    : _Node_allocator()
//            { }
//
//            _Rb_tree_impl(const _Rb_tree_impl& __x)
//                    : _Node_allocator(_Alloc_traits::_S_select_on_copy(__x))
//                    , _Base_key_compare(__x._M_key_compare)
//                    , _Rb_tree_header()
//            { }
//
//#if __cplusplus < 201103L
//            _Rb_tree_impl(const _Key_compare& __comp, const _Node_allocator& __a)
//	  : _Node_allocator(__a), _Base_key_compare(__comp)
//	  { }
//#else
//            _Rb_tree_impl(_Rb_tree_impl&&)
//            noexcept( std::is_nothrow_move_constructible<_Base_key_compare>::value )
//            = default;
//
//            explicit
//            _Rb_tree_impl(_Node_allocator&& __a)
//                    : _Node_allocator(std::move(__a))
//            { }
//
//            _Rb_tree_impl(_Rb_tree_impl&& __x, _Node_allocator&& __a)
//                    : _Node_allocator(std::move(__a)),
//                      _Base_key_compare(std::move(__x)),
//                      _Rb_tree_header(std::move(__x))
//            { }
//
//            _Rb_tree_impl(const _Key_compare& __comp, _Node_allocator&& __a)
//                    : _Node_allocator(std::move(__a)), _Base_key_compare(__comp)
//            { }
//#endif
//        };
//
//        _Rb_tree_impl<_Compare> _M_impl;
//
//    protected:
//        _Base_ptr&
//        _M_root() _GLIBCXX_NOEXCEPT
//        { return this->_M_impl._M_header._M_parent; }
//
//        _Const_Base_ptr
//        _M_root() const _GLIBCXX_NOEXCEPT
//        { return this->_M_impl._M_header._M_parent; }
//
//        _Base_ptr&
//        _M_leftmost() _GLIBCXX_NOEXCEPT
//        { return this->_M_impl._M_header._M_left; }
//
//        _Const_Base_ptr
//        _M_leftmost() const _GLIBCXX_NOEXCEPT
//        { return this->_M_impl._M_header._M_left; }
//
//        _Base_ptr&
//        _M_rightmost() _GLIBCXX_NOEXCEPT
//        { return this->_M_impl._M_header._M_right; }
//
//        _Const_Base_ptr
//        _M_rightmost() const _GLIBCXX_NOEXCEPT
//        { return this->_M_impl._M_header._M_right; }
//
//        _Link_type
//        _M_mbegin() const _GLIBCXX_NOEXCEPT
//        { return static_cast<_Link_type>(this->_M_impl._M_header._M_parent); }
//
//        _Link_type
//        _M_begin() _GLIBCXX_NOEXCEPT
//        { return _M_mbegin(); }
//
//        _Const_Link_type
//        _M_begin() const _GLIBCXX_NOEXCEPT
//        {
//            return static_cast<_Const_Link_type>
//            (this->_M_impl._M_header._M_parent);
//        }
//
//        _Base_ptr
//        _M_end() _GLIBCXX_NOEXCEPT
//        { return &this->_M_impl._M_header; }
//
//        _Const_Base_ptr
//        _M_end() const _GLIBCXX_NOEXCEPT
//        { return &this->_M_impl._M_header; }
//
//        static const _Key&
//        _S_key(_Const_Link_type __x)
//        {
//#if __cplusplus >= 201103L
//            // If we're asking for the key we're presumably using the comparison
//            // object, and so this is a good place to sanity check it.
//            static_assert(std::__is_invocable<_Compare&, const _Key&, const _Key&>{},
//                          "comparison object must be invocable "
//                          "with two arguments of key type");
//# if __cplusplus >= 201703L
//            // _GLIBCXX_RESOLVE_LIB_DEFECTS
//            // 2542. Missing const requirements for associative containers
//            if constexpr (std::__is_invocable<_Compare&, const _Key&, const _Key&>{})
//                static_assert(
//                        std::is_invocable_v<const _Compare&, const _Key&, const _Key&>,
//                        "comparison object must be invocable as const");
//# endif // C++17
//#endif // C++11
//
//            return _KeyOfValue()(*__x->_M_valptr());
//        }
//
//        static _Link_type
//        _S_left(_Base_ptr __x) _GLIBCXX_NOEXCEPT
//        { return static_cast<_Link_type>(__x->_M_left); }
//
//        static _Const_Link_type
//        _S_left(_Const_Base_ptr __x) _GLIBCXX_NOEXCEPT
//        { return static_cast<_Const_Link_type>(__x->_M_left); }
//
//        static _Link_type
//        _S_right(_Base_ptr __x) _GLIBCXX_NOEXCEPT
//        { return static_cast<_Link_type>(__x->_M_right); }
//
//        static _Const_Link_type
//        _S_right(_Const_Base_ptr __x) _GLIBCXX_NOEXCEPT
//        { return static_cast<_Const_Link_type>(__x->_M_right); }
//
//        static const _Key&
//        _S_key(_Const_Base_ptr __x)
//        { return _S_key(static_cast<_Const_Link_type>(__x)); }
//
//        static _Base_ptr
//        _S_minimum(_Base_ptr __x) _GLIBCXX_NOEXCEPT
//        { return _Rb_tree_node_base::_S_minimum(__x); }
//
//        static _Const_Base_ptr
//        _S_minimum(_Const_Base_ptr __x) _GLIBCXX_NOEXCEPT
//        { return _Rb_tree_node_base::_S_minimum(__x); }
//
//        static _Base_ptr
//        _S_maximum(_Base_ptr __x) _GLIBCXX_NOEXCEPT
//        { return _Rb_tree_node_base::_S_maximum(__x); }
//
//        static _Const_Base_ptr
//        _S_maximum(_Const_Base_ptr __x) _GLIBCXX_NOEXCEPT
//        { return _Rb_tree_node_base::_S_maximum(__x); }
//
//    public:
//        typedef _Rb_tree_iterator<value_type>       iterator;
//        typedef _Rb_tree_const_iterator<value_type> const_iterator;
//
//        typedef std::reverse_iterator<iterator>       reverse_iterator;
//        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
//
//#if __cplusplus > 201402L
//        using node_type = _Node_handle<_Key, _Val, _Node_allocator>;
//        using insert_return_type = _Node_insert_return<
//        __conditional_t<is_same_v<_Key, _Val>, const_iterator, iterator>,
//        node_type>;
//#endif
//
//        pair<_Base_ptr, _Base_ptr>
//        _M_get_insert_unique_pos(const key_type& __k);
//
//        pair<_Base_ptr, _Base_ptr>
//        _M_get_insert_equal_pos(const key_type& __k);
//
//        pair<_Base_ptr, _Base_ptr>
//        _M_get_insert_hint_unique_pos(const_iterator __pos,
//                                      const key_type& __k);
//
//        pair<_Base_ptr, _Base_ptr>
//        _M_get_insert_hint_equal_pos(const_iterator __pos,
//                                     const key_type& __k);
//
//    private:
//#if __cplusplus >= 201103L
//        template<typename _Arg, typename _NodeGen>
//        iterator
//        _M_insert_(_Base_ptr __x, _Base_ptr __y, _Arg&& __v, _NodeGen&);
//
//        iterator
//        _M_insert_node(_Base_ptr __x, _Base_ptr __y, _Link_type __z);
//
//        template<typename _Arg>
//        iterator
//        _M_insert_lower(_Base_ptr __y, _Arg&& __v);
//
//        template<typename _Arg>
//        iterator
//        _M_insert_equal_lower(_Arg&& __x);
//
//        iterator
//        _M_insert_lower_node(_Base_ptr __p, _Link_type __z);
//
//        iterator
//        _M_insert_equal_lower_node(_Link_type __z);
//#else
//        template<typename _NodeGen>
//	iterator
//	_M_insert_(_Base_ptr __x, _Base_ptr __y,
//		   const value_type& __v, _NodeGen&);
//
//      // _GLIBCXX_RESOLVE_LIB_DEFECTS
//      // 233. Insertion hints in associative containers.
//      iterator
//      _M_insert_lower(_Base_ptr __y, const value_type& __v);
//
//      iterator
//      _M_insert_equal_lower(const value_type& __x);
//#endif
//
//        enum { __as_lvalue, __as_rvalue };
//
//        template<bool _MoveValues, typename _NodeGen>
//        _Link_type
//        _M_copy(_Link_type, _Base_ptr, _NodeGen&);
//
//        template<bool _MoveValues, typename _NodeGen>
//        _Link_type
//        _M_copy(const _Rb_tree& __x, _NodeGen& __gen)
//        {
//            _Link_type __root =
//                    _M_copy<_MoveValues>(__x._M_mbegin(), _M_end(), __gen);
//            _M_leftmost() = _S_minimum(__root);
//            _M_rightmost() = _S_maximum(__root);
//            _M_impl._M_node_count = __x._M_impl._M_node_count;
//            return __root;
//        }
//
//        _Link_type
//        _M_copy(const _Rb_tree& __x)
//        {
//            _Alloc_node __an(*this);
//            return _M_copy<__as_lvalue>(__x, __an);
//        }
//
//        void
//        _M_erase(_Link_type __x);
//
//        iterator
//        _M_lower_bound(_Link_type __x, _Base_ptr __y,
//                       const _Key& __k);
//
//        const_iterator
//        _M_lower_bound(_Const_Link_type __x, _Const_Base_ptr __y,
//                       const _Key& __k) const;
//
//        iterator
//        _M_upper_bound(_Link_type __x, _Base_ptr __y,
//                       const _Key& __k);
//
//        const_iterator
//        _M_upper_bound(_Const_Link_type __x, _Const_Base_ptr __y,
//                       const _Key& __k) const;
//
//    public:
//        // allocation/deallocation
//#if __cplusplus < 201103L
//        _Rb_tree() { }
//#else
//        _Rb_tree() = default;
//#endif
//
//        _Rb_tree(const _Compare& __comp,
//                 const allocator_type& __a = allocator_type())
//                : _M_impl(__comp, _Node_allocator(__a)) { }
//
//        _Rb_tree(const _Rb_tree& __x)
//                : _M_impl(__x._M_impl)
//        {
//            if (__x._M_root() != 0)
//                _M_root() = _M_copy(__x);
//        }
//
//#if __cplusplus >= 201103L
//        _Rb_tree(const allocator_type& __a)
//                : _M_impl(_Node_allocator(__a))
//        { }
//
//        _Rb_tree(const _Rb_tree& __x, const allocator_type& __a)
//                : _M_impl(__x._M_impl._M_key_compare, _Node_allocator(__a))
//        {
//            if (__x._M_root() != nullptr)
//                _M_root() = _M_copy(__x);
//        }
//
//        _Rb_tree(_Rb_tree&&) = default;
//
//        _Rb_tree(_Rb_tree&& __x, const allocator_type& __a)
//                : _Rb_tree(std::move(__x), _Node_allocator(__a))
//        { }
//
//    private:
//        _Rb_tree(_Rb_tree&& __x, _Node_allocator&& __a, true_type)
//        noexcept(is_nothrow_default_constructible<_Compare>::value)
//                : _M_impl(std::move(__x._M_impl), std::move(__a))
//        { }
//
//        _Rb_tree(_Rb_tree&& __x, _Node_allocator&& __a, false_type)
//                : _M_impl(__x._M_impl._M_key_compare, std::move(__a))
//        {
//            if (__x._M_root() != nullptr)
//                _M_move_data(__x, false_type{});
//        }
//
//    public:
//        _Rb_tree(_Rb_tree&& __x, _Node_allocator&& __a)
//        noexcept( noexcept(
//                _Rb_tree(std::declval<_Rb_tree&&>(), std::declval<_Node_allocator&&>(),
//                         std::declval<typename _Alloc_traits::is_always_equal>())) )
//                : _Rb_tree(std::move(__x), std::move(__a),
//                           typename _Alloc_traits::is_always_equal{})
//        { }
//#endif
//
//        ~_Rb_tree() _GLIBCXX_NOEXCEPT
//        { _M_erase(_M_begin()); }
//
//        _Rb_tree&
//        operator=(const _Rb_tree& __x);
//
//        // Accessors.
//        _Compare
//        key_comp() const
//        { return _M_impl._M_key_compare; }
//
//        iterator
//        begin() _GLIBCXX_NOEXCEPT
//        { return iterator(this->_M_impl._M_header._M_left); }
//
//        const_iterator
//        begin() const _GLIBCXX_NOEXCEPT
//        { return const_iterator(this->_M_impl._M_header._M_left); }
//
//        iterator
//        end() _GLIBCXX_NOEXCEPT
//        { return iterator(&this->_M_impl._M_header); }
//
//        const_iterator
//        end() const _GLIBCXX_NOEXCEPT
//        { return const_iterator(&this->_M_impl._M_header); }
//
//        reverse_iterator
//        rbegin() _GLIBCXX_NOEXCEPT
//        { return reverse_iterator(end()); }
//
//        const_reverse_iterator
//        rbegin() const _GLIBCXX_NOEXCEPT
//        { return const_reverse_iterator(end()); }
//
//        reverse_iterator
//        rend() _GLIBCXX_NOEXCEPT
//        { return reverse_iterator(begin()); }
//
//        const_reverse_iterator
//        rend() const _GLIBCXX_NOEXCEPT
//        { return const_reverse_iterator(begin()); }
//
//        _GLIBCXX_NODISCARD bool
//        empty() const _GLIBCXX_NOEXCEPT
//        { return _M_impl._M_node_count == 0; }
//
//        size_type
//        size() const _GLIBCXX_NOEXCEPT
//        { return _M_impl._M_node_count; }
//
//        size_type
//        max_size() const _GLIBCXX_NOEXCEPT
//        { return _Alloc_traits::max_size(_M_get_Node_allocator()); }
//
//        void
//        swap(_Rb_tree& __t)
//        _GLIBCXX_NOEXCEPT_IF(__is_nothrow_swappable<_Compare>::value);
//
//        // Insert/erase.
//#if __cplusplus >= 201103L
//        template<typename _Arg>
//        pair<iterator, bool>
//        _M_insert_unique(_Arg&& __x);
//
//        template<typename _Arg>
//        iterator
//        _M_insert_equal(_Arg&& __x);
//
//        template<typename _Arg, typename _NodeGen>
//        iterator
//        _M_insert_unique_(const_iterator __pos, _Arg&& __x, _NodeGen&);
//
//        template<typename _Arg>
//        iterator
//        _M_insert_unique_(const_iterator __pos, _Arg&& __x)
//        {
//            _Alloc_node __an(*this);
//            return _M_insert_unique_(__pos, std::forward<_Arg>(__x), __an);
//        }
//
//        template<typename _Arg, typename _NodeGen>
//        iterator
//        _M_insert_equal_(const_iterator __pos, _Arg&& __x, _NodeGen&);
//
//        template<typename _Arg>
//        iterator
//        _M_insert_equal_(const_iterator __pos, _Arg&& __x)
//        {
//            _Alloc_node __an(*this);
//            return _M_insert_equal_(__pos, std::forward<_Arg>(__x), __an);
//        }
//
//        template<typename... _Args>
//        pair<iterator, bool>
//        _M_emplace_unique(_Args&&... __args);
//
//        template<typename... _Args>
//        iterator
//        _M_emplace_equal(_Args&&... __args);
//
//        template<typename... _Args>
//        iterator
//        _M_emplace_hint_unique(const_iterator __pos, _Args&&... __args);
//
//        template<typename... _Args>
//        iterator
//        _M_emplace_hint_equal(const_iterator __pos, _Args&&... __args);
//
//        template<typename _Iter>
//        using __same_value_type
//                = is_same<value_type, typename iterator_traits<_Iter>::value_type>;
//
//        template<typename _InputIterator>
//        __enable_if_t<__same_value_type<_InputIterator>::value>
//        _M_insert_range_unique(_InputIterator __first, _InputIterator __last)
//        {
//            _Alloc_node __an(*this);
//            for (; __first != __last; ++__first)
//                _M_insert_unique_(end(), *__first, __an);
//        }
//
//        template<typename _InputIterator>
//        __enable_if_t<!__same_value_type<_InputIterator>::value>
//        _M_insert_range_unique(_InputIterator __first, _InputIterator __last)
//        {
//            for (; __first != __last; ++__first)
//                _M_emplace_unique(*__first);
//        }
//
//        template<typename _InputIterator>
//        __enable_if_t<__same_value_type<_InputIterator>::value>
//        _M_insert_range_equal(_InputIterator __first, _InputIterator __last)
//        {
//            _Alloc_node __an(*this);
//            for (; __first != __last; ++__first)
//                _M_insert_equal_(end(), *__first, __an);
//        }
//
//        template<typename _InputIterator>
//        __enable_if_t<!__same_value_type<_InputIterator>::value>
//        _M_insert_range_equal(_InputIterator __first, _InputIterator __last)
//        {
//            for (; __first != __last; ++__first)
//                _M_emplace_equal(*__first);
//        }
//#else
//        pair<iterator, bool>
//      _M_insert_unique(const value_type& __x);
//
//      iterator
//      _M_insert_equal(const value_type& __x);
//
//      template<typename _NodeGen>
//	iterator
//	_M_insert_unique_(const_iterator __pos, const value_type& __x,
//			  _NodeGen&);
//
//      iterator
//      _M_insert_unique_(const_iterator __pos, const value_type& __x)
//      {
//	_Alloc_node __an(*this);
//	return _M_insert_unique_(__pos, __x, __an);
//      }
//
//      template<typename _NodeGen>
//	iterator
//	_M_insert_equal_(const_iterator __pos, const value_type& __x,
//			 _NodeGen&);
//      iterator
//      _M_insert_equal_(const_iterator __pos, const value_type& __x)
//      {
//	_Alloc_node __an(*this);
//	return _M_insert_equal_(__pos, __x, __an);
//      }
//
//      template<typename _InputIterator>
//	void
//	_M_insert_range_unique(_InputIterator __first, _InputIterator __last)
//	{
//	  _Alloc_node __an(*this);
//	  for (; __first != __last; ++__first)
//	    _M_insert_unique_(end(), *__first, __an);
//	}
//
//      template<typename _InputIterator>
//	void
//	_M_insert_range_equal(_InputIterator __first, _InputIterator __last)
//	{
//	  _Alloc_node __an(*this);
//	  for (; __first != __last; ++__first)
//	    _M_insert_equal_(end(), *__first, __an);
//	}
//#endif
//
//    private:
//        void
//        _M_erase_aux(const_iterator __position);
//
//        void
//        _M_erase_aux(const_iterator __first, const_iterator __last);
//
//    public:
//#if __cplusplus >= 201103L
//        // _GLIBCXX_RESOLVE_LIB_DEFECTS
//        // DR 130. Associative erase should return an iterator.
//        _GLIBCXX_ABI_TAG_CXX11
//        iterator
//        erase(const_iterator __position)
//        {
//            __glibcxx_assert(__position != end());
//            const_iterator __result = __position;
//            ++__result;
//            _M_erase_aux(__position);
//            return __result._M_const_cast();
//        }
//
//        // LWG 2059.
//        _GLIBCXX_ABI_TAG_CXX11
//        iterator
//        erase(iterator __position)
//        {
//            __glibcxx_assert(__position != end());
//            iterator __result = __position;
//            ++__result;
//            _M_erase_aux(__position);
//            return __result;
//        }
//#else
//        void
//      erase(iterator __position)
//      {
//	__glibcxx_assert(__position != end());
//	_M_erase_aux(__position);
//      }
//
//      void
//      erase(const_iterator __position)
//      {
//	__glibcxx_assert(__position != end());
//	_M_erase_aux(__position);
//      }
//#endif
//
//        size_type
//        erase(const key_type& __x);
//
//#if __cplusplus >= 201103L
//        // _GLIBCXX_RESOLVE_LIB_DEFECTS
//        // DR 130. Associative erase should return an iterator.
//        _GLIBCXX_ABI_TAG_CXX11
//        iterator
//        erase(const_iterator __first, const_iterator __last)
//        {
//            _M_erase_aux(__first, __last);
//            return __last._M_const_cast();
//        }
//#else
//        void
//      erase(iterator __first, iterator __last)
//      { _M_erase_aux(__first, __last); }
//
//      void
//      erase(const_iterator __first, const_iterator __last)
//      { _M_erase_aux(__first, __last); }
//#endif
//
//        void
//        clear() _GLIBCXX_NOEXCEPT
//        {
//            _M_erase(_M_begin());
//            _M_impl._M_reset();
//        }
//
//        // Set operations.
//        iterator
//        find(const key_type& __k);
//
//        const_iterator
//        find(const key_type& __k) const;
//
//        size_type
//        count(const key_type& __k) const;
//
//        iterator
//        lower_bound(const key_type& __k)
//        { return _M_lower_bound(_M_begin(), _M_end(), __k); }
//
//        const_iterator
//        lower_bound(const key_type& __k) const
//        { return _M_lower_bound(_M_begin(), _M_end(), __k); }
//
//        iterator
//        upper_bound(const key_type& __k)
//        { return _M_upper_bound(_M_begin(), _M_end(), __k); }
//
//        const_iterator
//        upper_bound(const key_type& __k) const
//        { return _M_upper_bound(_M_begin(), _M_end(), __k); }
//
//        pair<iterator, iterator>
//        equal_range(const key_type& __k);
//
//        pair<const_iterator, const_iterator>
//        equal_range(const key_type& __k) const;
//
//#if __cplusplus >= 201402L
//        template<typename _Kt,
//                typename _Req = __has_is_transparent_t<_Compare, _Kt>>
//        iterator
//        _M_find_tr(const _Kt& __k)
//        {
//            const _Rb_tree* __const_this = this;
//            return __const_this->_M_find_tr(__k)._M_const_cast();
//        }
//
//        template<typename _Kt,
//                typename _Req = __has_is_transparent_t<_Compare, _Kt>>
//        const_iterator
//        _M_find_tr(const _Kt& __k) const
//        {
//            auto __j = _M_lower_bound_tr(__k);
//            if (__j != end() && _M_impl._M_key_compare(__k, _S_key(__j._M_node)))
//                __j = end();
//            return __j;
//        }
//
//        template<typename _Kt,
//                typename _Req = __has_is_transparent_t<_Compare, _Kt>>
//        size_type
//        _M_count_tr(const _Kt& __k) const
//        {
//            auto __p = _M_equal_range_tr(__k);
//            return std::distance(__p.first, __p.second);
//        }
//
//        template<typename _Kt,
//                typename _Req = __has_is_transparent_t<_Compare, _Kt>>
//        iterator
//        _M_lower_bound_tr(const _Kt& __k)
//        {
//            const _Rb_tree* __const_this = this;
//            return __const_this->_M_lower_bound_tr(__k)._M_const_cast();
//        }
//
//        template<typename _Kt,
//                typename _Req = __has_is_transparent_t<_Compare, _Kt>>
//        const_iterator
//        _M_lower_bound_tr(const _Kt& __k) const
//        {
//            auto __x = _M_begin();
//            auto __y = _M_end();
//            while (__x != 0)
//                if (!_M_impl._M_key_compare(_S_key(__x), __k))
//                {
//                    __y = __x;
//                    __x = _S_left(__x);
//                }
//                else
//                    __x = _S_right(__x);
//            return const_iterator(__y);
//        }
//
//        template<typename _Kt,
//                typename _Req = __has_is_transparent_t<_Compare, _Kt>>
//        iterator
//        _M_upper_bound_tr(const _Kt& __k)
//        {
//            const _Rb_tree* __const_this = this;
//            return __const_this->_M_upper_bound_tr(__k)._M_const_cast();
//        }
//
//        template<typename _Kt,
//                typename _Req = __has_is_transparent_t<_Compare, _Kt>>
//        const_iterator
//        _M_upper_bound_tr(const _Kt& __k) const
//        {
//            auto __x = _M_begin();
//            auto __y = _M_end();
//            while (__x != 0)
//                if (_M_impl._M_key_compare(__k, _S_key(__x)))
//                {
//                    __y = __x;
//                    __x = _S_left(__x);
//                }
//                else
//                    __x = _S_right(__x);
//            return const_iterator(__y);
//        }
//
//        template<typename _Kt,
//                typename _Req = __has_is_transparent_t<_Compare, _Kt>>
//        pair<iterator, iterator>
//        _M_equal_range_tr(const _Kt& __k)
//        {
//            const _Rb_tree* __const_this = this;
//            auto __ret = __const_this->_M_equal_range_tr(__k);
//            return { __ret.first._M_const_cast(), __ret.second._M_const_cast() };
//        }
//
//        template<typename _Kt,
//                typename _Req = __has_is_transparent_t<_Compare, _Kt>>
//        pair<const_iterator, const_iterator>
//        _M_equal_range_tr(const _Kt& __k) const
//        {
//            auto __low = _M_lower_bound_tr(__k);
//            auto __high = __low;
//            auto& __cmp = _M_impl._M_key_compare;
//            while (__high != end() && !__cmp(__k, _S_key(__high._M_node)))
//                ++__high;
//            return { __low, __high };
//        }
//#endif
//
//        // Debugging.
//        bool
//        __rb_verify() const;
//
//#if __cplusplus >= 201103L
//        _Rb_tree&
//        operator=(_Rb_tree&&)
//        noexcept(_Alloc_traits::_S_nothrow_move()
//                 && is_nothrow_move_assignable<_Compare>::value);
//
//        template<typename _Iterator>
//        void
//        _M_assign_unique(_Iterator, _Iterator);
//
//        template<typename _Iterator>
//        void
//        _M_assign_equal(_Iterator, _Iterator);
//
//    private:
//        // Move elements from container with equal allocator.
//        void
//        _M_move_data(_Rb_tree& __x, true_type)
//        { _M_impl._M_move_data(__x._M_impl); }
//
//        // Move elements from container with possibly non-equal allocator,
//        // which might result in a copy not a move.
//        void
//        _M_move_data(_Rb_tree&, false_type);
//
//        // Move assignment from container with equal allocator.
//        void
//        _M_move_assign(_Rb_tree&, true_type);
//
//        // Move assignment from container with possibly non-equal allocator,
//        // which might result in a copy not a move.
//        void
//        _M_move_assign(_Rb_tree&, false_type);
//#endif
//
//#if __cplusplus > 201402L
//    public:
//        /// Re-insert an extracted node.
//        insert_return_type
//        _M_reinsert_node_unique(node_type&& __nh)
//        {
//            insert_return_type __ret;
//            if (__nh.empty())
//                __ret.position = end();
//            else
//            {
//                __glibcxx_assert(_M_get_Node_allocator() == *__nh._M_alloc);
//
//                auto __res = _M_get_insert_unique_pos(__nh._M_key());
//                if (__res.second)
//                {
//                    __ret.position
//                            = _M_insert_node(__res.first, __res.second, __nh._M_ptr);
//                    __nh._M_ptr = nullptr;
//                    __ret.inserted = true;
//                }
//                else
//                {
//                    __ret.node = std::move(__nh);
//                    __ret.position = iterator(__res.first);
//                    __ret.inserted = false;
//                }
//            }
//            return __ret;
//        }
//
//        /// Re-insert an extracted node.
//        iterator
//        _M_reinsert_node_equal(node_type&& __nh)
//        {
//            iterator __ret;
//            if (__nh.empty())
//                __ret = end();
//            else
//            {
//                __glibcxx_assert(_M_get_Node_allocator() == *__nh._M_alloc);
//                auto __res = _M_get_insert_equal_pos(__nh._M_key());
//                if (__res.second)
//                    __ret = _M_insert_node(__res.first, __res.second, __nh._M_ptr);
//                else
//                    __ret = _M_insert_equal_lower_node(__nh._M_ptr);
//                __nh._M_ptr = nullptr;
//            }
//            return __ret;
//        }
//
//        /// Re-insert an extracted node.
//        iterator
//        _M_reinsert_node_hint_unique(const_iterator __hint, node_type&& __nh)
//        {
//            iterator __ret;
//            if (__nh.empty())
//                __ret = end();
//            else
//            {
//                __glibcxx_assert(_M_get_Node_allocator() == *__nh._M_alloc);
//                auto __res = _M_get_insert_hint_unique_pos(__hint, __nh._M_key());
//                if (__res.second)
//                {
//                    __ret = _M_insert_node(__res.first, __res.second, __nh._M_ptr);
//                    __nh._M_ptr = nullptr;
//                }
//                else
//                    __ret = iterator(__res.first);
//            }
//            return __ret;
//        }
//
//        /// Re-insert an extracted node.
//        iterator
//        _M_reinsert_node_hint_equal(const_iterator __hint, node_type&& __nh)
//        {
//            iterator __ret;
//            if (__nh.empty())
//                __ret = end();
//            else
//            {
//                __glibcxx_assert(_M_get_Node_allocator() == *__nh._M_alloc);
//                auto __res = _M_get_insert_hint_equal_pos(__hint, __nh._M_key());
//                if (__res.second)
//                    __ret = _M_insert_node(__res.first, __res.second, __nh._M_ptr);
//                else
//                    __ret = _M_insert_equal_lower_node(__nh._M_ptr);
//                __nh._M_ptr = nullptr;
//            }
//            return __ret;
//        }
//
//        /// Extract a node.
//        node_type
//        extract(const_iterator __pos)
//        {
//            auto __ptr = _Rb_tree_rebalance_for_erase(
//                    __pos._M_const_cast()._M_node, _M_impl._M_header);
//            --_M_impl._M_node_count;
//            return { static_cast<_Link_type>(__ptr), _M_get_Node_allocator() };
//        }
//
//        /// Extract a node.
//        node_type
//        extract(const key_type& __k)
//        {
//            node_type __nh;
//            auto __pos = find(__k);
//            if (__pos != end())
//                __nh = extract(const_iterator(__pos));
//            return __nh;
//        }
//
//        template<typename _Compare2>
//        using _Compatible_tree
//                = _Rb_tree<_Key, _Val, _KeyOfValue, _Compare2, _Alloc>;
//
//        template<typename, typename>
//        friend class _Rb_tree_merge_helper;
//
//        /// Merge from a compatible container into one with unique keys.
//        template<typename _Compare2>
//        void
//        _M_merge_unique(_Compatible_tree<_Compare2>& __src) noexcept
//        {
//            using _Merge_helper = _Rb_tree_merge_helper<_Rb_tree, _Compare2>;
//            for (auto __i = __src.begin(), __end = __src.end(); __i != __end;)
//            {
//                auto __pos = __i++;
//                auto __res = _M_get_insert_unique_pos(_KeyOfValue()(*__pos));
//                if (__res.second)
//                {
//                    auto& __src_impl = _Merge_helper::_S_get_impl(__src);
//                    auto __ptr = _Rb_tree_rebalance_for_erase(
//                            __pos._M_node, __src_impl._M_header);
//                    --__src_impl._M_node_count;
//                    _M_insert_node(__res.first, __res.second,
//                                   static_cast<_Link_type>(__ptr));
//                }
//            }
//        }
//
//        /// Merge from a compatible container into one with equivalent keys.
//        template<typename _Compare2>
//        void
//        _M_merge_equal(_Compatible_tree<_Compare2>& __src) noexcept
//        {
//            using _Merge_helper = _Rb_tree_merge_helper<_Rb_tree, _Compare2>;
//            for (auto __i = __src.begin(), __end = __src.end(); __i != __end;)
//            {
//                auto __pos = __i++;
//                auto __res = _M_get_insert_equal_pos(_KeyOfValue()(*__pos));
//                if (__res.second)
//                {
//                    auto& __src_impl = _Merge_helper::_S_get_impl(__src);
//                    auto __ptr = _Rb_tree_rebalance_for_erase(
//                            __pos._M_node, __src_impl._M_header);
//                    --__src_impl._M_node_count;
//                    _M_insert_node(__res.first, __res.second,
//                                   static_cast<_Link_type>(__ptr));
//                }
//            }
//        }
//#endif // C++17
//
//        friend bool
//        operator==(const _Rb_tree& __x, const _Rb_tree& __y)
//        {
//            return __x.size() == __y.size()
//                   && std::equal(__x.begin(), __x.end(), __y.begin());
//        }
//
//#if __cpp_lib_three_way_comparison
//        friend auto
//        operator<=>(const _Rb_tree& __x, const _Rb_tree& __y)
//        {
//            if constexpr (requires { typename __detail::__synth3way_t<_Val>; })
//                return std::lexicographical_compare_three_way(__x.begin(), __x.end(),
//                                                              __y.begin(), __y.end(),
//                                                              __detail::__synth3way);
//        }
//#else
//        friend bool
//      operator<(const _Rb_tree& __x, const _Rb_tree& __y)
//      {
//	return std::lexicographical_compare(__x.begin(), __x.end(),
//					    __y.begin(), __y.end());
//      }
//#endif
//
//    private:
//#if __cplusplus >= 201103L
//        // An RAII _Node handle
//        struct _Auto_node
//        {
//            template<typename... _Args>
//            _Auto_node(_Rb_tree& __t, _Args&&... __args)
//                    : _M_t(__t),
//                      _M_node(__t._M_create_node(std::forward<_Args>(__args)...))
//            { }
//
//            ~_Auto_node()
//            {
//                if (_M_node)
//                    _M_t._M_drop_node(_M_node);
//            }
//
//            _Auto_node(_Auto_node&& __n)
//                    : _M_t(__n._M_t), _M_node(__n._M_node)
//            { __n._M_node = nullptr; }
//
//            const _Key&
//            _M_key() const
//            { return _S_key(_M_node); }
//
//            iterator
//            _M_insert(pair<_Base_ptr, _Base_ptr> __p)
//            {
//                auto __it = _M_t._M_insert_node(__p.first, __p.second, _M_node);
//                _M_node = nullptr;
//                return __it;
//            }
//
//            iterator
//            _M_insert_equal_lower()
//            {
//                auto __it = _M_t._M_insert_equal_lower_node(_M_node);
//                _M_node = nullptr;
//                return __it;
//            }
//
//            _Rb_tree& _M_t;
//            _Link_type _M_node;
//        };
//#endif // C++11
//    };

}
#endif //BLIB_TREE_H
