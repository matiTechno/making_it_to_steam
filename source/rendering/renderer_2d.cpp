#include "renderer_2d.hpp"
#include <assert.h>
#include "sprite.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "text.hpp"
#include "vbo_particle.hpp"

bool Renderer_2D::isCurrent = false;

Renderer_2D::Renderer_2D():
    is_batching(false),
    blend_sfactor(GL_ONE),
    blend_dfactor(GL_ZERO),
    shader_uniform("shaders/shader_2d.vert", "shaders/shader_2d.frag", "", true, "renderer_2D"),
    shader_batching("shaders/shader_2d_batching.vert", "shaders/shader_2d_batching.frag", "", true,
                    "renderer_2D_batching"),
    shader_p("shaders/shader_2d_p.vert", "shaders/shader_2d_p.frag", "", true,
             "renderer_2D_particles"),
    shader_p_tCs("shaders/shader_2d_p_tCs.vert", "shaders/shader_2d_p_tCs.frag", "", true,
                 "renderer_2D_particles_texCoords")
{
    assert(!isCurrent);
    isCurrent = true;

    sampler_linear.set_parameter_i(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    sampler_linear.set_parameter_i(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    sampler_linear.set_parameter_i(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    sampler_linear.set_parameter_i(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    sampler_nearest.set_parameter_i(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    sampler_nearest.set_parameter_i(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    sampler_nearest.set_parameter_i(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    sampler_nearest.set_parameter_i(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    vao.bind();

    GLfloat vertices[] =
    {
        // pos....texCoord
        0.f, 0.f, 0.f, 0.f,
        1.f, 0.f, 1.f, 0.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        0.f, 1.f, 0.f, 1.f,
        0.f, 0.f, 0.f, 0.f
    };

    vbo_static.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // vao configuration for batching system
    {
        vbo_dynamic.bind(GL_ARRAY_BUFFER);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vbo_instance), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vbo_instance),
                              reinterpret_cast<const void*>(sizeof(glm::vec4)));
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1);

        glVertexAttribIPointer(3, 2, GL_INT, sizeof(Vbo_instance),
                               reinterpret_cast<const void*>(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(3);
        glVertexAttribDivisor(3, 1);

        // attributes for model matrix...
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
        glEnableVertexAttribArray(7);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);

        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vbo_instance),
                              reinterpret_cast<const void*>(2 * sizeof(glm::vec4) + sizeof(glm::ivec2)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vbo_instance),
                              reinterpret_cast<const void*>(3 * sizeof(glm::vec4) + sizeof(glm::ivec2)));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vbo_instance),
                              reinterpret_cast<const void*>(4 * sizeof(glm::vec4) + sizeof(glm::ivec2)));
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vbo_instance),
                              reinterpret_cast<const void*>(5 * sizeof(glm::vec4) + sizeof(glm::ivec2)));
    }
    // vao_p
    {
        vao_p.bind();
        vbo_static.bind(GL_ARRAY_BUFFER);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        vbo_dynamic.bind(GL_ARRAY_BUFFER);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vbo_p), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vbo_p),
                              reinterpret_cast<const void*>(sizeof(glm::vec4)));
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1);
    }
    // vao_p_texCoords
    {
        vao_p_tCs.bind();
        vbo_static.bind(GL_ARRAY_BUFFER);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        vbo_dynamic.bind(GL_ARRAY_BUFFER);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vbo_p_tCs), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vbo_p_tCs),
                              reinterpret_cast<const void*>(sizeof(glm::vec4)));
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1);

        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vbo_p_tCs),
                              reinterpret_cast<const void*>(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(3);
        glVertexAttribDivisor(3, 1);

        glVertexAttribIPointer(4, 1, GL_INT, sizeof(Vbo_p_tCs),
                               reinterpret_cast<const void*>(3 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(4);
        glVertexAttribDivisor(4, 1);
    }
}

void Renderer_2D::beg_batching() const
{
    is_batching = true;
}

void Renderer_2D::end_batching() const
{
    if(batches.empty())
        return;

    shader_batching.bind();
    vao.bind();

    auto state_it = b_states.begin();
    for(auto& batch: batches)
    {
        if(state_it->texture)
        {
            state_it->texture->bind();

            if(state_it->sampl_type == Sampl_type::linear)
                sampler_linear.bind();
            else
                sampler_nearest.bind();
        }

        set_blend_func(state_it->blend_sfactor, state_it->blend_dfactor);

        ++state_it;

        vbo_dynamic.bind(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(Vbo_instance) * batch.size()),
                     batch.data(), GL_STREAM_DRAW);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(batch.size()));
    }

    batches.clear();
    b_states.clear();
    is_batching = false;
}

void Renderer_2D::render(const Sprite& sprite) const
{
    if(is_batching)
        batch_render(sprite);
    else
        uniform_render(sprite);
}

void Renderer_2D::render(const Text& text) const
{
    if(is_batching)
        batch_render(text);
    else
        uniform_render(text);
}

void Renderer_2D::load_projection(float left, float right, float top, float bottom) const
{
    assert(left < right);
    assert(top < bottom);
    glm::mat4 matrix = glm::ortho(left, right, bottom, top);
    load_proj_impl(matrix);
}

void Renderer_2D::load_projection(const glm::mat4& matrix) const
{
    load_proj_impl(matrix);
}

void Renderer_2D::load_proj_impl(const glm::mat4& matrix) const
{
    shader_uniform.bind();
    glUniformMatrix4fv(shader_uniform.getUniLocation("projection"), 1, GL_FALSE, &matrix[0][0]);
    glUniformMatrix4fv(shader_uniform.getUniLocation("projection"), 1, GL_FALSE, &matrix[0][0]);

    shader_batching.bind();
    glUniformMatrix4fv(shader_batching.getUniLocation("projection"), 1, GL_FALSE, &matrix[0][0]);
    glUniformMatrix4fv(shader_batching.getUniLocation("projection"), 1, GL_FALSE, &matrix[0][0]);

    shader_p.bind();
    glUniformMatrix4fv(shader_p.getUniLocation("projection"), 1, GL_FALSE, &matrix[0][0]);
    glUniformMatrix4fv(shader_p.getUniLocation("projection"), 1, GL_FALSE, &matrix[0][0]);

    shader_p_tCs.bind();
    glUniformMatrix4fv(shader_p_tCs.getUniLocation("projection"), 1, GL_FALSE, &matrix[0][0]);
    glUniformMatrix4fv(shader_p_tCs.getUniLocation("projection"), 1, GL_FALSE, &matrix[0][0]);
}

void Renderer_2D::set_blend_func(GLenum sfactor, GLenum dfactor) const
{
    if(blend_sfactor != sfactor || blend_dfactor != dfactor)
    {
        blend_sfactor = sfactor;
        blend_dfactor = dfactor;
        glBlendFunc(blend_sfactor, blend_dfactor);
    }
}

void Renderer_2D::uniform_render(const Sprite& sprite) const
{
    set_blend_func(sprite.blend_sfactor, sprite.blend_dfactor);
    vao.bind();
    shader_uniform.bind();

    if(sprite.texture)
    {
        if(sprite.sampl_type == Sampl_type::linear)
            sampler_linear.bind();
        else
            sampler_nearest.bind();

        sprite.texture->bind();
        glUniform1i(shader_uniform.getUniLocation("type"), 0);

        auto& coords = sprite.texCoords;
        auto texSize = sprite.texture->getSize();

        glm::vec2 texSize_gl(static_cast<float>(coords.z) / static_cast<float>(texSize.x),
                             static_cast<float>(coords.w) / static_cast<float>(texSize.y));
        glm::vec2 texShift(static_cast<float>(coords.x) / static_cast<float>(texSize.x),
                           static_cast<float>(coords.y) / static_cast<float>(texSize.y));

        glUniform2f(shader_uniform.getUniLocation("texSize"), texSize_gl.x, texSize_gl.y);
        glUniform2f(shader_uniform.getUniLocation("texShift"), texShift.x, texShift.y);

    }
    else
        glUniform1i(shader_uniform.getUniLocation("type"), 1);

    glm::mat4 model(1.f);
    model = glm::translate(model, glm::vec3(sprite.position, 0.f));
    if(sprite.rotation != 0.f)
    {
        model = glm::translate(model, glm::vec3(sprite.rotation_point, 0.f));
        model = glm::rotate(model, sprite.rotation, glm::vec3(0.f, 0.f, -1.f));
        model = glm::translate(model, glm::vec3(-sprite.rotation_point, 0.f));
    }
    model = glm::scale(model, glm::vec3(sprite.size, 1.f));

    glUniformMatrix4fv(shader_uniform.getUniLocation("model"), 1, GL_FALSE, &model[0][0]);

    auto& col = sprite.color;
    glUniform4f(shader_uniform.getUniLocation("spriteColor"), col.x, col.y, col.z, col.w);

    if(sprite.bloom)
        glUniform1f(shader_uniform.getUniLocation("isBloom"), true);
    else
        glUniform1f(shader_uniform.getUniLocation("isBloom"), false);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer_2D::uniform_render(const Text& text) const
{
    set_blend_func(text.blend_sfactor, text.blend_dfactor);
    vao.bind();
    shader_uniform.bind();

    if(!text.render_quads)
    {
        if(text.sampl_type == Sampl_type::linear)
            sampler_linear.bind();
        else
            sampler_nearest.bind();

        text.font->atlas.bind();
        glUniform1i(shader_uniform.getUniLocation("type"), 2);
    }
    else
        glUniform1i(shader_uniform.getUniLocation("type"), 1);

    if(text.bloom)
        glUniform1f(shader_uniform.getUniLocation("isBloom"), true);
    else
        glUniform1f(shader_uniform.getUniLocation("isBloom"), false);

    auto& col = text.color;
    glUniform4f(shader_uniform.getUniLocation("spriteColor"), col.x, col.y, col.z, col.w);

    glm::vec2 pen_pos = text.position;

    bool first_char_in_line = true;

    for(auto c: text.text)
    {
        if(c == '\n')
        {
            pen_pos.x = text.position.x;
            pen_pos.y += static_cast<float>(text.font->line_space) * text.scale;
            first_char_in_line = true;
            continue;
        }

        assert(c >= 32 && c < 127);

        auto& this_char = text.font->chars.at(c);

        // texture coordinates
        if(!text.render_quads)
        {
            auto& coords = this_char.texCoords;
            auto texSize = text.font->atlas.getSize();

            glm::vec2 texSize_gl(static_cast<float>(coords.z) / static_cast<float>(texSize.x),
                                 static_cast<float>(coords.w) / static_cast<float>(texSize.y));
            glm::vec2 texShift(static_cast<float>(coords.x) / static_cast<float>(texSize.x),
                               static_cast<float>(coords.y) / static_cast<float>(texSize.y));

            glUniform2f(shader_uniform.getUniLocation("texSize"), texSize_gl.x, texSize_gl.y);
            glUniform2f(shader_uniform.getUniLocation("texShift"), texShift.x, texShift.y);
        }
        // model matrix
        {
            glm::vec2 glyph_size(this_char.texCoords.z * text.scale, this_char.texCoords.w * text.scale);
            glm::vec2 glyph_pos;

            if(first_char_in_line)
            {
                glyph_pos = glm::vec2(pen_pos.x,
                                      pen_pos.y + static_cast<float>(text.font->max_bearing_y - this_char.bearing.y)
                                      * text.scale);

                first_char_in_line = false;
            }
            else
            {
                glyph_pos = glm::vec2(pen_pos.x + static_cast<float>(this_char.bearing.x) * text.scale,
                                      pen_pos.y + static_cast<float>(text.font->max_bearing_y - this_char.bearing.y)
                                      * text.scale);
            }

            glm::mat4 model(1.f);
            model = glm::translate(model, glm::vec3(glyph_pos, 0.f));
            if(text.rotation != 0.f)
            {
                model = glm::translate(model, glm::vec3(text.rotation_point - (glyph_pos - text.position), 0.f));
                model = glm::rotate(model, text.rotation, glm::vec3(0.f, 0.f, -1.f));
                model = glm::translate(model, glm::vec3(-text.rotation_point + (glyph_pos - text.position), 0.f));
            }
            model = glm::scale(model, glm::vec3(glyph_size, 1.f));

            glUniformMatrix4fv(shader_uniform.getUniLocation("model"), 1, GL_FALSE, &model[0][0]);
        }

        pen_pos.x += static_cast<float>(this_char.advance) * text.scale;
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void Renderer_2D::batch_render(const Sprite& sprite) const
{
    if(batches.size())
    {
        if(sprite.blend_sfactor != b_states.back().blend_sfactor ||
                sprite.blend_dfactor != b_states.back().blend_dfactor ||
                (sprite.texture != b_states.back().texture && sprite.texture != nullptr) ||
                (sprite.sampl_type != b_states.back().sampl_type && sprite.texture != nullptr))
        {
            b_states.push_back({sprite.blend_sfactor, sprite.blend_dfactor,
                                sprite.texture, sprite.sampl_type});
            batches.emplace_back();
        }
    }
    else
    {
        b_states.push_back({sprite.blend_sfactor, sprite.blend_dfactor,
                            sprite.texture, sprite.sampl_type});
        batches.emplace_back();
    }

    batches.back().emplace_back();
    Vbo_instance& instance = batches.back().back();

    if(sprite.texture)
    {
        instance.bloom_type.y = 0;

        auto& coords = sprite.texCoords;
        auto texSize = sprite.texture->getSize();

        glm::vec2 texSize_gl(static_cast<float>(coords.z) / static_cast<float>(texSize.x),
                             static_cast<float>(coords.w) / static_cast<float>(texSize.y));
        glm::vec2 texShift(static_cast<float>(coords.x) / static_cast<float>(texSize.x),
                           static_cast<float>(coords.y) / static_cast<float>(texSize.y));

        instance.texData = glm::vec4(texSize_gl, texShift);
    }
    else
        instance.bloom_type.y = 1;

    glm::mat4 model(1.f);
    model = glm::translate(model, glm::vec3(sprite.position, 0.f));
    if(sprite.rotation != 0.f)
    {
        model = glm::translate(model, glm::vec3(sprite.rotation_point, 0.f));
        model = glm::rotate(model, sprite.rotation, glm::vec3(0.f, 0.f, -1.f));
        model = glm::translate(model, glm::vec3(-sprite.rotation_point, 0.f));
    }
    model = glm::scale(model, glm::vec3(sprite.size, 1.f));
    instance.model = std::move(model);

    instance.color = sprite.color;

    if(sprite.bloom)
        instance.bloom_type.x = 1;
    else
        instance.bloom_type.x = 0;
}

void Renderer_2D::batch_render(const Text& text) const
{
    Texture* temp = nullptr;
    if(!text.render_quads)
        temp = &text.font->atlas;

    if(batches.size())
    {
        if(text.blend_sfactor != b_states.back().blend_sfactor ||
                text.blend_dfactor != b_states.back().blend_dfactor ||
                (&text.font->atlas != b_states.back().texture && !text.render_quads) ||
                (text.sampl_type != b_states.back().sampl_type && !text.render_quads))
        {
            b_states.push_back({text.blend_sfactor, text.blend_dfactor,
                                temp, text.sampl_type});
            batches.emplace_back();
        }
    }
    else
    {
        b_states.push_back({text.blend_sfactor, text.blend_dfactor,
                            temp, text.sampl_type});
        batches.emplace_back();
    }

    glm::vec2 pen_pos = text.position;

    bool first_char_in_line = true;

    for(auto c: text.text)
    {
        if(c == '\n')
        {
            pen_pos.x = text.position.x;
            pen_pos.y += static_cast<float>(text.font->line_space) * text.scale;
            first_char_in_line = true;
            continue;
        }

        assert(c >= 32 && c < 127);

        batches.back().emplace_back();
        Vbo_instance& instance = batches.back().back();

        instance.color = text.color;

        if(text.bloom)
            instance.bloom_type.x = 1;
        else
            instance.bloom_type.x = 0;

        auto& this_char = text.font->chars.at(c);

        // texData + type
        if(!text.render_quads)
        {
            instance.bloom_type.y = 2;

            auto& coords = this_char.texCoords;
            auto texSize = text.font->atlas.getSize();

            glm::vec2 texSize_gl(static_cast<float>(coords.z) / static_cast<float>(texSize.x),
                                 static_cast<float>(coords.w) / static_cast<float>(texSize.y));
            glm::vec2 texShift(static_cast<float>(coords.x) / static_cast<float>(texSize.x),
                               static_cast<float>(coords.y) / static_cast<float>(texSize.y));
            instance.texData = glm::vec4(texSize_gl, texShift);
        }
        else
            instance.bloom_type.y = 1;

        // model matrix
        {
            glm::vec2 glyph_size(this_char.texCoords.z * text.scale, this_char.texCoords.w * text.scale);
            glm::vec2 glyph_pos;

            if(first_char_in_line)
            {
                glyph_pos = glm::vec2(pen_pos.x,
                                      pen_pos.y + static_cast<float>(text.font->max_bearing_y - this_char.bearing.y)
                                      * text.scale);

                first_char_in_line = false;
            }
            else
            {
                glyph_pos = glm::vec2(pen_pos.x + static_cast<float>(this_char.bearing.x) * text.scale,
                                      pen_pos.y + static_cast<float>(text.font->max_bearing_y - this_char.bearing.y)
                                      * text.scale);
            }

            glm::mat4 model(1.f);
            model = glm::translate(model, glm::vec3(glyph_pos, 0.f));
            if(text.rotation != 0.f)
            {
                model = glm::translate(model, glm::vec3(text.rotation_point - (glyph_pos - text.position), 0.f));
                model = glm::rotate(model, text.rotation, glm::vec3(0.f, 0.f, -1.f));
                model = glm::translate(model, glm::vec3(-text.rotation_point + (glyph_pos - text.position), 0.f));
            }
            model = glm::scale(model, glm::vec3(glyph_size, 1.f));
            instance.model = std::move(model);
        }

        pen_pos.x += static_cast<float>(this_char.advance) * text.scale;
    }
}

void Renderer_2D::rend_particles(const P_data& p_data) const
{
    bool was_batching = is_batching;
    end_batching();

    vao_p.bind();

    shader_p.bind();

    set_blend_func(p_data.blend_sfactor, p_data.blend_dfactor);

    if(p_data.texture)
    {
        if(p_data.sampl_type == Sampl_type::linear)
            sampler_linear.bind();
        else
            sampler_nearest.bind();

        p_data.texture->bind();

        glUniform1i(shader_p.getUniLocation("type"), 0);

        auto& coords = p_data.texCoords;
        auto texSize = p_data.texture->getSize();

        glm::vec2 texSize_gl(static_cast<float>(coords.z) / static_cast<float>(texSize.x),
                             static_cast<float>(coords.w) / static_cast<float>(texSize.y));
        glm::vec2 texShift(static_cast<float>(coords.x) / static_cast<float>(texSize.x),
                           static_cast<float>(coords.y) / static_cast<float>(texSize.y));

        glUniform2f(shader_p.getUniLocation("texSize"), texSize_gl.x, texSize_gl.y);
        glUniform2f(shader_p.getUniLocation("texShift"), texShift.x, texShift.y);

    }
    else
        glUniform1i(shader_p.getUniLocation("type"), 1);

    if(p_data.bloom)
        glUniform1i(shader_p.getUniLocation("isBloom"), 1);
    else
        glUniform1i(shader_p.getUniLocation("isBloom"), 0);

    vbo_dynamic.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(Vbo_p) * p_data.num_to_render),
                 p_data.vbo_data.data(), GL_STREAM_DRAW);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(p_data.num_to_render));

    if(was_batching)
        is_batching = true;
}

void Renderer_2D::rend_particles(const P_data_tCs& p_data) const
{
    assert(p_data.texture);

    bool was_batching = is_batching;
    end_batching();

    vao_p_tCs.bind();

    shader_p_tCs.bind();

    set_blend_func(p_data.blend_sfactor, p_data.blend_dfactor);

    if(p_data.sampl_type == Sampl_type::linear)
        sampler_linear.bind();
    else
        sampler_nearest.bind();

    p_data.texture->bind();

    glUniform2f(shader_p_tCs.getUniLocation("texSize"), static_cast<float>(p_data.texture->getSize().x),
                static_cast<float>(p_data.texture->getSize().y));

    vbo_dynamic.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(Vbo_p_tCs) * p_data.num_to_render),
                 p_data.vbo_data.data(), GL_STREAM_DRAW);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(p_data.num_to_render));

    if(was_batching)
        is_batching = true;
}
