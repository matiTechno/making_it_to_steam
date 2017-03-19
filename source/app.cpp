#include "app.hpp"
#include <assert.h>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <string>
#include <deque>
#include "common_scene.hpp"
#include <SDL2/SDL_mixer.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl_gl3.h"
#include <queue>

bool App::isCurrent  = false;
bool App::should_close = false;
const App* App::handle;

App::~App() = default;

App::App()
{
    assert(!isCurrent);
    isCurrent = true;

    handle = this;

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

        SDL_Window* temp = SDL_CreateWindow("making_it_to_steam", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            800, 600, SDL_WINDOW_OPENGL /*| SDL_WINDOW_RESIZABLE*/);
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

    // sound
    sound_system = std::make_unique<Sound_system>(MIX_INIT_OGG);

    // gl states
    set_opengl_states();

    // gl units
    renderer = std::make_unique<Renderer_2D>();
    font_loader = std::make_unique<Font_loader>();
    pp_unit = std::make_unique<Postprocessor>(fb_width, fb_height);

    wrp_imgui = std::make_unique<Wrp_ImGui>();
    ImGui_ImplSdlGL3_Init(sdl_win_handle);
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
    std::queue<SDL_Event> events;
    bool was_quit_event = false;
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT && !was_quit_event)
        {
            was_quit_event = true;
            scenes.back()->on_quit_event();
        }
        else
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
            events.push(std::move(event));
        }
    }
    ImGui_ImplSdlGL3_NewFrame(sdl_win_handle);
    scenes.back()->processInput(events);
}

void App::update(float dt)
{
    for(auto& scene: scenes)
    {
        if(&scene == &scenes.back() || scene->SCENE_update_when_not_top)
            scene->update(dt);
    }
}

void App::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_GetDrawableSize(sdl_win_handle, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);
    pp_unit->set_new_size(fb_width, fb_height);

    std::deque<Scene*> scenes_to_render;
    for(auto it = scenes.rbegin(); it != scenes.rend(); ++it)
    {
        scenes_to_render.push_front(it->get());
        if((*it)->SCENE_is_opaque)
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
    GLint s, d;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &s);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &d);
    src_alpha = static_cast<GLenum>(s);
    dst_alpha = static_cast<GLenum>(d);
    SDL_GL_GetDrawableSize(sdl_win_handle, &fb_width, &fb_height);
}

void App::manage_scenes()
{
    std::unique_ptr<Scene> new_scene = std::move(scenes.back()->new_scene);

    int num = scenes.back()->SCENE_num_scenes_to_pop;
    if(num)
    {
        assert(num > 0 && num <= static_cast<int>(scenes.size()));
        for(int i = 0; i < num; ++i)
            scenes.pop_back();
    }

    if(new_scene)
        scenes.push_back(std::move(new_scene));

    // :)
    for(auto& scene: scenes)
    {
        if(&scene == &scenes.back())
            scene->is_top = true;
        else
            scene->is_top = false;
    }
}

void App::set_blend_func(GLenum s, GLenum d) const
{
    if(src_alpha != s || dst_alpha != d)
    {
        src_alpha = s;
        dst_alpha = d;
        glBlendFunc(src_alpha, dst_alpha);
    }
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

Wrp_ImGui::Wrp_ImGui():
    Res_class([](void*){ImGui_ImplSdlGL3_Shutdown();})
{}
