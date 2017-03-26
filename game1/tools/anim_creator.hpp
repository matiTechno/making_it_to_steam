#ifndef ANIM_CREATOR_HPP
#define ANIM_CREATOR_HPP

#include "../../source/common_scene.hpp"
#include "anim_frame.hpp"
#include <list>

class Anim_creator: public Scene
{
public:
    Anim_creator();

    void processEvent(const SDL_Event& event) override;

    void render() override;

    void render_ImGui() override;

private:
    std::list<std::unique_ptr<Anim_frame>> frames;
    std::unique_ptr<Texture> texture;
    Sprite tex_sprite;
    std::vector<Sprite> grids;
    std::vector<char> input;
    std::string err_msg;
    const int grid_size = 15;
    glm::vec4 grid_white{0.7f, 0.7f, 0.7f, 0.8f};
    glm::vec4 grid_grey{0.5f, 0.5f, 0.5f, 0.8f};
    glm::vec4 camera;
    float camera_scale;
    const float zoom_factor = 1.2f;
    bool is_rb_pressed = false;
    glm::vec2 w_pos;
    glm::vec2 w_size;
    glm::ivec2 cursor_pos;
    bool show_countures = false;
    int id = 0;
    float ft_for_all;

    void set_sprite();
    void set_coords();
    void set_camera();
    bool is_point_in_im_win(const glm::vec2& point);
    void set_origin_for_all(const glm::vec2& origin);
};

#endif // ANIM_CREATOR_HPP
