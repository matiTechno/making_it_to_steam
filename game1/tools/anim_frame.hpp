#ifndef ANIM_FRAME_HPP
#define ANIM_FRAME_HPP

#include "../../source/common_scene.hpp"

glm::vec2 get_cursor_cam_pos(int x, int y, const glm::vec4& camera);

struct Box
{
    glm::vec2 pos;
    glm::vec2 size;
    bool is_selected;
};

class Anim_frame
{
public:
    Anim_frame(const glm::vec2& position, const glm::vec2& size, std::size_t index);

    void render(const Renderer_2D& renderer);
    // returns true if reselected
    bool on_left_button_press(int x, int y, const glm::vec4& camera);
    void on_left_button_release();
    void on_mouse_motion(int relx, int rely, float camera_scale);

    glm::vec4 getCoords(){return glm::vec4(main_box.pos, main_box.size);}
    void deselect(){is_selected = false;}
private:
    float frametime = 0.040f;
    std::size_t index;
    glm::vec2 origin{0.5f, 0.5f};
    bool is_selected = true;
    float box_width = 5.f;
    std::vector<Box> boxes;
    Box main_box;
    glm::vec4 col_main{1.f, 0.f, 1.f, 0.1f};
    glm::vec4 col_box{0.f, 0.f, 1.f, 0.6f};
    glm::vec4 col_inactive{0.f, 1.f, 0.f, 0.6f};
    bool snap_to_grid = true;
    bool move_lock = false;

    void update_boxes_to_main(bool snap_to_grid);

    enum
    {
        left,
        right,
        top,
        bottom
    };

    bool is_p_in_box(const glm::vec2& point, const Box& box);
};

#endif // ANIM_FRAME_HPP
