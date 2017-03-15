#ifndef RENDERER_2D_HPP
#define RENDERER_2D_HPP

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
class Sprite;
class Text;
#include "../opengl/buffers.hpp"
#include "../opengl/texture.hpp"
#include "../opengl/shader.hpp"
#include <vector>
#include "render_obj_base.hpp"

// NOTE: do not change blend factors manually
class Renderer_2D
{
public:
    Renderer_2D();
    Renderer_2D(const Renderer_2D&) = delete;

    // this is good for text / particles (single texture atlas) rendering
    // between these calls render function will batch sprite / text data
    // any change in the following sprite / text members will start new batch:
    // * blend_sfactor
    // * blend_dfactor
    // * sampl_type
    // * texture / font
    void beg_batching() const;
    void end_batching() const;

    void render(const Sprite& sprite) const;
    void render(const Text& text) const;

    void load_projection(const glm::mat4& matrix) const;

private:
    static bool isCurrent;

    mutable bool is_batching_on;

    VAO vao;
    BO vbo_static;
    Sampler sampler_linear, sampler_nearest;
    mutable GLenum blend_sfactor, blend_dfactor;
    Shader shader_uniform;

    // don't change the order
    struct Vbo_instance
    {
        // size + shift
        glm::vec4 texData;
        glm::vec4 color;
        /// bloom + type
        glm::ivec2 bloom_type;
        glm::mat4 model;
    };

    struct Batch_state
    {
        GLenum blend_sfactor;
        GLenum blend_dfactor;
        Texture* texture;
        Render_obj_base::Sampl_type sampl_type;
    };

    // members for batching system
    mutable std::vector<std::vector<Vbo_instance>> batches;
    mutable std::vector<Batch_state> b_states;
    BO vbo_dynamic;
    Shader shader_batching;

    void set_blend_func(GLenum sfactor, GLenum dfactor) const;

    void uniform_render(const Sprite& sprite) const;
    void uniform_render(const Text& text) const;
    void batch_render(const Sprite& sprite) const;
    void batch_render(const Text& text) const;
};

#endif // RENDERER_2D_HPP
