cmake_minimum_required(VERSION 3.25)
project(blib)

set(CMAKE_CXX_STANDARD 23)

include_directories(blib)
include_directories(blib/thread_pool)

file(GLOB SRC blib/*.cpp)
file(GLOB HEADERS blib/*.h)

add_executable(blib ${SRC} ${HEADERS}
        blib/stl_tree_comment.h)

#add_executable(blib
#        blib/main.cpp blib/config.h blib/allocator.h blib/type_traits.h blib/iterator_concepts.h blib/iterator_legacy.h blib/type_traits_new.h)
