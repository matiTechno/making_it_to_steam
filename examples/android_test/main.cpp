#include <iostream>
#include <exception>
#include <MITS/app.hpp>
#include "android_scene.hpp"

int main()
{
    try
    {
        App app(800, 600, "android scene");
        app.start<Android_scene>();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
