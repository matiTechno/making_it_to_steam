#include "postprocessor.hpp"
#include <assert.h>

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

Postprocessor::Postprocessor(int width, int height):
    finished(true),
    fbSize(0, 0),
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

    set_new_size(width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Postprocessor::set_new_size(int width, int height)
{
    if(fbSize.x == width && fbSize.y == height)
        return;

    fbSize = glm::ivec2(width, height);
    int tex_type = GL_RGBA8;

    fb_beg.bind();
    tex_base = std::make_unique<Texture>(tex_type, width, height);
    tex_bright = std::make_unique<Texture>(tex_type, width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_base->get_id(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_bright->get_id(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    fb_pp1.bind();
    tex_pp1 = std::make_unique<Texture>(tex_type, width / 2, height / 2);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_pp1->get_id(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    fb_pp2.bind();
    tex_pp2 = std::make_unique<Texture>(tex_type, width / 2, height / 2);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_pp2->get_id(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    fb_custom_1.bind();
    tex_custom_1 = std::make_unique<Texture>(tex_type, width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_custom_1->get_id(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    fb_custom_2.bind();
    tex_custom_2 = std::make_unique<Texture>(tex_type, width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_custom_2->get_id(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Postprocessor::begRender() const
{
    fb_beg.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    finished = false;
}

void Postprocessor::endRender(int num_blurs) const
{
    Wrp_blend wrp_blend;
    (void)wrp_blend;

    // blur
    vao.bind();
    shader_blur.bind();
    sampler.bind();

    glViewport(0, 0, tex_pp1->getSize().x, tex_pp1->getSize().y);
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
    glViewport(0, 0, fbSize.x, fbSize.y);

    // blend
    fb_custom_1.bind();
    next_tex_custom_bind = 1;

    vao.bind();
    shader_blend.bind();

    sampler.bind(1);
    tex_base->bind(0);
    tex_pp2->bind(1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Postprocessor::apply_effect(const Shader& shader) const
{
    Wrp_blend wrp_blend;
    (void)wrp_blend;

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
}

void Postprocessor::render() const
{
    Wrp_blend wrp_blend;
    (void)wrp_blend;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    vao.bind();
    shader_final.bind();
    sampler.bind();

    if(next_tex_custom_bind == 1)
        tex_custom_1->bind();
    else
        tex_custom_2->bind();

    glDrawArrays(GL_TRIANGLES, 0, 6);

    finished = true;
}
