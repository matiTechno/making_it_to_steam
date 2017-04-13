#include "particle_editor.hpp"
#include "emitter.hpp"

Particle_editor::~Particle_editor() = default;

Particle_editor::Particle_editor():
    texture("res/particle.png")
{
    emitter = std::make_unique<Emitter>(10000, texture);
    emitter->position = coords.size / 2 + glm::ivec2(0, 200);
    emitter->size = glm::vec2(100.f, 10.f);
    emitter->pos_is_center = true;
    emitter->circle_spawn = false;
    emitter->quads = true;
    emitter->always_active = true;
    emitter->spawn_time = 0.001f;
    emitter->bloom = false;
    emitter->dst_alpha = GL_ONE;
    emitter->ranges.size_low = glm::vec2(3.f);
    emitter->ranges.size_high = glm::vec2(10.f);
    emitter->ranges.color_low = glm::vec4(1.f, 0.2f, 0, 0.2f);
    emitter->ranges.color_high = glm::vec4(1.f, 0.6f, 0.2f, 0.5f);
    emitter->ranges.life_low = 3.f;
    emitter->ranges.life_high = 6.f;
    emitter->ranges.vel_low = glm::vec2(0, -20.f);
    emitter->ranges.vel_high = glm::vec2(0, -40.f);
    emitter->ranges.acc_low = glm::vec2(-5.f, -5.f);
    emitter->ranges.acc_high = glm::vec2(-10.f, -12.f);
}

void Particle_editor::render_ImGui()
{
    ImGui::Begin("particle editor");
    {
        ImGui::SliderFloat3("background color", colors, 0, 1.f, "%.2f");
        glClearColor(colors[0], colors[1], colors[2], 1.f);
    }
    ImGui::End();
}

void Particle_editor::render()
{
    emitter->update(dt);
    pp_unit.begRender();
    renderer.load_projection(glm::vec4(0, 0, coords.size));
    renderer.rend_particles(emitter->get_render_data());
    pp_unit.endRender(2);
    pp_unit.render_fb0();
}
