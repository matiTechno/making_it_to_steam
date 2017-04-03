#include "preview.hpp"

bool Preview::origin_visible = true;
bool Preview::frame_rect_visible = true;

Preview::Preview(const std::list<Anim_rect>& frames, float scale, const Texture& texture):
    frames(frames),
    scale(scale),
    texture(texture)
{
    is_opaque = false;
    this->frames.sort([](const Anim_rect& f1, const Anim_rect& f2)
    {return f1.id < f2.id;});

    origin_pos = coords.size / 2;

    frame = this->frames.begin();
    prev_frame = this->frames.end();
    --prev_frame;

    pos = glm::vec2(origin_pos) - get_origin_distance(frame);
}

void Preview::update()
{
    accumulator += dt;
    while(accumulator >= frame->frametime)
    {
        accumulator -= frame->frametime;

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
        sprite.size = glm::vec2(frame->get_coords().z, frame->get_coords().w) * scale;
        sprite.color = glm::vec4(0, 1.f, 0, 0.3f);
        renderer.render(sprite);
    }
    {
        Sprite sprite;
        sprite.position = pos;
        sprite.size = glm::vec2(frame->get_coords().z, frame->get_coords().w) * scale;
        sprite.texture = &texture;
        sprite.texCoords = frame->get_coords();
        sprite.sampl_type = Sampl_type::nearest;
        renderer.render(sprite);
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

        ImGui::Checkbox("origin rect", &origin_visible);
        ImGui::Checkbox("frame rect", &frame_rect_visible);
    }
    ImGui::End();
}

glm::vec2 Preview::get_origin_distance(std::list<Anim_rect>::iterator frame)
{
    return glm::vec2(frame->origin * glm::vec2(frame->get_coords().z, frame->get_coords().w) * scale);
}
