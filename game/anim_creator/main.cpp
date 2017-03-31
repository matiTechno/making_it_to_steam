#include <iostream>
#include <exception>
#include <MITS/app.hpp>
#include "anim_creator.hpp"

int main()
{
    try
    {
        App app(800, 600, "anim_creator", SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
        app.start<Anim_creator>();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
