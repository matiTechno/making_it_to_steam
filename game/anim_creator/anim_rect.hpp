#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
class Renderer_2D;
#include <vector>
class Texture;

struct Box
{
    glm::vec2 pos;
    glm::vec2 size;
    bool is_selected = false;
};


class Anim_rect
{
public:
    Anim_rect(std::size_t id, const glm::vec2& pos, const glm::vec2& size, float frametime, const glm::vec2& origin);

    void render(const Renderer_2D& renderer, bool coll_mode = false) const;
    // for origin mode
    void render(const Renderer_2D& renderer, const Texture& texture, const glm::ivec4& coords, bool rect) const;

    glm::ivec4 get_coords() const;

    // call on selected frame when ImGui wants input
    void on_left_button_release();

    // returns true if selected
    bool on_left_button_press(int x, int y, const glm::vec4& camera, bool origin_mode);

    void on_mouse_motion(int relx, int rely, float camera_scale);

    void set_position(const glm::vec2& pos);

    void set_size(const glm::vec2& size);

    std::size_t id;
    float frametime;
    glm::vec2 origin;
    bool is_selected = true;
    float alpha = 1.f;
    glm::vec4 coll_cords;

private:
    enum
    {
        left,
        right,
        top,
        bottom
    };

    mutable std::vector<Box> boxes;
    mutable Box main_box;
    int box_width = 5;
    glm::vec4 col_main{1.f, 0, 1.f, 0.1f};
    glm::vec4 col_main_inactive{0, 1.f, 0, 0.3f};
    glm::vec4 col_main_origin_mode{1.f, 0, 1.f, 0.3f};
    glm::vec4 col_main_coll_mode_inactive{1.f, 0, 0, 0.6f};
    glm::vec4 col_boxes{0, 0, 1.f, 0.6f};
    mutable bool snap_to_grid = true;
    bool move_lock = false;

    void update_boxes_to_main(bool snap_to_grid) const;
    bool is_cursor_in_box(const glm::vec2& pos, const Box& box) const;
};
