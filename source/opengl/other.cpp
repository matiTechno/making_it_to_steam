#include "other.hpp"
#include "../app.hpp"

GLenum Blend_alpha::src = GL_ONE, Blend_alpha::dst = GL_ZERO;

void Blend_alpha::set(GLenum src, GLenum dst, GLuint buf)
{
    if(buf)
    {
        glBlendFunci(buf, src, dst);
        return;
    }

    if(src != Blend_alpha::src || dst != Blend_alpha::dst)
    {
        Blend_alpha::src = src;
        Blend_alpha::dst = dst;
        glBlendFunci(0, src, dst);
    }
}

void Viewport::set(const Scene_coords& coords)
{
    int flipped_y = App::get_fb_size().y - (coords.y + coords.w);
    glViewport(coords.x, flipped_y, coords.z, coords.w);
}

void Viewport::set(int x, int y, int w, int h)
{glViewport(x, y, w, h);}

void Scissor::set(const Scene_coords& coords)
{
    int flipped_y = App::get_fb_size().y - (coords.y + coords.w);
    glScissor(coords.x, flipped_y, coords.z, coords.w);
}

void Scissor::set(int x, int y, int w, int h)
{glScissor(x, y, w, h);}

void Scissor::set_scaled(const Scene_coords& coords,
                         const glm::ivec2& input_tex_size, const glm::ivec2& output_tex_size)
{
    glm::vec2 scale = glm::vec2(output_tex_size) / glm::vec2(input_tex_size);

    int flipped_y = App::get_fb_size().y - (coords.y + coords.w);

    glm::ivec4 gl_coords(static_cast<float>(coords.x) * scale.x,
                         static_cast<float>(flipped_y) * scale.y,
                         static_cast<float>(coords.z) * scale.x + 1.f,
                         static_cast<float>(coords.w) * scale.y + 1.f);

    glScissor(gl_coords.x, gl_coords.y, gl_coords.z, gl_coords.w);
}
