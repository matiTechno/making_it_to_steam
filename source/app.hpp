#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <vector>
#include "scene.hpp"
struct SDL_Window;
typedef void *SDL_GLContext;
#include "res_class.hpp"
#include <assert.h>

// define USE_IMGUI to support ImGui
// all you need to do in your scene class
// is to create imgui window and call ImGui::Render()
// also include:
// imgui/imgui.h
// imgui/imgui_impl_sdl_gl3.h
// from source dir or 'common_scene.hpp'

// NOTE: not all functionality is tested

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

class App
{
public:
    friend class Scene;

    App();
    App(const App&) = delete;
    ~App();

    template<typename T, typename ...Args>
    void start(Args... args)
    {
        // if should_close = true no more scenes can be added
        // to reuse this function scenes must be empty (last scene must pop itself and don't
        // change should_close to true
        assert(!should_close);
        scenes.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        run();
    }

    static bool should_close;

private:
    std::unique_ptr<Wrp_sdl_lib> sdl_lib;
    std::unique_ptr<Wrp_sdl_win> sdl_win;
    std::unique_ptr<Wrp_sdl_context> sdl_context;
    static SDL_Window* sdl_win_handle;
    static std::unique_ptr<Sound_system> sound_system;
    static std::unique_ptr<Renderer_2D> renderer;
    static std::unique_ptr<Font_loader> font_loader;
    static std::unique_ptr<Postprocessor> pp_unit;
    std::vector<std::unique_ptr<Scene>> scenes;
    std::unique_ptr<Wrp_ImGui> wrp_imgui;

    void run();
    void processInput();
    void update(float dt);
    void render();
    void set_opengl_states();
    void manage_scenes();

    static bool isCurrent;
};

#endif // APP_HPP
