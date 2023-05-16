
#include <iostream>
#include "config.h"
#include "type_traits_new.h"


struct myClass{
    operator int()
    {
        std::cout<<"inversion";
        return 1;
    }
};


int main() {
    myClass obj;
    int a = int(obj);

}

