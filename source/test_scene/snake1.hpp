#ifndef SNAKE1_HPP
#define SNAKE1_HPP

#include "../common_scene.hpp"
#include <random>
#include <list>

class Snake1: public Scene
{
public:
    Snake1();

    void update(float dt) override;

    void processEvent(const SDL_Event& event) override;

    void render() override;

private:
    int score;
    // map
    glm::ivec2 map_pos;
    int map_size;
    int grid_size;
    Texture tex_grid;
    std::vector<Sprite> grids;
    // time
    float step_time;
    float spawn_time;
    float next_spawn;
    float accumulator;
    // entities
    std::vector<Sprite> snake_parts;
    // int = moves to wait
    std::list<std::pair<Sprite, int>> waiting_parts;
    Sprite food;
    // other
    glm::ivec2 vel_dir;
    std::mt19937 rn_eng;
    bool was_move_key;
    glm::vec4 camera;
    Sample sample;

    void spawn_food();
    void init_map();
    void move_snake();
    bool is_collision(Sprite& sprite1, Sprite& sprite2);

    // resizing
    bool is_resizing = false;
    const float times_bigger = 0.5f;
    const float target_time = 0.3f;
    glm::vec2 slope;
    float time;
    glm::ivec4 coords_init;
};

#endif // SNAKE1_HPP
