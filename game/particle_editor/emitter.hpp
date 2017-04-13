#include <MITS/rendering/vbo_particle.hpp>
#include "particle.hpp"
#include <random>

class Emitter
{
public:
    Emitter(std::size_t reserve, const Texture& texture);

    void update(float dt);

    bool is_active();

    const P_data_tCs& get_render_data();

    glm::vec2 position;
    glm::vec2 size;
    bool pos_is_center;
    bool circle_spawn;
    bool quads;
    float life;
    bool always_active = false;
    float spawn_time;
    bool bloom;
    GLenum &src_alpha; // GL_SRC_ALPHA
    GLenum &dst_alpha; // GL_ONE_MINUS_SRC_ALPHA
    Sampl_type &sampl_type; // linear
    P_ranges ranges;

private:
    P_data_tCs render_data;
    std::vector<Particle> particles;
    float accumulator = 0;
    int last_active = -1;
    std::mt19937 rn_eng;
    glm::ivec4 texCoords;

    void spawn_particle();
    void kill_particle(Particle& particle);
};
