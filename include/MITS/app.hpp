#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <vector>
#include "scene.hpp"
struct SDL_Window;
typedef void *SDL_GLContext;
#include <assert.h>
#include <glm/vec2.hpp>
class Wrp_sdl_lib;
class Wrp_sdl_win;
class Wrp_sdl_context;
class Wrp_ImGui;
#include <SDL2/SDL_video.h>

class App
{
public:
    // SDL_WINDOW_OPENGL flag is already set
    App(int w, int h, const char* title, unsigned int win_flags_sdl = 0,
        int x = SDL_WINDOWPOS_CENTERED, int y = SDL_WINDOWPOS_CENTERED);
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
    static glm::ivec2 get_fb_size()
    {return fbSize;}
    static std::size_t get_num_scenes()
    {return handle->scenes.size();}

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
    void update();
    void prepare_scene_to_render(Scene& scene);
    void render();
    void set_opengl_states();
    void manage_scenes();

    static bool isCurrent;
    // resource class pattern again
    friend class Scene;
    static const App* handle;

    static glm::ivec2 fbSize;
    float frametime;
    bool ImGui_wants_input;
};

#endif // APP_HPP
