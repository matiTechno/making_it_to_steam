#include "scene.hpp"
#include <SDL2/SDL.h>

Scene::Scene(const Systems& systems, bool is_opaque):
    systems(systems),
    is_opaque(is_opaque),
    renderer_2D(systems.renderer_2D)
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
