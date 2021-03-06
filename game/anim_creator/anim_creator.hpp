#pragma once

#include "camera_scene.hpp"
#include <unordered_map>
#include "animation.hpp"
#include <list>
#include <fstream>

class Anim_creator: public Camera_scene
{
public:
    Anim_creator();

    void on_quit_event() override;

    void render_ImGui() override;

    static const Anim_creator* handle;

    mutable bool quit_request = false;

private:
    P_data grids;
    int grid_size = 15.f;
    glm::vec4 color1{0.7, 0.7, 0.7, 0.8};
    glm::vec4 color2{0.5, 0.5, 0.5, 0.8};
    glm::ivec2 max_fb_size{0, 0};
    std::vector<char> tex_filename_input, anim_name_input, anim_rename_input,
    coll_group_input, coll_group_rename_input, anim_filename_input, save_anim_input;
    std::unique_ptr<Texture> texture;
    std::string err_msg;
    bool countures = false;
    Sprite tex_sprite;
    std::string tex_filename;
    std::unordered_map<std::string, Animation> animations;
    Animation* anim = nullptr;
    std::list<std::string> store_anim_names, store_coll_group_names;
    std::vector<const char*> anim_names, coll_group_names;
    int current_anim_name = -1, current_coll_group_name = -1;
    float preview_scale = 1.f;
    int anim_to_compare = 0;
    int first_frame = 1;
    std::string anim_filename, anim_file_to_load;

    void set_grid();
    bool load_texture(const std::string& filename);
    void set_sprite();
    void clear();
    void set_origin_for_all(const glm::vec2& origin);
    bool save(const std::string& filename, bool allow_override);
    void load_anim(const std::string& filename);
    void popups();

    void processEvent2(const SDL_Event& event) override;
    void render2() override;
};
