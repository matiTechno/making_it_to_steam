#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP

#include "../scene.hpp"
#include "../rendering/font_loader.hpp"
#include "../sound_system.hpp"
#include "../opengl/shader.hpp"

class Test_scene: public Scene
{
public:
    Test_scene();

    void update(float dt) override;

    void render() override;

private:
    void process_event(SDL_Event& event) override;

    Font font, font_progy;
    Texture texture;
    Music music;
    Sample sample;
    bool v_sync;
    int num_frames;
    float acc_time;
    float frame_time;
    Shader red_effect;
};

#endif // TEST_SCENE_HPP
