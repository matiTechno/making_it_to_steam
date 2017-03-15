#include "scene.hpp"
#include <SDL2/SDL.h>
#include "app.hpp"

Scene::Scene():
    sdl_win_handle(App::sdl_win_handle),
    sound_system(*App::sound_system),
    renderer(*App::renderer),
    font_loader(*App::font_loader),
    pp_unit(*App::pp_unit)
{}

void Scene::processInput()
{
    beg_processInput();

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
            continue;
        process_event(event);
    }

    end_processInput();
}

void Scene::update(float dt)
{(void)dt;}

void Scene::render()
{}

void Scene::process_event(SDL_Event& event)
{(void)event;}

void Scene::beg_processInput()
{}

void Scene::end_processInput()
{}

void Scene::on_quit_event()
{
    App::should_close = true;
}
