#include <iostream>
#include <exception>
#include "app.hpp"
#include "test_scene/test_scene.hpp"

int main()
{
    try
    {
        App app;
        app.start<Test_scene>();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
