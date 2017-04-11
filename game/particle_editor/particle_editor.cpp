#include "particle_editor.hpp"

void Particle_editor::render_ImGui()
{
    ImGui::Begin("particle editor");
    {
        ImGui::SliderFloat3("background color", colors, 0, 1.f, "%.2f");
        glClearColor(colors[0], colors[1], colors[2], 1.f);
    }
    ImGui::End();
}
