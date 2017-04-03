#pragma once

#include "anim_rect.hpp"
#include <list>

struct Animation
{
    std::list<Anim_rect> frames;
    std::size_t id = 0;
    float global_frametime = 0.4f;
    glm::vec2 global_origin{0};
    glm::ivec2 new_frame_size{80, 80};
};
