#ifndef BUFFERS_HPP
#define BUFFERS_HPP

#include "base.hpp"

// vertex array object
class VAO: public GL_Base
{
public:
    VAO();

    void bind() const;

private:
    static GLuint bound_id;
};

// buffer object
class BO: public GL_Base
{
public:
    BO();

    void bind(GLenum target) const;
};

// framebuffer object
class FBO: public GL_Base
{
public:
    FBO();

    void bind(GLenum target = GL_FRAMEBUFFER) const;
};

// renderbuffer object
class RBO: public GL_Base
{
public:
    RBO();

    void bind() const;

    GLuint get_id() const;
};

#endif // BUFFERS_HPP
