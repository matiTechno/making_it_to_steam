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
struct P_data;
struct P_data_tCs;

// any change in the following sprite / text members will start new batch:
// * blend_sfactor
// * blend_dfactor
// * sampl_type
// * texture / font
// ...
// calling rend_particles() after beg_batching
// will:
// * call end_batching()
// * render particles
// * call beg_batching() if batching was enabled before

// WHEN to use rend_particles?
// * no rotation needed
// * P_data - one common 'texCoords' vec / texture might be null
// * p_data_tCs - each particle have it's own 'texCoords' vec
// texture must not be null

// WHEN to use batching?
// * when rendering text
// * when rendering sprites with common texture && blending && sampling
// ...
// when not to:
// * I don't know
// don't change projection between beg_batching() - end_batching() block

class Renderer_2D
{
public:
    Renderer_2D();
    Renderer_2D(const Renderer_2D&) = delete;

    void beg_batching() const;
    void end_batching() const;

    void render(const Sprite& sprite) const;
    void render(const Text& text) const;

    void rend_particles(const P_data& p_data) const;
    void rend_particles(const P_data_tCs& p_data) const;

    // camera position + view range
    void load_projection(const glm::vec4& coords) const;
    // if you want to calculate projection matrix yourself
    void load_projection(const glm::mat4& matrix) const;

private:
    static bool isCurrent;

    mutable bool is_batching;

    VAO vao;
    BO vbo_static;
    Sampler sampler_linear, sampler_nearest;
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
        GLenum src_alpha;
        GLenum dst_alpha;
        const Texture* texture;
        Sampl_type sampl_type;
    };

    // members for batching system
    mutable std::vector<std::vector<Vbo_instance>> batches;
    mutable std::vector<Batch_state> b_states;
    BO vbo_dynamic;
    Shader shader_batching;

    // members for particle rendering
    VAO vao_p, vao_p_tCs;
    Shader shader_p, shader_p_tCs;

    void uniform_render(const Sprite& sprite) const;
    void uniform_render(const Text& text) const;
    void batch_render(const Sprite& sprite) const;
    void batch_render(const Text& text) const;

    void load_proj_impl(const glm::mat4& matrix) const;
};

#endif // RENDERER_2D_HPP
