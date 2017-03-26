#include <iostream>
#include <exception>
#include "app.hpp"
#include "test_scene/test_scene.hpp"
#include "../game1/tools/anim_creator.hpp"

int main()
{
    try
    {
        App app;
        //app.start<Test_scene>();
        app.start<Anim_creator>();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
