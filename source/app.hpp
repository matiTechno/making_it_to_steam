#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <vector>
#include "scene.hpp"
struct SDL_Window;
typedef void *SDL_GLContext;
#include "res_class.hpp"
#include <assert.h>
#include "glad/glad.h"

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
    App();
    App(const App&) = delete;
    ~App();

    template<typename T, typename ...Args>
    void start(Args&&... args)
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
    // don't change the order
    std::unique_ptr<Wrp_sdl_lib> sdl_lib;
    std::unique_ptr<Wrp_sdl_win> sdl_win;
    std::unique_ptr<Wrp_sdl_context> sdl_context;
    SDL_Window* sdl_win_handle;
    std::unique_ptr<Sound_system> sound_system;
    // opengl && SDL_Mixer resources
    std::unique_ptr<Renderer_2D> renderer;
    std::unique_ptr<Font_loader> font_loader;
    std::unique_ptr<Postprocessor> pp_unit;
    std::vector<std::unique_ptr<Scene>> scenes;
    std::unique_ptr<Wrp_ImGui> wrp_imgui;

    void run();
    void processInput();
    void update(float dt);
    void render();
    void set_opengl_states();
    void manage_scenes();

    static bool isCurrent;
    // resource class pattern again
    // I need to patent this :D
    friend class Scene;
    friend class Renderer_2D;
    static const App* handle;

    mutable GLenum src_alpha, dst_alpha;
    int fb_width, fb_height;

    void set_blend_func(GLenum s, GLenum d) const;
};

#endif // APP_HPP
