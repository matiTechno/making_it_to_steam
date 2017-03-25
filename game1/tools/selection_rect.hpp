#ifndef SELECTION_RECT_HPP
#define SELECTION_RECT_HPP

#include "../../source/common_scene.hpp"

// PROTOTYPE CODE

struct Box
{
    glm::vec2 pos;
    glm::vec2 size;
    bool is_selected = false;
};

class Selection_rect
{
public:
    Selection_rect(const glm::vec2& pos, const glm::vec2& size, std::size_t id);
    void render(const Renderer_2D& renderer);
    glm::ivec4 getCoords(){return glm::ivec4(b_main_body.pos, b_main_body.size);}

    // call on press event leftbutton
    bool left_button_press_event(const glm::ivec2& button, const glm::vec4& camera);
    // no left button click ---- call when active
    void processInput_when_active(const SDL_Event& event, float scale);

    bool is_active = true;

    std::size_t id;
    float frametime = 0.f;
    Box b_main_body;
private:
    int width = 5.f;
    glm::vec4 color_body{1.f, 0.f, 1.f, 0.1f};
    glm::vec4 color_side_box{0.f, 0.f, 1.f, 0.6f};
    Box b_left;
    Box b_right;
    Box b_top;
    Box b_down;

    glm::vec2 get_cursor_camera_space(int x, int y, const glm::vec4& camera);

    void update_boxes_size(bool snap_to_grid);

    // later use to generating frame data


};

#endif // SELECTION_RECT_HPP
