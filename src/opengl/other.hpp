#ifndef OTHER_HPP
#define OTHER_HPP

#include <MITS/scene.hpp>
#include <MITS/glad/glad.h>

class Blend_alpha
{
public:
    static void set(GLenum src, GLenum dst);

private:
    static GLenum src, dst;
};

// for Scene_coords y is flipped when comparing to gl format
class Viewport
{
public:
    static void set(const Scene_coords& coords);
    static void set(int x, int y, int w, int h);
};

class Scissor
{
public:
    static void set_scaled(const Scene_coords& coords,
                           const glm::ivec2& input_tex_size, const glm::ivec2& output_tex_size);
    static void set(const Scene_coords& coords);
    static void set(int x, int y, int w, int h);
};

#endif // OTHER_HPP
