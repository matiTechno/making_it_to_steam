#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "base.hpp"
#include <string>
#include <glm/vec2.hpp>
#include <utility>

struct ID_UNIT
{
    GLuint id;
    GLuint unit;
};

class Texture: public GL_Base
{
public:
    // for 1 and 2 target is GL_TEXTURE_2D
    // 2 - to update texture call glTexSubImage2D
    // 3 - generates id and calls bind, nothing more
    /*1*/Texture(const std::string& filename, bool sRGB = false, GLsizei levels = 1);
    /*2*/Texture(GLenum internal_format, GLsizei width, GLsizei height, GLsizei levels = 1);
    /*3*/Texture();

    const glm::ivec2& getSize() const;
    void bind(GLuint unit = 0, GLenum target = GL_TEXTURE_2D) const;

    GLuint get_id() const;

private:
    glm::ivec2 size;
    static ID_UNIT bound_id;
};

class Sampler: public GL_Base
{
public:
    Sampler();

    void bind(GLuint unit = 0) const;
    void set_parameter_i(GLenum pname, GLint param) const;

private:
    static ID_UNIT bound_id;
};

#endif // TEXTURE_HPP
