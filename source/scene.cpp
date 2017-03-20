#include "scene.hpp"
#include "app.hpp"
#include <SDL2/SDL_events.h>

Scene::Scene():
    coords(0, 0, App::get_fb_size()),
    sdl_win_handle(App::handle->sdl_win_handle),
    sound_system(*App::handle->sound_system),
    renderer(*App::handle->renderer),
    font_loader(*App::handle->font_loader),
    pp_unit(*App::handle->pp_unit),
    is_top(true)
{}

void Scene::processInput(const std::vector<SDL_Event>& events)
{
    beg_events();
    for(auto event: events)
        processEvent(event);
    end_events();
}

void Scene::beg_events()
{}

void Scene::end_events()
{}

void Scene::processEvent(const SDL_Event& event)
{(void)event;}

void Scene::update(float dt)
{(void)dt;}

void Scene::render()
{}

void Scene::on_quit_event()
{
    App::should_close = true;
}

void Scene::render_ImGui()
{}
