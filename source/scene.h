#ifndef SCENE_H
#define SCENE_H

#include <memory>
class Sound_system;
class Renderer_2D;
class Font_loader;
class Postprocessor;
union SDL_Event;
struct SDL_Window;

// declarations for common resources
class Font;
class Texture;
class Music;
class Sample;

struct Systems
{
    const Sound_system* sound_system;
    const Renderer_2D* renderer_2D;
    const Font_loader* font_loader;
    const Postprocessor* pp_unit;
    // use with care
    SDL_Window* window;
};

// num_scenes_to_pop && new_scene
// requests are executed at the end of frame
class Scene
{
public:
    friend class App;

    Scene(const Systems& systems, bool is_opaque);

    virtual ~Scene() = default;

    void processInput();

    virtual void update(float dt);

    virtual void render();

    virtual void on_quit_event() = 0;

protected:
    // including this scene
    // must be in range [0, num_scenes] or assertion will fail
    int num_scenes_to_pop = 0;
    const Systems systems;
    bool is_opaque;

    template<typename T>
    void set_new_scene(bool is_opaque)
    {
        new_scene = std::make_unique<T>(systems, is_opaque);
    }

    // shortcut
    const Renderer_2D* const renderer_2D;

private:
    std::unique_ptr<Scene> new_scene;

    // SDL_QUIT events are never passed to this function
    virtual void process_event(SDL_Event& event);
};

#endif // SCENE_H
