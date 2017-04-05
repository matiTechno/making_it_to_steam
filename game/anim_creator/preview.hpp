#pragma once

#include "camera_scene.hpp"
#include "animation.hpp"
#include <list>

class Preview: public Camera_scene
{
public:
    Preview(const std::list<Frame>& frames, float scale, const Texture& texture);

    void update() override;

    void render_ImGui() override;

private:
    std::list<Frame> frames;
    float scale;
    const Texture& texture;
    float accumulator = 0;
    glm::vec2 pos;
    std::list<Frame>::iterator frame, prev_frame;
    glm::ivec2 origin_pos;
    glm::vec2 origin_rect_size{80.f};
    static bool origin_visible;
    static bool frame_rect_visible;

    glm::vec2 get_origin_distance(std::list<Frame>::iterator frame);

    void render2() override;
};
