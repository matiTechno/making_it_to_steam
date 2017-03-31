#ifndef RENDER_OBJ_BASE_HPP
#define RENDER_OBJ_BASE_HPP

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "../glad/glad.h"

class No_init
{};

enum class Sampl_type
{
    linear,
    nearest
};

class Render_obj_base
{
public:
    Render_obj_base():
        color(1.f, 1.f, 1.f, 1.f),
        rotation(0.f),
        bloom(false),
        src_alpha(GL_SRC_ALPHA),
        dst_alpha(GL_ONE_MINUS_SRC_ALPHA)
    {}

    Render_obj_base(No_init)
    {}

    glm::vec2 position;
    glm::vec4 color;
    // rotation_point is relative to sprite top-left corner
    // you probably want to update it whenever sprite size changes
    glm::vec2 rotation_point;
    // radians
    float rotation;
    bool bloom;
    // for blending
    GLenum src_alpha, dst_alpha;
};

#endif // RENDER_OBJ_BASE_HPP
