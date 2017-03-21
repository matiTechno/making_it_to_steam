#ifndef SCENE_HPP
#define SCENE_HPP

#include <memory>
#include <vector>
class Sound_system;
class Renderer_2D;
class Font_loader;
class Postprocessor;
union SDL_Event;
struct SDL_Window;
#include <glm/vec4.hpp>
// position + size
typedef glm::ivec4 Scene_coords;

// num_scenes_to_pop && new_scene
// requests are executed at the end of frame
// only for top scene
// if num_scenes_to_pop = 1 current scene will be popped
// num_scenes_to_pop must be in range [0, num_scenes] or assertion will fail
// by default Scene is opaque

// Sample / Music / Texture / Font and possibly other
// objects must be local to your scenes (non-static)

// to use ImGui:
// in function render_ImGui:
// * create ImGui windows
// !!! don't call ImGui::Render()
// ...
// render_ImGui is called only for top most scene or if
// render_ImGui_when_not_top = true
// input is captured by all ImGui windows
// by default scene is positioned to fit fb size

class Full_window
{};

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
    // how to iterate (right event order)
    // for(auto& event: events)
    // processEvent(event);
    // ...
    //
    virtual void processInput(const std::vector<SDL_Event>& events);

    virtual void update(float dt);

    virtual void render();

    virtual void render_ImGui();

    virtual void on_quit_event();

protected:
    int num_scenes_to_pop = 0;
    bool is_opaque = true;
    // these are some hacks
    // will be replaced with recursive scenes
    bool update_when_not_top = false;
    bool processInput_when_not_top = false;
    bool render_ImGui_when_not_top = false;

    // easy projection set up to use renderer
    // load_projection(glm::vec4(0.f, 0.f, coords.z, coords.w)
    // (0.f, 0.f) - camera position
    // (coords.z, coords.w) - view range
    Scene_coords coords;

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
    virtual void processEvent(const SDL_Event& event);
    virtual void end_events();
};

#endif // SCENE_HPP
