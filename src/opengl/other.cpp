#include "other.hpp"
#include <MITS/app.hpp>
#include <glm/vec4.hpp>

GLenum Blend_alpha::src = GL_ONE, Blend_alpha::dst = GL_ZERO;

void Blend_alpha::set(GLenum src, GLenum dst)
{
    if(src != Blend_alpha::src || dst != Blend_alpha::dst)
    {
        Blend_alpha::src = src;
        Blend_alpha::dst = dst;
        glBlendFunc(src, dst);
    }
}

void Viewport::set(const Scene_coords& coords)
{
    int flipped_y = App::get_fb_size().y - (coords.pos.y + coords.size.y);
    glViewport(coords.pos.x, flipped_y, coords.size.x, coords.size.y);
}

void Viewport::set(int x, int y, int w, int h)
{glViewport(x, y, w, h);}

void Scissor::set(const Scene_coords& coords)
{
    int flipped_y = App::get_fb_size().y - (coords.pos.y + coords.size.y);
    glScissor(coords.pos.x, flipped_y, coords.size.x, coords.size.y);
}

void Scissor::set(int x, int y, int w, int h)
{glScissor(x, y, w, h);}

void Scissor::set_scaled(const Scene_coords& coords,
                         const glm::ivec2& input_tex_size, const glm::ivec2& output_tex_size)
{
    glm::vec2 scale = glm::vec2(output_tex_size) / glm::vec2(input_tex_size);

    int flipped_y = App::get_fb_size().y - (coords.pos.y + coords.size.y);

    glm::ivec4 gl_coords(static_cast<float>(coords.pos.x) * scale.x,
                         static_cast<float>(flipped_y) * scale.y,
                         static_cast<float>(coords.size.x) * scale.x + 1.f,
                         static_cast<float>(coords.size.y) * scale.y + 1.f);

    glScissor(gl_coords.x, gl_coords.y, gl_coords.z, gl_coords.w);
}
