#include "set_coll_data.hpp"

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

void Set_coll_data::processEvent2(const SDL_Event& event)
{
    (void)event;
}

void Set_coll_data::render_ImGui()
{
    ImGui::Begin("set collision data");
    {
        ImGui::PushItemWidth(200);
        if(ImGui::Button("quit"))
        {
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
    renderer.load_projection(camera);
    renderer.beg_batching();
    {
        frame.anim_rect.render(renderer, texture, saved_coords, false);
    }
    renderer.end_batching();
}
