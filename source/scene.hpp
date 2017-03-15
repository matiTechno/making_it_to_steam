#ifndef SCENE_HPP
#define SCENE_HPP

#include <memory>
class Sound_system;
class Renderer_2D;
class Font_loader;
class Postprocessor;
union SDL_Event;
struct SDL_Window;

// num_scenes_to_pop && new_scene
// requests are executed at the end of frame
// by default Scene is opaque

// Sample / Music / Texture / Font and possibly other
// objects must be local to your scenes (non-static)

class Scene
{
public:
    friend class App;

    Scene();
    // no copy no move operations for safety
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    virtual ~Scene() = default;

    void processInput();

    virtual void update(float dt);

    virtual void render();

    virtual void on_quit_event();

protected:
    // including this scene
    // must be in range [0, num_scenes] or assertion will fail
    int num_scenes_to_pop = 0;

    bool is_opaque = true;

    template<typename T, typename ...Args>
    void set_new_scene(Args... args)
    {
        new_scene = std::make_unique<T>(std::forward<Args>(args)...);
    }

    // use with care
    SDL_Window* const sdl_win_handle;

    const Sound_system& sound_system;
    const Renderer_2D& renderer;
    const Font_loader& font_loader;
    const Postprocessor& pp_unit;

private:
    std::unique_ptr<Scene> new_scene;

    // SDL_QUIT events are never passed to this function
    virtual void process_event(SDL_Event& event);
    virtual void beg_processInput();
    virtual void end_processInput();
};

#endif // SCENE_HPP
