#pragma once

#include "camera_scene.hpp"
#include "anim_rect.hpp"
#include <list>

class Origin_mode: public Camera_scene
{
public:
    Origin_mode(std::list<Anim_rect>& frames, const Texture& texture, float* global_frametime);

    void render_ImGui() override;

private:
    glm::ivec2 origin_pos;
    glm::vec2 origin_rect_size{80.f};
    std::list<Anim_rect>& frames;
    const Texture& texture;
    float* global_frametime;
    std::unordered_map<std::size_t, glm::ivec4> saved_coords;

    void processEvent2(const SDL_Event& event) override;
    void render2() override;
};
