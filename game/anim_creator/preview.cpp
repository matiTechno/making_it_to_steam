#include "preview.hpp"

bool Preview::origin_visible = true;
bool Preview::frame_rect_visible = true;

Preview::Preview(const std::list<Frame>& frames, float scale, const Texture& texture,
                 const std::vector<const char*>& coll_group_names, bool flipped):
    frames(frames),
    scale(scale),
    texture(texture),
    coll_group_names(coll_group_names),
    show_coll_group(coll_group_names.size(), false),
    flipped(flipped)
{
    is_opaque = false;
    this->frames.sort([](const Frame& f1, const Frame& f2)
    {return f1.anim_rect.id < f2.anim_rect.id;});

    origin_pos = coords.size / 2;

    frame = this->frames.begin();
    prev_frame = this->frames.end();
    --prev_frame;

    pos = glm::vec2(origin_pos) - get_origin_distance(frame);
}

void Preview::update()
{
    if(!play)
        return;

    accumulator += dt;
    while(accumulator >= frame->anim_rect.frametime)
    {
        accumulator -= frame->anim_rect.frametime;

        ++frame;
        ++prev_frame;
        if(frame == frames.end())
            frame = frames.begin();
        if(prev_frame == frames.end())
            prev_frame = frames.begin();

        pos += get_origin_distance(prev_frame) - get_origin_distance(frame);
    }
}

void Preview::render2()
{
    renderer.load_projection(glm::vec4(0, 0, coords.size));
    Sprite sprite;
    sprite.position = glm::vec2(0);
    sprite.size = coords.size;
    sprite.color = glm::vec4(0, 0, 0, 0.5f);
    renderer.render(sprite);
    renderer.load_projection(camera);
    renderer.beg_batching();

    if(origin_visible)
    {
        Sprite sprite;
        sprite.position = glm::vec2(origin_pos) - origin_rect_size / 2.f;
        sprite.size = origin_rect_size;
        sprite.color = glm::vec4(1.f, 0, 0, 0.1f);
        renderer.render(sprite);
    }
    if(frame_rect_visible)
    {
        Sprite sprite;
        sprite.position = pos;
        sprite.size = glm::vec2(frame->anim_rect.get_coords().z, frame->anim_rect.get_coords().w) * scale;
        sprite.color = glm::vec4(0, 1.f, 0, 0.3f);
        renderer.render(sprite);
    }
    {
        Sprite sprite;
        sprite.position = pos;
        sprite.size = glm::vec2(frame->anim_rect.get_coords().z, frame->anim_rect.get_coords().w) * scale;
        sprite.texture = &texture;
        sprite.texCoords = frame->anim_rect.get_coords();
        sprite.sampl_type = Sampl_type::nearest;
        if(flipped)
            renderer.render_flipped(sprite);
        else
            renderer.render(sprite);

        for(std::size_t i = 0; i < coll_group_names.size(); ++i)
        {
            if(show_coll_group[i])
            {
                auto it = frame->coll_groups.find(coll_group_names[i]);
                if(it != frame->coll_groups.end())
                    for(auto& rect: it->second)
                    {
                        Sprite sprite2;
                        sprite2.position = glm::vec2(rect.coll_cords.x, rect.coll_cords.y) * sprite.size + pos;
                        sprite2.size = glm::vec2(rect.coll_cords.z, rect.coll_cords.w) * sprite.size;
                        sprite2.color = glm::vec4(1.f, 0, 0, 0.6f);
                        renderer.render(sprite2);
                    }
            }
        }
    }
    if(origin_visible)
    {
        Sprite sprite;
        sprite.position = origin_pos;
        sprite.size = glm::vec2(1.f);
        sprite.color = glm::vec4(1.f, 0, 0, 1.f);
        renderer.render(sprite);
    }
    renderer.end_batching();
}

void Preview::render_ImGui()
{
    ImGui::Begin("preview");
    {
        if(ImGui::Button("quit preview"))
            num_scenes_to_pop = 1;

        if(ImGui::Button("play / stop"))
            play = !play;
        ImGui::Checkbox("origin rect", &origin_visible);
        ImGui::Checkbox("frame rect", &frame_rect_visible);

        if(ImGui::TreeNode("collision groups"))
        {
            for(std::size_t i = 0; i < coll_group_names.size(); ++i)
                ImGui::Selectable(coll_group_names[i], &*&show_coll_group[i]);
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

glm::vec2 Preview::get_origin_distance(std::list<Frame>::iterator frame)
{
    return glm::vec2(frame->anim_rect.origin * glm::vec2(frame->anim_rect.get_coords().z, frame->anim_rect.get_coords().w) * scale);
}
