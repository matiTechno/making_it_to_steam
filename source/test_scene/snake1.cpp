#include "snake1.hpp"
#include "snake1_end_menu.hpp"

Snake1::Snake1():
    score(0),
    map_pos(80.f, 100.f),
    map_size(400),
    grid_size(20),
    tex_grid("source/test_scene/res/tile.png", true),
    grid_time(0.2f),
    vel_dir(0, -1),
    food_spawn_time(6.f),
    next_spawn(food_spawn_time),
    accumulator(0.f),
    rn_eng(),
    was_move_key(false)
{
    SCENE_is_opaque = false;
    assert(map_size % grid_size == 0);
    int num_grids_x = map_size / grid_size;

    for(int i = 0; i < num_grids_x; ++i)
    {
        for(int j = 0; j < num_grids_x; ++j)
        {
            Sprite grid;
            grid.position.x = map_pos.x + i * grid_size;
            grid.position.y = map_pos.y + j * grid_size;
            grid.size = glm::vec2(grid_size);
            grid.texture = &tex_grid;
            grid.texCoords = glm::ivec4(0, 0, tex_grid.getSize());
            grid.color.a = 0.2f;
            grids.push_back(std::move(grid));
        }
    }

    std::random_device rd;
    rn_eng.seed(rd());
    std::uniform_int_distribution<int> uni(0, grids.size());

    Sprite sprite;
    sprite.position = grids[uni(rn_eng)].position;
    sprite.color = glm::vec4(0.f, 1.f, 0.f, 0.5f);
    snake_parts.push_back(std::move(sprite));

    while(true)
    {
        food.position = grids[uni(rn_eng)].position;
        if(food.position == snake_parts[0].position)
            continue;
        break;
    }

    food.size = glm::vec2(grid_size);
    food.color = glm::vec4(1.f, 0.f, 0.f, 0.8f);
}

void Snake1::processEvent(SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN)
    {
        if(event.key.keysym.sym == SDLK_ESCAPE)
        {
            set_new_scene<Snake1_end_menu>(score, Game_state::paused);
        }
        else if(!was_move_key)
        {
            was_move_key = true;
            if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
            {
                if(vel_dir != glm::ivec2(0, 1))
                    vel_dir = glm::ivec2(0, -1);
            }
            else if(event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
            {
                if(vel_dir != glm::ivec2(0, -1))
                    vel_dir = glm::ivec2(0, 1);
            }
            else if(event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
            {
                if(vel_dir != glm::ivec2(1, 0))
                    vel_dir = glm::ivec2(-1, 0);
            }
            else if(event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
            {
                if(vel_dir != glm::ivec2(-1, 0))
                    vel_dir = glm::ivec2(1, 0);
            }
        }
    }
}

void Snake1::update(float dt)
{
    accumulator += dt;
    next_spawn -= dt;
    while(accumulator >= grid_time)
    {
        update_logic();
        accumulator -= grid_time;
    }
}

void Snake1::render()
{
    renderer.beg_batching();
    {
        float border = 10.f;
        Sprite sprite;
        sprite.position = map_pos - border;
        sprite.size = glm::vec2(map_size + border * 2.f);
        sprite.color = glm::vec4(0.f, 0.f, 0.2f, 0.9f);
        renderer.render(sprite);

        for(auto& grid: grids)
            renderer.render(grid);

        renderer.render(food);

        for(auto& part: waiting_parts)
            renderer.render(part.first);

        for(auto& part: snake_parts)
            renderer.render(part);
    }
    renderer.end_batching();
}

void Snake1::update_logic()
{
    was_move_key = false;

    bool respawn = false;
    if(next_spawn <= 0.f)
        respawn = true;

    for(auto it = snake_parts.rbegin(); it < snake_parts.rend() - 1; ++it)
    {
        it->position = (it + 1)->position;
    }
    for(auto& part: waiting_parts)
        part.second -= 1;

    if(waiting_parts.size())
    {
        if(waiting_parts.front().second == 0)
        {
            snake_parts.push_back(snake_parts[0]);
            waiting_parts.erase(waiting_parts.begin());
        }
        else if(waiting_parts.front().second == 1)
            waiting_parts.front().first.color = snake_parts[0].color;
    }

    Sprite& head = snake_parts[0];
    head.position += vel_dir * grid_size;
    if(head.position.x < map_pos.x)
        head.position.x += map_size;
    else if(head.position.x == map_pos.x + map_size)
        head.position.x = map_pos.x;
    else if(head.position.y < map_pos.y)
        head.position.y += map_size;
    else if(head.position.y == map_pos.y + map_size)
        head.position.y = map_pos.y;

    for(std::size_t i = 1; i < snake_parts.size(); ++i)
    {
        if(head.position == snake_parts[i].position)
            set_new_scene<Snake1_end_menu>(score, Game_state::over);
    }

    if(head.position == food.position && !respawn)
    {
        respawn = true;
        ++score;
        Sprite sprite(food);
        waiting_parts.emplace_back(std::move(sprite), snake_parts.size() + 1);
    }

    if(respawn)
    {
        next_spawn = food_spawn_time;
        std::uniform_int_distribution<int> uni(0, grids.size());
        while(true)
        {
            food.position = grids[uni(rn_eng)].position;
            for(auto part: snake_parts)
            {
                if(part.position == food.position)
                    continue;
            }
            break;
        }
    }
}
