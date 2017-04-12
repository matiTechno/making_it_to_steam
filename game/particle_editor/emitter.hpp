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

    float life;
    bool always_active = false;
    bool pos_in_center;
    glm::vec2 position;
    glm::vec2 size;
    bool circle_spawn = false;
    bool quads = true;
    float spawn_time;
    P_ranges ranges;
    bool bloom = false;

    GLenum &src_alpha;
    GLenum &dst_alpha;
    Sampl_type &sampl_type;

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
