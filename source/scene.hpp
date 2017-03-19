#ifndef SCENE_HPP
#define SCENE_HPP

#include <memory>
#include <queue>
class Sound_system;
class Renderer_2D;
class Font_loader;
class Postprocessor;
union SDL_Event;
struct SDL_Window;

// num_scenes_to_pop && new_scene
// requests are executed at the end of frame
// only for top scene
// by default Scene is opaque

// Sample / Music / Texture / Font and possibly other
// objects must be local to your scenes (non-static)

// to use ImGui:
// * create ImGui windows
// * call ImGui::Render()
// ...
// input processing is already done in App class
// you shouldn't use ImGui when your scene is not on top
// (check it with is_on_top())

class Scene
{
public:
    friend class App;

    Scene();
    // no copy no move operations for safety
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    virtual ~Scene() = default;

    // by default calls:
    // ...
    // * beg_events()
    // * processEvent(SDL_Event& event)
    // * end_events()
    // ...
    // how to std::queue:
    // while(size())
    // front()
    // pop()
    virtual void processInput(std::queue<SDL_Event>& events);

    virtual void update(float dt);

    virtual void render();

    virtual void on_quit_event();

protected:
    // including this scene
    // must be in range [0, num_scenes] or assertion will fail
    int SCENE_num_scenes_to_pop = 0;
    bool SCENE_is_opaque = true;
    bool SCENE_update_when_not_top = false;

    template<typename T, typename ...Args>
    void set_new_scene(Args&&... args)
    {
        new_scene = std::make_unique<T>(std::forward<Args>(args)...);
    }

    // use with care
    SDL_Window* const sdl_win_handle;

    const Sound_system& sound_system;
    const Renderer_2D& renderer;
    const Font_loader& font_loader;
    const Postprocessor& pp_unit;

    bool is_on_top() const
    {return is_top;}

private:
    std::unique_ptr<Scene> new_scene;
    bool is_top;

    virtual void beg_events();
    virtual void processEvent(SDL_Event& event);
    virtual void end_events();
};

#endif // SCENE_HPP
