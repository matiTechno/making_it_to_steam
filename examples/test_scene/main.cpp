#include <iostream>
#include <exception>
#include <MITS/app.hpp>
#include "test_scene.hpp"

int main()
{
    try
    {
        App app(800, 600, "test_scene", SDL_WINDOW_RESIZABLE);
        app.start<Test_scene>();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
