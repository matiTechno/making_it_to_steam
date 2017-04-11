#include <MITS/rendering/vbo_particle.hpp>

struct Particle: Vbo_p_tCs
{
    glm::vec2 acceleration;
    glm::vec2 velocity;
    float life;
};
