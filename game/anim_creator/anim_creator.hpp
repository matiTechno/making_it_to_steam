#pragma once

#include "camera_scene.hpp"
#include <unordered_map>
#include "animation.hpp"
#include <list>

class Anim_creator: public Camera_scene
{
public:
    Anim_creator();

    void on_quit_event() override;

    void render_ImGui() override;

private:
    P_data grids;
    int grid_size = 15.f;
    glm::vec4 color1{0.7, 0.7, 0.7, 0.8};
    glm::vec4 color2{0.5, 0.5, 0.5, 0.8};
    glm::ivec2 max_fb_size{0, 0};
    std::vector<char> tex_filename_input, anim_name_input, anim_rename_input;
    std::unique_ptr<Texture> texture;
    std::string err_msg;
    bool countures = false;
    Sprite tex_sprite;
    std::string tex_filename;
    std::unordered_map<std::string, Animation> animations;
    Animation* anim = nullptr;
    std::list<std::string> store_anim_names;
    std::vector<const char*> anim_names;
    int current_anim_name = -1;
    float preview_scale = 1.f;
    int first_frame = 1, first_frame_to_compare = 1;
    int anim_to_compare = 0;

    void set_grid();
    void load_texture(const std::string& filename);
    void set_sprite();
    void clear();
    void set_origin_for_all(const glm::vec2& origin);

    void processEvent2(const SDL_Event& event) override;
    void render2() override;
};