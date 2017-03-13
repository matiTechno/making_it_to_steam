#include "buffers.hpp"

GLuint VAO::bound_id = 0;

VAO::VAO():
    GL_Base([](GLuint id){glDeleteVertexArrays(1, &id);})
{
    glGenVertexArrays(1, &id);
}

void VAO::bind() const
{
    if(bound_id != id)
    {
        bound_id = id;
        glBindVertexArray(id);
    }
}

BO::BO():
    GL_Base([](GLuint id){glDeleteBuffers(1, &id);})
{
    glGenBuffers(1, &id);
}

void BO::bind(GLenum target) const
{
    glBindBuffer(target, id);
}

FBO::FBO():
    GL_Base([](GLuint id){glDeleteFramebuffers(1, &id);})
{
    glGenFramebuffers(1, &id);
}

void FBO::bind(GLenum target) const
{
    glBindFramebuffer(target, id);
}

RBO::RBO():
    GL_Base([](GLuint id){glDeleteRenderbuffers(1, &id);})
{
    glGenRenderbuffers(1, &id);
}

void RBO::bind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, id);
}

GLuint RBO::get_id() const
{
    return id;
}
