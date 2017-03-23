#ifndef TEXT_HPP
#define TEXT_HPP

#include "font_loader.hpp"
#include <string>
#include "render_obj_base.hpp"

// rotation and scaling produces artifacts
// only ASCII + '\n' or std::map::at will
// throw exception in Text::getSize()
// or Renderer_2D::render()
class Text: public Render_obj_base
{
public:
    friend class Renderer_2D;

    explicit Text(const Font& font):
        scale(1.f),
        render_quads(false),
        snap_to_pixel_grid(true),
        font(&font)
    {}

    void set_pixel_size(unsigned size);

    glm::vec2 getSize() const;

    std::string text;
    float scale;
    bool render_quads;
    // only when no rotation
    // and scale == 1.f
    bool snap_to_pixel_grid;

private:
    const Font* font;
};

#endif // TEXT_HPP
