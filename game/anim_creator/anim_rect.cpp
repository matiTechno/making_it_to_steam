#include "anim_rect.hpp"
#include <MITS/rendering/sprite.hpp>
#include <MITS/rendering/renderer_2d.hpp>
#include <SDL2/SDL_events.h>
glm::vec2 get_cursor_cam_pos(int x, int y, const glm::vec4& camera);

Anim_rect::Anim_rect(std::size_t id, const glm::vec2& pos, const glm::vec2& size, float frametime, const glm::vec2& origin):
    id(id),
    frametime(frametime),
    origin(origin),
    boxes(4)
{
    main_box.pos = pos;
    main_box.size = size;
}

bool Anim_rect::on_left_button_press(int x, int y, const glm::vec4& camera, bool origin_mode)
{
    bool prev_selected = is_selected;
    is_selected = false;
    glm::vec2 cursor_cam_pos = get_cursor_cam_pos(x, y, camera);

    if(is_cursor_in_box(cursor_cam_pos, main_box))
    {
        main_box.is_selected = true;
        is_selected = true;
    }

    if(prev_selected && !origin_mode)
        for(auto& box: boxes)
            if(is_cursor_in_box(cursor_cam_pos, box))
            {
                box.is_selected = true;
                is_selected = true;
            }

    if(origin_mode)
        move_lock = false;
    else if(!prev_selected && is_selected)
        move_lock = true;
    else if(prev_selected && is_selected)
        move_lock = false;

    return is_selected;
}

void Anim_rect::on_mouse_motion(int relx, int rely, float camera_scale)
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

void Anim_rect::render(const Renderer_2D& renderer, bool coll_mode) const
{
    update_boxes_to_main(snap_to_grid);
    snap_to_grid = false;

    Sprite sprite;
    sprite.position = main_box.pos;
    sprite.size = main_box.size;
    if(is_selected)
        sprite.color = col_main;
    else if(coll_mode)
        sprite.color = col_main_coll_mode_inactive;
    else
        sprite.color = col_main_inactive;
    renderer.render(sprite);

    if(is_selected)
        for(auto& box: boxes)
        {
            Sprite sprite;
            sprite.color = col_boxes;
            sprite.size = box.size;
            sprite.position = box.pos;
            renderer.render(sprite);
        }
}

glm::ivec4 Anim_rect::get_coords() const
{return glm::ivec4(main_box.pos, main_box.size);}

void Anim_rect::on_left_button_release()
{
    main_box.is_selected = false;

    for(auto& box: boxes)
        box.is_selected = false;

    snap_to_grid = true;
}

void Anim_rect::update_boxes_to_main(bool snap_to_grid) const
{
    if(snap_to_grid)
    {
        if(main_box.pos.x < 0.f)
            main_box.pos.x = static_cast<int>(main_box.pos.x - 0.5f);
        else
            main_box.pos.x = static_cast<int>(main_box.pos.x + 0.5f);

        if(main_box.pos.y < 0.f)
            main_box.pos.y = static_cast<int>(main_box.pos.y - 0.5f);
        else
            main_box.pos.y = static_cast<int>(main_box.pos.y + 0.5f);

        main_box.size = glm::ivec2(main_box.size + 0.5f);
    }
    boxes[left].pos = main_box.pos - static_cast<float>(box_width);
    boxes[left].size = glm::vec2(box_width, main_box.size.y + 2 * box_width);
    boxes[right].pos = main_box.pos + glm::vec2(main_box.size.x, -box_width);
    boxes[right].size = boxes[left].size;
    boxes[top].pos = boxes[left].pos;
    boxes[top].size = glm::vec2(main_box.size.x + 2 * box_width, box_width);
    boxes[bottom].pos = main_box.pos + glm::vec2(-box_width, main_box.size.y);
    boxes[bottom].size = boxes[top].size;
}

bool Anim_rect::is_cursor_in_box(const glm::vec2& pos, const Box& box) const
{
    if(pos.x >= box.pos.x && pos.x < box.pos.x + box.size.x &&
            pos.y >= box.pos.y && pos.y < box.pos.y + box.size.y)
        return true;
    return false;
}

void Anim_rect::render(const Renderer_2D& renderer, const Texture& texture, const glm::ivec4& coords, bool rect) const
{
    update_boxes_to_main(snap_to_grid);
    snap_to_grid = false;
    if(rect)
    {
        Sprite sprite;
        sprite.position = main_box.pos;
        sprite.size = main_box.size;
        if(is_selected)
            sprite.color = col_main_origin_mode;
        else
            sprite.color = col_main_inactive;
        renderer.render(sprite);
    }
    {
        Sprite sprite;
        sprite.position = main_box.pos;
        sprite.size = main_box.size;
        sprite.sampl_type = Sampl_type::nearest;
        sprite.color.a = alpha;
        sprite.texture = &texture;
        sprite.texCoords = coords;
        renderer.render(sprite);
    }
}

void Anim_rect::set_position(const glm::vec2& pos)
{main_box.pos = pos;}

void Anim_rect::set_size(const glm::vec2& size)
{main_box.size = size;}
