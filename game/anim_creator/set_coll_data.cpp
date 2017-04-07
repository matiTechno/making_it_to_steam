#include "set_coll_data.hpp"
#include "anim_creator.hpp"

Set_coll_data::Set_coll_data(Frame& frame, Texture& texture, const std::vector<const char*>& coll_group_names):
    frame(frame),
    texture(texture),
    coll_group_names(coll_group_names)
{
    is_opaque = false;
    saved_coords = frame.anim_rect.get_coords();
    frame.anim_rect.set_position(coords.size / 2 - glm::ivec2(saved_coords.z,
                                                              saved_coords.w));

    for(auto name: coll_group_names)
        frame.coll_groups[name];

    active_coll_group = &frame.coll_groups.at(coll_group_names.front());
}

void Set_coll_data::on_quit_event()
{
    Anim_creator::handle->quit_request = true;
}

void Set_coll_data::processEvent2(const SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN && !event.key.repeat)
    {
        if(event.key.keysym.sym == SDLK_n)
        {
            if(active_coll_group->size())
                active_coll_group->front().is_selected = false;

            active_coll_group->emplace_front(0, get_cursor_cam_pos(get_cursor_pos().x, get_cursor_pos().y, camera)
                                             - glm::vec2(saved_coords.z, saved_coords.w) / 2.f,
                                             glm::vec2(saved_coords.z, saved_coords.w), 0, glm::vec2());
        }
        else if(event.key.keysym.sym == SDLK_r && active_coll_group->size() && active_coll_group->front().is_selected)
            active_coll_group->erase(active_coll_group->begin());
    }
    else if(event.type == SDL_MOUSEMOTION && active_coll_group->size() && !get_is_rmb_pressed())
    {
        if(active_coll_group->front().is_selected)
            active_coll_group->front().on_mouse_motion(event.motion.xrel, event.motion.yrel, get_camera_scale());
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        for(auto it =active_coll_group->begin(); it != active_coll_group->end(); ++it)
        {
            if(it->on_left_button_press(event.button.x, event.button.y, camera, false))
            {
                if(&*it != &active_coll_group->front())
                {
                    active_coll_group->push_front(std::move(*it));
                    active_coll_group->erase(it);
                }
                break;
            }
        }

    else if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && active_coll_group->size())
        active_coll_group->front().on_left_button_release();
}

void Set_coll_data::render_ImGui()
{
    ImGui::Begin("set collision data");
    {
        ImGui::PushItemWidth(200);
        if(ImGui::Button("quit"))
        {
            for(auto& coll_group: frame.coll_groups)
                for(auto& rect: coll_group.second)
                {
                    rect.coll_cords.z = rect.get_coords().z / saved_coords.z;
                    rect.coll_cords.w = rect.get_coords().w / saved_coords.w;
                    rect.coll_cords.x = (rect.get_coords().x - frame.anim_rect.get_coords().x) / saved_coords.z;
                    rect.coll_cords.y = (rect.get_coords().y - frame.anim_rect.get_coords().y) / saved_coords.w;
                }

            frame.anim_rect.set_position(glm::vec2(saved_coords.x, saved_coords.y));
            num_scenes_to_pop = 1;
        }
        ImGui::Combo("collision group", &current_name, coll_group_names.data(),
                     static_cast<int>(coll_group_names.size()));
        active_coll_group = &frame.coll_groups.at(coll_group_names[static_cast<std::size_t>(current_name)]);
    }
    ImGui::End();
}

void Set_coll_data::render2()
{
    if(ImGui_wants_input && active_coll_group->size())
        active_coll_group->front().on_left_button_release();

    renderer.load_projection(camera);
    renderer.beg_batching();
    {
        frame.anim_rect.render(renderer, texture, saved_coords, false);

        for(auto it = active_coll_group->rbegin(); it != active_coll_group->rend(); ++it)
            it->render(renderer, true);
    }
    renderer.end_batching();
}
