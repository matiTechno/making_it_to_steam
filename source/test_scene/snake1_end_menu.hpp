#ifndef SNAKE1_END_MENU_HPP
#define SNAKE1_END_MENU_HPP

#include "../common_scene.hpp"

// score: ...
// game over / game paused
// * play
// * go to test_scene
// * exit

enum class Game_state
{
    over,
    paused
};

class Snake1_end_menu: public Scene
{
public:
    Snake1_end_menu(int score, Game_state state);

    void processEvent(const SDL_Event& event) override;

    void update_coords() override;

    void render() override;

private:
    std::vector<Text> vec_text;
    std::size_t num_title_texts, current_option;
    glm::vec2 border_size;
    float spacing;
    Game_state state;

    // pp
    Shader shader_wave;
    float wave_vel = 100.f;
    float wave_length = 300.f;
    float time = 0.f;

    // mouse grabbing
    bool is_l_button_pressed;
    bool was_moved =  false;
};

#endif // SNAKE1_END_MENU_HPP
