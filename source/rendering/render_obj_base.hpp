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
        blend_sfactor(GL_SRC_ALPHA),
        blend_dfactor(GL_ONE_MINUS_SRC_ALPHA),
        sampl_type(Sampl_type::linear)
    {}

    Render_obj_base(No_init)
    {}

    glm::vec2 position;
    glm::vec4 color;
    // rotation_point is relative to sprite top-left corner
    glm::vec2 rotation_point;
    // radians
    float rotation;
    bool bloom;
    GLenum blend_sfactor, blend_dfactor;

    Sampl_type sampl_type;
};

#endif // RENDER_OBJ_BASE_HPP
