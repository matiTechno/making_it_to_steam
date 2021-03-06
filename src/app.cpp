#include <MITS/app.hpp>
#include <assert.h>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <string>
#include <deque>
#include <MITS/imgui/imgui.h>
#include "imgui/imgui_impl_sdl_gl3.h"
#include "opengl/other.hpp"
#include <SDL2/SDL.h>
#include <MITS/sound_system.hpp>
#include <MITS/rendering/renderer_2d.hpp>
#include <MITS/rendering/postprocessor.hpp>
#include <MITS/rendering/font_loader.hpp>
#include <SDL2/SDL_mixer.h>

// wrapper calsses for SDL2
class Wrp_sdl_lib: public Res_class<void*>
{
public:
    Wrp_sdl_lib();
};

class Wrp_sdl_win: public Res_class<SDL_Window*>
{
public:
    Wrp_sdl_win(SDL_Window* id);
};

class Wrp_sdl_context: public Res_class<SDL_GLContext>
{
public:
    Wrp_sdl_context(SDL_GLContext id);
};

// wrapper class for ImGui
class Wrp_ImGui: public Res_class<void*>
{
public:
    Wrp_ImGui();
};

bool App::isCurrent  = false;
bool App::should_close = false;
const App* App::handle;
glm::ivec2 App::fbSize;

App::~App() = default;

App::App(int w, int h, const char* title, unsigned int win_flags_sdl, int x, int y)
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
#if(__APPLE__)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

        SDL_Window* temp = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_OPENGL | win_flags_sdl);
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
    sound_system = std::make_unique<Sound_system>();

    // gl states
    set_opengl_states();

    // gl units
    SDL_GL_GetDrawableSize(sdl_win_handle, &fbSize.x, &fbSize.y);
    renderer = std::make_unique<Renderer_2D>();
    font_loader = std::make_unique<Font_loader>();
    pp_unit = std::make_unique<Postprocessor>(fbSize);

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
        frametime = std::chrono::duration<float>(new_time - current_time).count();
        current_time = new_time;

        update();

        render();

        manage_scenes();
    }
}

void App::processInput()
{
    std::vector<SDL_Event> events;
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
            events.push_back(std::move(event));
        }
    }
    ImGui_ImplSdlGL3_NewFrame(sdl_win_handle);

    SDL_GL_GetDrawableSize(sdl_win_handle, &fbSize.x, &fbSize.y);

    if(ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse)
    {
        ImGui_wants_input = true;
        return;
    }
    else
        ImGui_wants_input = false;

    for(auto& scene: scenes)
    {
        if(&scene == &scenes.back() || scene->processInput_when_not_top)
            scene->processInput(events);
    }
}

void App::update()
{
    for(auto& scene: scenes)
    {
        if(&scene == &scenes.back() || scene->update_when_not_top)
            scene->update();
    }
}

void App::render()
{
    // moved 'SDL_GL_GetDrawableSize(sdl_win_handle, &fbSize.x, &fbSize.y);'
    // to processInput() so new size is avaible in earlier Scene functions
    Scissor::set(0, 0, fbSize.x, fbSize.y);
    glClear(GL_COLOR_BUFFER_BIT);
    pp_unit->set_new_size(fbSize);

    std::deque<Scene*> scenes_to_render;
    for(auto it = scenes.rbegin(); it != scenes.rend(); ++it)
    {
        scenes_to_render.push_front(it->get());
        if((*it)->is_opaque)
            break;
    }
    for(auto& scene: scenes_to_render)
    {
        prepare_scene_to_render(*scene);
        scene->render();
        assert(pp_unit->has_finished());
    }
    for(auto& scene: scenes)
    {
        if(&scene == &scenes.back() || scene->render_ImGui_when_not_top)
            scene->render_ImGui();
    }
    ImGui::Render();

    SDL_GL_SwapWindow(sdl_win_handle);
}

void App::set_opengl_states()
{
    glEnable(GL_SCISSOR_TEST);
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

    // :)
    for(auto& scene: scenes)
    {
        if(&scene == &scenes.back())
            scene->is_top = true;
        else
            scene->is_top = false;
    }
}

void App::prepare_scene_to_render(Scene& scene)
{
    scene.update_coords();

    // glViewport:
    // GL_INVALID_VALUE is generated if either width or height is negative.
    if(scene.coords.pos.x < 0)
        scene.coords.pos.x = 0;
    if(fbSize.y - (scene.coords.pos.y + scene.coords.size.y) < 0)
        scene.coords.pos.y = fbSize.y - scene.coords.size.y;

    Viewport::set(scene.coords);
    pp_unit->set_new_scene(scene.coords);
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
