#include <MITS/opengl/texture.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdexcept>

ID_UNIT Texture::bound_id{0, 0};

ID_UNIT Sampler::bound_id{0, 0};

Texture::Texture(const std::string& filename, bool sRGB, GLsizei levels):
    GL_Base([](GLuint id){glDeleteTextures(1, &id);})
{
    unsigned char* data = stbi_load(filename.c_str(), &size.x, &size.y, nullptr, 4);
    if(!data)
        throw std::runtime_error("stbi_load failed: " + filename);

    glGenTextures(1, &id);
    bind();

    GLenum internal_format;
    if(sRGB)
        internal_format = GL_SRGB8_ALPHA8;
    else
        internal_format = GL_RGBA8;

    glTexStorage2D(GL_TEXTURE_2D, levels, internal_format, size.x, size.y);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, data);

    if(levels > 1)
        glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

Texture::Texture(GLenum internal_format, GLsizei width, GLsizei height, GLsizei levels):
    GL_Base([](GLuint id){glDeleteTextures(1, &id);}),
    size(width, height)
{
    glGenTextures(1, &id);
    bind();
    glTexStorage2D(GL_TEXTURE_2D, levels, internal_format, width, height);
}

Texture::Texture():
    GL_Base([](GLuint id){glDeleteTextures(1, &id);})
{
    glGenTextures(1, &id);
    bind();
}

const glm::ivec2& Texture::getSize() const
{
    return size;
}

void Texture::bind(GLuint unit, GLenum target) const
{
    if(bound_id.id != id || bound_id.unit != unit)
    {
        bound_id.id = id;
        bound_id.unit = unit;
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(target, id);
    }
}

GLuint Texture::get_id() const
{
    return id;
}

Sampler::Sampler():
    GL_Base([](GLuint id){glDeleteSamplers(1, &id);})
{
    glGenSamplers(1, &id);
}

void Sampler::bind(GLuint unit) const
{
    if(bound_id.id != id || bound_id.unit != unit)
    {
        bound_id.id = id;
        bound_id.unit = unit;
        glBindSampler(unit, id);
    }
}

void Sampler::set_parameter_i(GLenum pname, GLint param) const
{
    glSamplerParameteri(id, pname, param);
}
