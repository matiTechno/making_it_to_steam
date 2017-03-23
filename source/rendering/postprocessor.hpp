#ifndef POSTPROCESSOR_HPP
#define POSTPROCESSOR_HPP

#include "../opengl/buffers.hpp"
#include "../opengl/texture.hpp"
#include <glm/vec2.hpp>
#include "../opengl/shader.hpp"
#include <memory>
#include "../scene.hpp"

// HOW TO USE:
// begRender()
// ...
// render all your stuff with Renderer_2D
// ...
// endRender() (when num_blurs != 0 Scene loses transparency)
// ...
// now apply as many effects as you want with apply_effect()
// color space is linear (if you correctly marked sprite textures as sRGB)
// ...
// you can still render some sprites on top of effects
// ...
// render the final texture to default framebuffer with render_fb0()
// ...
// after that you can render rest of your stuff directly to default fb

class Postprocessor
{
public:
    Postprocessor(const glm::ivec2& fbSize);
    Postprocessor(const Postprocessor&) = delete;

    void begRender() const;

    void endRender(int num_blurs) const;

    void apply_effect(const Shader& shader) const;

    void render_fb0() const;

    bool has_finished()
    {return finished;}
    void set_new_size(const glm::ivec2& fbSize);
    void set_new_scene(const Scene_coords& coords);

private:
    static bool isCurrent;

    mutable bool finished;
    glm::ivec2 fbSize;
    int tex_type;
    VAO vao;
    BO vbo;
    Sampler sampler;
    Shader shader_blur, shader_blend, shader_final;
    std::unique_ptr<Texture> tex_base, tex_bright, tex_pp1, tex_pp2;
    FBO fb_beg, fb_pp1, fb_pp2;
    // members for custom effects support
    std::unique_ptr<Texture> tex_custom_1, tex_custom_2;
    FBO fb_custom_1, fb_custom_2;
    mutable int next_tex_custom_bind;

    Scene_coords scene_coords;
};

#endif // POSTPROCESSOR_HPP
