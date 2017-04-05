#pragma once

#include "anim_rect.hpp"
#include <list>
#include <unordered_map>

struct Frame
{
    Anim_rect anim_rect;
    // ............... collision group
    std::unordered_map<std::string, std::list<Anim_rect>> coll_groups;
};

struct Animation
{
    std::list<Frame> frames;
    std::size_t id = 0;
    float global_frametime = 0.4f;
    glm::vec2 global_origin{0};
    glm::ivec2 new_frame_size{80, 80};
    bool mirror = false;
};
