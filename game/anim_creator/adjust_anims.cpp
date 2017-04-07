#include "adjust_anims.hpp"
#include "anim_creator.hpp"

Adjust_anims::Adjust_anims(Animation& anim1, const Animation& anim2, const Texture& texture, bool first):
    anim1(anim1),
    texture(texture)
{
    is_opaque = false;
    origin_pos = coords.size / 2;
    {
        std::size_t id = 1000000;
        for(auto& frame: anim2.frames)
            if(frame.anim_rect.id < id)
                id = frame.anim_rect.id;
        for(auto& frame: anim2.frames)
            if(frame.anim_rect.id == id)
                back_frame = std::make_unique<Anim_rect>(frame.anim_rect);

        back_frame_coords = back_frame->get_coords();
        back_frame->set_position(glm::vec2(origin_pos) - get_origin_distance(*back_frame));
        back_frame->is_selected = false;
    }
    {
        std::size_t id;
        if(first)
        {
            id = 1000000;
            for(auto& frame: anim1.frames)
            {
                if(frame.anim_rect.id < id)
                    id = frame.anim_rect.id;
            }
        }
        else
        {
            id = 0;
            for(auto& frame: anim1.frames)
            {
                if(frame.anim_rect.id > id)
                    id = frame.anim_rect.id;
            }

        }
        for(auto& frame: anim1.frames)
            if(frame.anim_rect.id == id)
            {
                if(frame.anim_rect.is_selected)
                    was_selected = true;
                front_frame = &frame.anim_rect;
            }

        front_frame_coords = front_frame->get_coords();
        front_frame_init_pos = glm::vec2(origin_pos) - get_origin_distance(*front_frame);
        front_frame->set_position(front_frame_init_pos);
    }
}

void Adjust_anims::on_quit_event()
{
    Anim_creator::handle->quit_request =  true;
}

void Adjust_anims::render_ImGui()
{
    ImGui::Begin("adjust animations");
    {
        if(ImGui::Button("quit"))
        {
            num_scenes_to_pop = 1;
            if(!was_selected)
                front_frame->is_selected = false;

            for(auto& frame: anim1.frames)
            {
                frame.anim_rect.origin -= (glm::vec2(front_frame->get_coords().x, front_frame->get_coords().y)
                                           - front_frame_init_pos)
                        / glm::vec2(front_frame->get_coords().z, front_frame->get_coords().w);
            }
            front_frame->set_position(glm::vec2(front_frame_coords.x, front_frame_coords.y));
            front_frame->alpha = 1.f;
        }

        ImGui::SliderFloat("alpha", &front_frame->alpha, 0, 1.f, "%.2f");
    }
    ImGui::End();
}

void Adjust_anims::processEvent2(const SDL_Event& event)
{
    if(event.type == SDL_MOUSEMOTION && !get_is_rmb_pressed())
    {
        if(front_frame->is_selected)
            front_frame->on_mouse_motion(event.motion.xrel, event.motion.yrel, get_camera_scale());
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        front_frame->on_left_button_press(event.button.x, event.button.y, camera, true);

    else if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
        front_frame->on_left_button_release();
}

void Adjust_anims::render2()
{
    if(ImGui_wants_input)
        front_frame->on_left_button_release();
    front_frame->is_selected = true;

    renderer.load_projection(camera);
    {
        Sprite sprite;
        sprite.position = glm::vec2(origin_pos) - origin_rect_size / 2.f;
        sprite.size = origin_rect_size;
        sprite.color = glm::vec4(1.f, 0, 0, 0.1f);
        renderer.render(sprite);
    }
    back_frame->render(renderer, texture, back_frame_coords, true);
    front_frame->render(renderer, texture, front_frame_coords, true);
    {
        Sprite sprite;
        sprite.position = origin_pos;
        sprite.size = glm::vec2(1.f);
        sprite.color = glm::vec4(1.f, 0, 0, 1.f);
        renderer.render(sprite);
    }
}

glm::vec2 Adjust_anims::get_origin_distance(const Anim_rect& frame)
{
    return glm::vec2(frame.origin * glm::vec2(frame.get_coords().z, frame.get_coords().w));
}
