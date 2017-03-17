#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP

#include "../scene.hpp"
#include "../common_scene.hpp"

class Test_scene: public Scene
{
public:
    Test_scene();

    void update(float dt) override;

    void render() override;

private:
    void process_event(SDL_Event& event) override;

    Font font, font_progy;
    Texture tex_sprite, tex_parti, tex_tC_parti;
    Music music;
    Sample sample;
    bool v_sync;
    bool is_pp;
    int num_frames;
    float acc_time;
    float frametime;
    Shader red_effect;
    P_data p_data;
    P_data_tCs p_data_tcs;
    std::vector<Sprite> vec_sprites;
};

#endif // TEST_SCENE_HPP
