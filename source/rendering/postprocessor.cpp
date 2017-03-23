#include "postprocessor.hpp"
#include <assert.h>
#include "../app.hpp"
#include "../opengl/other.hpp"

class Wrp_blend
{
public:
    Wrp_blend()
    {glDisable(GL_BLEND);}
    ~Wrp_blend()
    {glEnable(GL_BLEND);}
    Wrp_blend(const Wrp_blend&) = delete;
    Wrp_blend& operator=(const Wrp_blend&) = delete;
};

bool Postprocessor::isCurrent = false;

Postprocessor::Postprocessor(const glm::ivec2& fbSize):
    finished(true),
    fbSize(0, 0),
    tex_type(GL_RGBA8),
    shader_blur("shaders/shader_fb.vert", "shaders/shader_fb_blur.frag", "", true, "fb_blur"),
    shader_blend("shaders/shader_fb.vert", "shaders/shader_fb_blend.frag", "", true, "fb_blend"),
    shader_final("shaders/shader_fb.vert", "shaders/shader_fb_final.frag", "", true, "fb_final")
{
    assert(!isCurrent);
    isCurrent = true;

    sampler.set_parameter_i(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    sampler.set_parameter_i(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    sampler.set_parameter_i(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    sampler.set_parameter_i(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLfloat vertices[] =
    {
        // pos....texCoord
        -1.f, -1.f, 0.f, 0.f,
        1.f, -1.f, 1.f, 0.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        -1.f, 1.f, 0.f, 1.f,
        -1.f, -1.f, 0.f, 0.f
    };

    vbo.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vao.bind();
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    fb_beg.bind();
    const GLenum attachments[] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1
    };
    glDrawBuffers(2, attachments);

    shader_blend.bind();
    glUniform1i(shader_blend.getUniLocation("base"), 0);
    glUniform1i(shader_blend.getUniLocation("blur"), 1);

    set_new_size(fbSize);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Postprocessor::set_new_size(const glm::ivec2& fbSize)
{
    if(this->fbSize == fbSize)
        return;

    this->fbSize = fbSize;

    fb_beg.bind();
    tex_base = std::make_unique<Texture>(tex_type, fbSize.x, fbSize.y);
    tex_bright = std::make_unique<Texture>(tex_type, fbSize.x, fbSize.y);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_base->get_id(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_bright->get_id(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    fb_pp1.bind();
    tex_pp1 = std::make_unique<Texture>(tex_type, fbSize.x / 4, fbSize.y / 4);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_pp1->get_id(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    fb_pp2.bind();
    tex_pp2 = std::make_unique<Texture>(tex_type, fbSize.x / 4, fbSize.y / 4);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_pp2->get_id(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    fb_custom_1.bind();
    tex_custom_1 = std::make_unique<Texture>(tex_type, fbSize.x, fbSize.y);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_custom_1->get_id(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    fb_custom_2.bind();
    tex_custom_2 = std::make_unique<Texture>(tex_type, fbSize.x, fbSize.y);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_custom_2->get_id(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Postprocessor::set_new_scene(const Scene_coords& coords)
{this->scene_coords = coords;}

void Postprocessor::begRender() const
{
    finished = false;

    fb_pp1.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    fb_pp2.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    fb_custom_2.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    fb_custom_1.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    fb_beg.bind();
    glClear(GL_COLOR_BUFFER_BIT);
}

void Postprocessor::endRender(int num_blurs) const
{
    Wrp_blend wrp_blend;
    (void)wrp_blend;

    // blur
    Scissor::set_scaled(scene_coords, fbSize, tex_pp1->getSize());
    Viewport::set(0, 0, tex_pp1->getSize().x, tex_pp1->getSize().y);

    vao.bind();
    shader_blur.bind();
    sampler.bind();

    bool horizontal_pass = true, first_iter = true;

    for(int i = 0; i < num_blurs * 2; ++i)
    {
        if(horizontal_pass)
        {
            fb_pp1.bind();
            glUniform1f(shader_blur.getUniLocation("horizontal"), true);

            if(first_iter)
            {
                first_iter = false;
                tex_bright->bind();
            }
            else
                tex_pp2->bind();
        }
        else
        {
            fb_pp2.bind();
            glUniform1f(shader_blur.getUniLocation("horizontal"), false);
            tex_pp1->bind();
        }

        horizontal_pass = !horizontal_pass;
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // blend
    Scissor::set(scene_coords);
    Viewport::set(0, 0, tex_custom_1->getSize().x, tex_custom_1->getSize().y);

    fb_custom_1.bind();
    next_tex_custom_bind = 1;
    shader_blend.bind();
    sampler.bind(1);
    tex_base->bind(0);
    tex_pp2->bind(1);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    Viewport::set(scene_coords);
}

void Postprocessor::apply_effect(const Shader& shader) const
{
    Wrp_blend wrp_blend;
    (void)wrp_blend;

    // scissor already set by endRender()
    Viewport::set(0, 0, tex_custom_1->getSize().x, tex_custom_1->getSize().y);

    vao.bind();
    shader.bind();
    sampler.bind();

    if(next_tex_custom_bind == 1)
    {
        tex_custom_1->bind();
        fb_custom_2.bind();
        next_tex_custom_bind = 2;
    }
    else
    {
        tex_custom_2->bind();
        fb_custom_1.bind();
        next_tex_custom_bind = 1;
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);

    Viewport::set(scene_coords);
}

void Postprocessor::render_fb0() const
{
    // scissor already set by endRender()
    Viewport::set(0, 0, fbSize.x, fbSize.y);
    // GL_ONE is because we don't want to blend scene twice
    // (when rendering sprites to scene and when rendering scene to fb0)
    Blend_alpha::set(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    vao.bind();
    shader_final.bind();
    sampler.bind();

    if(next_tex_custom_bind == 1)
        tex_custom_1->bind();
    else
        tex_custom_2->bind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    finished = true;
    Viewport::set(scene_coords);
    // set scissor as it was before begRender()
    Scissor::set(0, 0, fbSize.x, fbSize.y);
}
