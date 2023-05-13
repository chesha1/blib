#include "type_traits.h"

namespace blib {

//    template<typename _Tp>
//    concept movable = is_object_v<_Tp> && ::move_constructible<_Tp>
//            && assignable_from<_Tp&, _Tp> && swappable<_Tp>;

    template<typename _Tp>
    concept movable = is_object_v<_Tp>;




}