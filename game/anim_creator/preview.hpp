#pragma once

#include "camera_scene.hpp"
#include "animation.hpp"
#include <list>
#include <deque>

class Preview: public Camera_scene
{
public:
    Preview(const std::list<Frame>& frames, float scale, const Texture& texture,
            const std::vector<const char*>& coll_group_names, bool flipped = false);

    void on_quit_event() override;

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
    static bool repeat;
    bool play = true;
    const std::vector<const char*>& coll_group_names;
    std::deque<bool> show_coll_group;
    bool flipped;

    glm::vec2 get_origin_distance(std::list<Frame>::iterator frame);

    void render2() override;
};
