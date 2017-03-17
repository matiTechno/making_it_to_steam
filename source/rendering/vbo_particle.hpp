#ifndef VBO_PARTICLE_HPP
#define VBO_PARTICLE_HPP

#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include "../rendering/render_obj_base.hpp"
class Texture;

// tCs = texCoords

struct Vbo_p
{
    // when uploading to gpu merge into vec4
    glm::vec2 pos;
    glm::vec2 size;
    glm::vec4 color;

};

struct Vbo_p_tCs: public Vbo_p
{
    // float not int for shader possible optimization
    glm::vec4 texCoords;
    bool bloom;
};

struct P_data_base
{
    Texture* texture = nullptr;
    GLenum blend_sfactor = GL_SRC_ALPHA, blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
    Sampl_type sampl_type = Sampl_type::linear;
    std::size_t num_to_render;
};

struct P_data: public P_data_base
{
    glm::ivec4 texCoords;
    bool bloom = false;
    std::vector<Vbo_p> vbo_data;
};

struct P_data_tCs: public P_data_base
{
    std::vector<Vbo_p_tCs> vbo_data;
};

#endif // VBO_PARTICLE_HPP
