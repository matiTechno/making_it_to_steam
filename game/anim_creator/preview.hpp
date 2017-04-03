#pragma once

#include "camera_scene.hpp"
#include "anim_rect.hpp"
#include <list>

class Preview: public Camera_scene
{
public:
    Preview(const std::list<Anim_rect>& frames, float scale, const Texture& texture);

    void update() override;

    void render_ImGui() override;

private:
    std::list<Anim_rect> frames;
    float scale;
    const Texture& texture;
    float accumulator = 0;
    glm::vec2 pos;
    std::list<Anim_rect>::iterator frame, prev_frame;
    glm::ivec2 origin_pos;
    glm::vec2 origin_rect_size{80.f};
    bool origin_visible = true;
    bool frame_rect_visible = true;

    glm::vec2 get_origin_distance(std::list<Anim_rect>::iterator frame);

    void render2() override;
};
