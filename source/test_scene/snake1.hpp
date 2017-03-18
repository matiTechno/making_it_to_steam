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

    void processEvent(SDL_Event& event) override;

    void render() override;

private:
    int score;
    glm::vec2 map_pos;
    int map_size;
    int grid_size;
    std::vector<Sprite> snake_parts;
    std::vector<Sprite> grids;
    Texture tex_grid;
    float grid_time;
    glm::ivec2 vel_dir;
    float food_spawn_time;
    float next_spawn;
    float accumulator;
    Sprite food;
    std::mt19937 rn_eng;
    // int = rounds to wait
    std::list<std::pair<Sprite, int>> waiting_parts;
    bool was_move_key;

    void update_logic();
};

#endif // SNAKE1_HPP
