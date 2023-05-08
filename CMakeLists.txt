cmake_minimum_required(VERSION 3.25)
project(blib)

set(CMAKE_CXX_STANDARD 20)

add_executable(blib
        blib/main.cpp blib/config.h blib/allocator.h blib/type_traits.h)
