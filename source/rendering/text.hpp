#ifndef TEXT_HPP
#define TEXT_HPP

#include "font_loader.hpp"
#include <string>
#include "render_obj_base.hpp"

// no kerning support for now
// rotation might produce artifacts
class Text: public Render_obj_base
{
public:
    friend class Renderer_2D;

    Text(const Font* font):
        scale(1.f),
        render_quads(false),
        snap_to_pixel_grid(true),
        font(font)
    {}

    // produces artifacts
    void set_pixel_size(unsigned size)
    {
        scale = static_cast<float>(size) / static_cast<float>(font->pixel_size);
    }

    glm::vec2 getSize() const;

    std::string text;
    float scale;
    bool render_quads;
    // only when no rotation
    bool snap_to_pixel_grid;

private:
    const Font* font;
};

#endif // TEXT_HPP
