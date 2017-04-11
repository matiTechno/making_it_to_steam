#include <iostream>
#include <exception>
#include <MITS/app.hpp>
#include "particle_editor.hpp"

int main()
{
    try
    {
        App app(800, 600, "particle editor", SDL_WINDOW_RESIZABLE);
        app.start<Particle_editor>();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
