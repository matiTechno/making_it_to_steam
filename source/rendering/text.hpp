#ifndef TEXT_HPP
#define TEXT_HPP

#include "font_loader.hpp"
#include <string>
#include "render_obj_base.hpp"

// no kerning support for now
class Text: public Render_obj_base
{
public:
    friend class Renderer_2D;

    Text(const Font* font):
        scale(1.f),
        render_quads(false),
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

private:
    const Font* font;
};

#endif // TEXT_HPP
