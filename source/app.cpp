#include "app.hpp"
#include <assert.h>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <string>
#include <deque>
#include "glad/glad.h"
#include "common_scene.hpp"
#include <SDL2/SDL_mixer.h>
#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl_gl3.h"
#endif

bool App::isCurrent  = false;
bool App::should_close = false;
SDL_Window* App::sdl_win_handle;
std::unique_ptr<Sound_system> App::sound_system;
std::unique_ptr<Renderer_2D> App::renderer;
std::unique_ptr<Font_loader> App::font_loader;
std::unique_ptr<Postprocessor> App::pp_unit;

App::~App()
{
    pp_unit.reset();
    font_loader.reset();
    renderer.reset();
    sound_system.reset();
}

App::App()
{
    assert(!isCurrent);
    isCurrent = true;

    SDL_version ver_compiled, ver_linked;
    SDL_VERSION(&ver_compiled);
    SDL_GetVersion(&ver_linked);

    std::cout << "compiled against SDL " << int(ver_compiled.major) << '.' <<
                 int(ver_compiled.minor) << '.' << int(ver_compiled.patch) << std::endl;
    std::cout << "linked against SDL " << int(ver_linked.major) << '.' <<
                 int(ver_linked.minor) << '.' << int(ver_linked.patch) << std::endl;

    // SDL2 init
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        throw std::runtime_error(std::string("SDL2 init failed: ") + SDL_GetError());

    sdl_lib = std::make_unique<Wrp_sdl_lib>();

    // SDL2 window
    {
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        SDL_Window* temp = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            800, 600, SDL_WINDOW_OPENGL);
        if(temp == nullptr)
            throw std::runtime_error(std::string("SDL2 window creation failed: ") + SDL_GetError());

        sdl_win = std::make_unique<Wrp_sdl_win>(temp);
        sdl_win_handle = temp;
    }

    // SDL2 context
    {
        SDL_GLContext temp = SDL_GL_CreateContext(sdl_win_handle);
        if(temp == nullptr)
            throw std::runtime_error(std::string("SDL2 context creation failed: ") + SDL_GetError());

        sdl_context = std::make_unique<Wrp_sdl_context>(temp);
    }

    // GLAD init
    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
        throw std::runtime_error("gladLoadGLLoader failed");

    std::cout << "vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    if(!GLAD_GL_ARB_texture_storage)
        throw std::runtime_error("ARB_texture_storage not available");

    // v_sync
    SDL_GL_SetSwapInterval(1);

    // rest...
    sound_system = std::make_unique<Sound_system>(MIX_INIT_OGG);

    renderer = std::make_unique<Renderer_2D>();
    font_loader = std::make_unique<Font_loader>();

    {
        int width, height;
        SDL_GL_GetDrawableSize(sdl_win_handle, &width, &height);
        pp_unit = std::make_unique<Postprocessor>(width, height);
    }

    set_opengl_states();

#ifdef USE_IMGUI
    wrp_imgui = std::make_unique<Wrp_ImGui>();
    ImGui_ImplSdlGL3_Init(sdl_win_handle);
#endif

}

void App::run()
{
    auto current_time = std::chrono::high_resolution_clock::now();

    while(!should_close && scenes.size())
    {
        processInput();

        auto new_time = std::chrono::high_resolution_clock::now();
        auto frame_time = std::chrono::duration<float>(new_time - current_time).count();
        current_time = new_time;

        update(frame_time);

        render();

        manage_scenes();
    }
}

void App::processInput()
{
    SDL_PumpEvents();
    SDL_Event quit_event;
    if(SDL_PeepEvents(&quit_event, 1, SDL_PEEKEVENT, SDL_QUIT, SDL_QUIT))
        scenes.back()->on_quit_event();

    scenes.back()->processInput();

#ifdef USE_IMGUI
    ImGui_ImplSdlGL3_NewFrame(sdl_win_handle);
#endif
}

void App::update(float dt)
{
    scenes.back()->update(dt);
}

void App::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    int width, height;
    SDL_GL_GetDrawableSize(sdl_win_handle, &width, &height);
    glViewport(0, 0, width, height);
    pp_unit->set_new_size(width, height);

    std::deque<Scene*> scenes_to_render;
    for(auto& scene: scenes)
    {
        scenes_to_render.push_front(scene.get());
        if(scene->is_opaque)
            break;
    }
    for(auto& scene: scenes_to_render)
    {
        scene->render();
        if(!pp_unit->has_finished())
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    SDL_GL_SwapWindow(sdl_win_handle);
}

void App::set_opengl_states()
{
    glEnable(GL_BLEND);
}

void App::manage_scenes()
{
    std::unique_ptr<Scene> new_scene = std::move(scenes.back()->new_scene);

    int num = scenes.back()->num_scenes_to_pop;
    if(num)
    {
        assert(num > 0 && num <= static_cast<int>(scenes.size()));
        for(int i = 0; i < num; ++i)
            scenes.pop_back();
    }

    if(new_scene)
        scenes.push_back(std::move(new_scene));
}

Wrp_sdl_lib::Wrp_sdl_lib():
    Res_class([](void*){SDL_Quit();})
{}

Wrp_sdl_win::Wrp_sdl_win(SDL_Window* id):
    Res_class([](SDL_Window* id){SDL_DestroyWindow(id);})
{
this->id = id;
}

Wrp_sdl_context::Wrp_sdl_context(SDL_GLContext id):
    Res_class([](SDL_GLContext id){SDL_GL_DeleteContext(id);})
{
this->id = id;
}

#ifdef USE_IMGUI
Wrp_ImGui::Wrp_ImGui():
    Res_class([](void*){ImGui_ImplSdlGL3_Shutdown();})
{}
#endif
