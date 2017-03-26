#include "anim_frame.hpp"

glm::vec2 get_cursor_cam_pos(int x, int y, const glm::vec4& camera)
{
    return glm::vec2(camera.x, camera.y) +
            glm::vec2(camera.z, camera.w) / glm::vec2(App::get_fb_size()) * glm::vec2(x, y);
}

Anim_frame::Anim_frame(const glm::vec2& position, const glm::vec2& size, int id, float frametime,
                       const glm::vec2& origin):
    frametime(frametime),
    id(id),
    origin(origin),
    boxes(4)
{
    main_box.pos = position;
    main_box.size = size;
}

void Anim_frame::render(const Renderer_2D& renderer)
{
    update_boxes_to_main(snap_to_grid);

    Sprite sprite;
    if(is_selected)
        sprite.color = col_main;
    else
        sprite.color = col_inactive;
    sprite.position = main_box.pos;
    sprite.size = main_box.size;
    renderer.render(sprite);

    if(is_selected)
    {
        for(auto& box: boxes)
        {
            Sprite sprite;
            sprite.color = col_box;
            sprite.size = box.size;
            sprite.position = box.pos;
            renderer.render(sprite);
        }
    }
    snap_to_grid = false;
}

bool Anim_frame::on_left_button_press(int x, int y, const glm::vec4& camera)
{
    bool prev_selected = is_selected;
    is_selected = false;
    glm::vec2 cursor_cam_pos = get_cursor_cam_pos(x, y, camera);

    if(is_p_in_box(cursor_cam_pos, main_box))
    {
        is_selected = true;
        main_box.is_selected = true;
    }
    if(prev_selected)
        for(auto& box: boxes)
        {
            if(is_p_in_box(cursor_cam_pos, box))
            {
                is_selected = true;
                box.is_selected = true;
            }
        }

    if(!prev_selected && is_selected)
        move_lock = true;
    else if(prev_selected && is_selected)
        move_lock = false;

    return is_selected;
}

void Anim_frame::on_left_button_release()
{
    if(main_box.is_selected)
    {
        main_box.is_selected = false;
        snap_to_grid = true;
    }
    for(auto& box: boxes)
    {
        if(box.is_selected)
        {
            box.is_selected = false;
            snap_to_grid = true;
        }
    }
}

void Anim_frame::on_mouse_motion(int relx, int rely, float camera_scale)
{
    if(move_lock)
        return;

    glm::vec2 diff(relx * camera_scale, rely * camera_scale);

    if(main_box.is_selected)
        main_box.pos += diff;

    if(boxes[left].is_selected)
    {
        main_box.pos.x += diff.x;
        main_box.size.x -= diff.x;
    }
    if(boxes[right].is_selected)
        main_box.size.x += diff.x;

    if(boxes[top].is_selected)
    {
        main_box.pos.y += diff.y;
        main_box.size.y -= diff.y;
    }
    if(boxes[bottom].is_selected)
        main_box.size.y += diff.y;

    if(main_box.size.x < 1.f)
        main_box.size.x = 1.f;
    if(main_box.size.y < 1.f)
        main_box.size.y = 1.f;
}

void Anim_frame::update_boxes_to_main(bool snap_to_grid)
{
    if(snap_to_grid)
    {
        main_box.pos = glm::ivec2(main_box.pos + 0.5f);
        main_box.size = glm::ivec2(main_box.size + 0.5f);
    }
    boxes[left].pos = main_box.pos - box_width;
    boxes[left].size = glm::vec2(box_width, main_box.size.y + 2 * box_width);
    boxes[right].pos = main_box.pos + glm::vec2(main_box.size.x, -box_width);
    boxes[right].size = boxes[left].size;
    boxes[top].pos = boxes[left].pos;
    boxes[top].size = glm::vec2(main_box.size.x + 2 * box_width, box_width);
    boxes[bottom].pos = main_box.pos + glm::vec2(-box_width, main_box.size.y);
    boxes[bottom].size = boxes[top].size;
}

bool Anim_frame::is_p_in_box(const glm::vec2& point, const Box& box)
{
    if(point.x >= box.pos.x && point.x < box.pos.x + box.size.x &&
            point.y >= box.pos.y && point.y < box.pos.y + box.size.y)
        return true;
    return false;
}
