#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "render_obj_base.hpp"
class Texture;

class Sprite: public Render_obj_base
{
public:
    Sprite():
        texture(nullptr),
        sampl_type(Sampl_type::linear)
    {}

    Sprite(No_init):
        Render_obj_base(No_init())
    {}

    glm::vec2 size;
    glm::ivec4 texCoords;
    const Texture* texture;
    Sampl_type sampl_type;
};

#endif // SPRITE_HPP
