#ifndef ANIM_TOOL_HPP
#define ANIM_TOOL_HPP

#include "source/common_scene.hpp"
#include <list>
//#include "selection_rect.hpp"
#include "anim_frame.hpp"

// PROTOTYPE CODE

struct Frame
{
    std::size_t index;
    glm::ivec4 texCoords;
    float time;
    glm::vec2 origin;
};

class Anim_tool: public Scene
{
public:
    Anim_tool();

    void processEvent(const SDL_Event& event) override;

    void render() override;

    void render_ImGui() override;

private:
    Font font;
    glm::ivec2 margin{40, 40};
    std::unique_ptr<Texture> texture;
    Sprite sprite;
    std::vector<Sprite> grids;
    std::vector<char> input;
    std::string err_msg;
    const int grid_size = 15;
    glm::vec4 grid_white{0.7f, 0.7f, 0.7f, 0.8f};
    glm::vec4 grid_grey{0.5f, 0.5f, 0.5f, 0.8f};

    // ...
    //std::list<Frame> frames;
    //Frame* current_frame;
    //std::vector<Selection_rect> rects;

    // camera
    glm::vec4 camera;
    bool is_rb_pressed = false;
    float camera_scale = 1.f;
    const float zoom_factor = 1.2f;

    void set_sprite();
    void set_scene();
    bool is_in_tex(int x, int y);
    glm::vec2 get_cursor_camera_space(int x, int y);
    glm::ivec2 cursor_pos;
    glm::vec2 window_pos;
    glm::vec2 window_size;

    Anim_frame frame{glm::vec2(100.f, 100.f), glm::vec2(100.f, 100.f), 0};

    bool is_in_window(int x, int y)
    {
        if(x >= window_pos.x && x <= window_pos.x + window_size.x &&
               y >= window_pos.y && y <= window_pos.y + window_size.y)
            return true;
        return false;
    }
};




// works not just do this shit input
// rect -> rect -> rect -> rect -> rect_back
// jesli byl click lewym przyciskiem i nie trafil on w rect_back to rect_back jest nieaktywny
// sprawdzamy czy ten sam event trafil w poprzedniego i tak dalej az do konca
// jest trafil to ten rect idzie przed rect_back i w tej klatce ignoruje reszte eventow
//
//
//
//
//
#endif // ANIM_TOOL_HPP
