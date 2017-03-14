#include <iostream>
#include <exception>
#include "app.hpp"
#include "game1/game1.h"

int main()
{
    try
    {
        App app;
        app.start<Game1>();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
