#ifndef APP_HPP
#define APP_HPP

#include "glad/glad.h"
#include <memory>
#include <vector>
#include "scene.h"
struct SDL_Window;
typedef void *SDL_GLContext;
#include "res_class.h"

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
