#include "emitter.hpp"
#include <MITS/opengl/texture.hpp>

Emitter::Emitter(std::size_t reserve, const Texture& texture):
    src_alpha(render_data.src_alpha),
    dst_alpha(render_data.dst_alpha),
    sampl_type(render_data.sampl_type)
{
    render_data.vbo_data.reserve(reserve);
    particles.reserve(reserve);
    render_data.texture = &texture;

    std::random_device rd;
    rn_eng.seed(rd());

    texCoords = glm::ivec4(0, 0, texture.getSize());
}

void Emitter::update(float dt)
{
    for(std::size_t i = 0; i < static_cast<std::size_t>(last_active) + 1; ++i)
    {
        if((particles[i].life -= dt) < 0)
            kill_particle(particles[i]);
        else
            particles[i].update(dt);
    }

    accumulator += dt;
    life -= dt;

    while(accumulator > spawn_time)
    {
        spawn_particle();
        accumulator -= spawn_time;
    }

    for(std::size_t i = 0; i < static_cast<std::size_t>(last_active) + 1; ++i)
        render_data.vbo_data[i] = static_cast<Vbo_p_tCs>(particles[i]);

    render_data.num_to_render = static_cast<std::size_t>(last_active + 1);
}

void Emitter::spawn_particle()
{
    Particle particle;

    particle.bloom = bloom;
    particle.texCoords = texCoords;

    std::uniform_real_distribution<float> uni;
    if(pos_is_center)
        uni = std::uniform_real_distribution<float>(-0.5f, 0.5f);
    else
        uni = std::uniform_real_distribution<float>(0, 1.f);

    particle.pos.x = position.x + uni(rn_eng) * size.x;

    if(circle_spawn)
    {
        float y_range;
        if(pos_is_center)
            y_range = glm::sqrt(glm::pow(size.x / 2.f, 2.f) - glm::pow(particle.pos.x - position.x, 2.f));
        else
            y_range = glm::sqrt(glm::pow(size.x / 2.f, 2.f) - glm::pow(particle.pos.x - position.x - size.x / 2.f, 2.f));

        std::uniform_real_distribution<float> uni_y(-y_range, y_range);
        particle.pos.y = uni_y(rn_eng) + position.y;
    }
    else
        particle.pos.y = position.y + uni(rn_eng) * size.y;

    std::uniform_real_distribution<float> s_x(ranges.size_low.x, ranges.size_high.x);
    if(quads)
        particle.size = glm::vec2(s_x(rn_eng));
    else
    {
        particle.size.x = s_x(rn_eng);
        std::uniform_real_distribution<float> s_y(ranges.size_low.y, ranges.size_high.y);
        particle.size.y = s_y(rn_eng);
    }

    particle.pos -= particle.size / 2.f;

    std::uniform_real_distribution<float> r(ranges.color_low.r, ranges.color_low.r);
    std::uniform_real_distribution<float> g(ranges.color_low.g, ranges.color_low.g);
    std::uniform_real_distribution<float> b(ranges.color_low.b, ranges.color_low.b);
    std::uniform_real_distribution<float> a(ranges.color_low.a, ranges.color_low.a);
    particle.color = glm::vec4(r(rn_eng), g(rn_eng), b(rn_eng), a(rn_eng));

    std::uniform_real_distribution<float> life(ranges.life_low, ranges.life_high);
    particle.life = life(rn_eng);

    std::uniform_real_distribution<float> vel_x(ranges.vel_low.x, ranges.vel_high.x);
    particle.velocity.x = vel_x(rn_eng);
    std::uniform_real_distribution<float> vel_y(ranges.vel_low.y, ranges.vel_high.y);
    particle.velocity.y = vel_y(rn_eng);

    std::uniform_real_distribution<float> acc_x(ranges.acc_low.x, ranges.acc_high.x);
    particle.acceleration.x = acc_x(rn_eng);
    std::uniform_real_distribution<float> acc_y(ranges.acc_low.y, ranges.acc_high.y);
    particle.acceleration.y = acc_y(rn_eng);

    if(static_cast<std::size_t>(last_active) + 1 == particles.size())
    {
        render_data.vbo_data.emplace_back();
        particles.push_back(std::move(particle));
    }
    else
        particles[static_cast<std::size_t>(last_active + 1)] = std::move(particle);
    ++last_active;
}

void Emitter::kill_particle(Particle& particle)
{
    std::swap(particle, particles[static_cast<std::size_t>(last_active)]);
    --last_active;
}

bool Emitter::is_active()
{
    if(always_active)
        return true;
    return life > 0;
}

const P_data_tCs& Emitter::get_render_data()
{
    return render_data;
}
