#include <iostream>
#include <exception>
#include "app.hpp"

int main()
{
    try
    {
        App app;
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
