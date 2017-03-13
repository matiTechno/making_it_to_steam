#include "scene.h"
#include <SDL2/SDL.h>

Scene::Scene(const Systems& systems, bool is_opaque):
    systems(systems),
    is_opaque(is_opaque),
    renderer_2D(systems.renderer_2D)
{}

void Scene::processInput()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
            continue;
        process_event(event);
    }
}

void Scene::update(float dt)
{(void)dt;}

void Scene::render()
{}

void Scene::process_event(SDL_Event& event)
{(void)event;}
