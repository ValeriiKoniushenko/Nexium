#include "Core/StringHelper.h"

#include <iostream>

int main()
{
    Core::StringAtom h = "Hello world123";
    std::cout << "Hello world"_atom << std::endl;
    return 0;
}