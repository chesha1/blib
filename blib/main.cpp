#include <iostream>
#include "config.h"
#include "allocator.h"
#include <new>

int main() {
    blib::new_allocator<int> newAllocator;
    auto i = newAllocator.allocate(1000);
    return 0;
}