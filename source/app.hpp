#ifndef APP_HPP
#define APP_HPP

#include "glad/glad.h"
#include <memory>
#include <vector>
#include "scene.h"
struct SDL_Window;
typedef void *SDL_GLContext;
#include "res_class.h"
#include <assert.h>

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

    SDL_Window* get_id()
    {
        return id;
    }
};

class Wrp_sdl_context: public Res_class<SDL_GLContext>
{
public:
    Wrp_sdl_context(SDL_GLContext id);
};

// NOTE: do not change blend factors manually
class App
{
public:
    App();
    App(const App&) = delete;
    ~App();

    template<typename T>
    void start()
    {
        assert(!should_close); // if should_close = true no more scenes can be added
        // to reuse this function scenes must be empty (last scene must pop itself and don't
        // change should_close to true
        scenes.push_back(std::make_unique<T>(Systems{sound_system.get(), renderer_2D.get(),
                                                     font_loader.get(), pp_unit.get(),
                                                     sdl_win->get_id()}, false));
        run();
    }

    static bool should_close;

private:
    std::unique_ptr<Wrp_sdl_lib> sdl_lib;
    std::unique_ptr<Wrp_sdl_win> sdl_win;
    std::unique_ptr<Wrp_sdl_context> sdl_context;
    std::unique_ptr<Sound_system> sound_system;
    std::unique_ptr<Renderer_2D> renderer_2D;
    std::unique_ptr<Font_loader> font_loader;
    std::unique_ptr<Postprocessor> pp_unit;
    std::vector<std::unique_ptr<Scene>> scenes;

    void run();
    void processInput();
    void update(float dt);
    void render();
    void set_opengl_states();
    void manage_scenes();

    static bool isCurrent;
};

#endif // APP_HPP
