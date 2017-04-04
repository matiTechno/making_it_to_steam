#pragma once

#include "camera_scene.hpp"
#include "animation.hpp"

class Adjust_anims: public Camera_scene
{
public:
    Adjust_anims(Animation& anim1, const Animation& anim2, const Texture& texture);

    void render_ImGui() override;

private:
    Animation& anim1;
    const Texture& texture;
    std::unique_ptr<Anim_rect> back_frame;
    Anim_rect* front_frame;
    bool was_selected = false;
    glm::ivec4 back_frame_coords, front_frame_coords;
    glm::ivec2 origin_pos;
    glm::vec2 origin_rect_size{80.f};
    glm::vec2 front_frame_init_pos;

    glm::vec2 get_origin_distance(const Anim_rect& frame);

    void processEvent2(const SDL_Event& event) override;
    void render2() override;
};
