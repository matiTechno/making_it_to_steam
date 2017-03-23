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
#include <glm/vec2.hpp>

struct Scene_coords
{
    glm::ivec2 pos;
    glm::ivec2 size;
};

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

// by default scene is positioned to fit window

// changing coords will affect rendering only if done
// before render() function
// update_coords() is called for all scenes that will
// be renderer in the current frame
// if you want to update your coords only if scene is top most
// do it in update()

// NOTE: MEMBER REFERENCE VARS
// public functions are executed in the same order
// as they appear in Scene definition
// *** dt member is updated just before update()
// *** use renderer and pp_unit only in render()
// *** you can use font_loader and sound_system anytime
// ... misusing any of these won't affect other scenes
// reason for this design is to avoid dependency injection
// (I don't like it for this class, want to keep it lightweight)

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
    virtual void processInput(const std::vector<SDL_Event>& events);

    virtual void update();

    virtual void update_coords();

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
    // load_projection(glm::vec4(0.f, 0.f, coords.size.x, coords.size.y))
    // (0.f, 0.f) - camera position
    // (coords.size.x, coords.size.y) - view range
    Scene_coords coords;

    template<typename T, typename ...Args>
    void set_new_scene(Args&&... args)
    {
        new_scene = std::make_unique<T>(std::forward<Args>(args)...);
    }

    // not useful for now
    //SDL_Window* const sdl_win_handle;

    const float& dt;
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
