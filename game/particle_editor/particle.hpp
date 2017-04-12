#include <MITS/rendering/vbo_particle.hpp>
#include <glm/exponential.hpp>

struct Particle: Vbo_p_tCs
{
    glm::vec2 acceleration;
    glm::vec2 velocity;
    float life;

    void update(float dt)
    {
        pos += velocity * dt + 0.5f * acceleration * glm::pow(dt, 2.f);
        velocity += acceleration * dt;
    }
};

struct P_ranges
{
    glm::vec2 size_low;
    glm::vec2 size_high;
    glm::vec4 color_low;
    glm::vec4 color_high;
    float life_low;
    float life_high;
    glm::vec2 vel_low;
    glm::vec2 vel_high;
    glm::vec2 acc_low;
    glm::vec2 acc_high;
};
