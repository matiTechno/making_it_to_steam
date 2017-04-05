#pragma once

#include "camera_scene.hpp"
#include "animation.hpp"
#include <list>

class Origin_mode: public Camera_scene
{
public:
    Origin_mode(std::list<Frame>& frames, const Texture& texture, float* global_frametime,
                const std::vector<const char*>& coll_group_names);

    void render_ImGui() override;

private:
    glm::ivec2 origin_pos;
    glm::vec2 origin_rect_size{80.f};
    std::list<Frame>& frames;
    const Texture& texture;
    float* global_frametime;
    std::unordered_map<std::size_t, glm::ivec4> saved_coords;
    const std::vector<const char*>& coll_group_names;

    void processEvent2(const SDL_Event& event) override;
    void render2() override;
};
