#include "particle_editor.hpp"
#include "emitter.hpp"

Particle_editor::~Particle_editor() = default;

Particle_editor::Particle_editor():
    texture("res/particle.png")
{
    emitter = std::make_unique<Emitter>(10000, texture);
    emitter->always_active = true;
    emitter->position = glm::vec2(100.f, coords.size.y - 100.f);
    emitter->size = glm::vec2(300.f, 10.f);
    emitter->spawn_time = 0.0005f;
    emitter->ranges.size_low = glm::vec2(2.f);
    emitter->ranges.size_high = glm::vec2(10.f);
    emitter->ranges.color_low = glm::vec4(0.f, 1.f, 0.f, 0.1f);
    emitter->ranges.color_high = glm::vec4(0.3f, 1.f, 0.3f, 0.2f);
    emitter->ranges.life_low = 2.f;
    emitter->ranges.life_high = 10.f;
    emitter->ranges.vel_low = glm::vec2(0.f, -20.f);
    emitter->ranges.vel_high = glm::vec2(0.f, -40.f);
    emitter->ranges.acc_low = glm::vec2(-2.f, -5.f);
    emitter->ranges.acc_high = glm::vec2(10.f, -12.f);
    emitter->dst_alpha = GL_ONE;
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
    renderer.load_projection(glm::vec4(0, 0, coords.size));
    renderer.rend_particles(emitter->get_render_data());
}
