#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP

#include <MITS/scene_common.hpp>

class Test_scene: public Scene
{
public:
    Test_scene();

    void update() override;

    void update_coords() override;

    void render() override;

    void render_ImGui() override;

private:

    Font font, font_progy;
    Texture tex_sprite, tex_tC_parti;
    Music music;
    Sample sample;
    bool v_sync, show_ImGui, is_pp, is_red;
    int num_frames;
    float acc_time;
    float frametime;
    Shader red_effect;
    P_data p_data;
    P_data_tCs p_data_tcs;
    std::vector<Sprite> vec_sprites;
    glm::vec4 camera;

    // resource class design
    // exposed only by const
    friend class Snake1;
    friend class Snake1_end_menu;
    static const Test_scene* handle;

    static bool isCurrent;

    void processEvent(const SDL_Event& event) override;
};

#endif // TEST_SCENE_HPP
