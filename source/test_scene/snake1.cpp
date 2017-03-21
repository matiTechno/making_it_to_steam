#include "snake1.hpp"
#include "snake1_end_menu.hpp"
#include "test_scene.hpp"

Snake1::Snake1():
    score(0),
    map_pos(0, 0),
    map_size(400),
    grid_size(20),
    tex_grid("source/test_scene/res/tile.png", true),
    step_time(0.2f),
    spawn_time(6.f),
    accumulator(0.f),
    vel_dir(0, -1),
    was_move_key(false),
    sample("source/test_scene/res/sfx_sounds_powerup6.wav")
{
    assert(map_size % grid_size == 0);

    is_opaque = false;
    coords = glm::ivec4(60, 60, map_size, map_size);
    camera = glm::vec4(map_pos, map_size, map_size);

    std::random_device rd;
    rn_eng.seed(rd());

    glm::ivec2 vec_gird_size(grid_size);
    snake_parts.emplace_back();
    snake_parts.back().position = map_pos + glm::ivec2(map_size / 2);
    snake_parts.back().size = vec_gird_size;
    snake_parts.back().color = glm::vec4(1.f, 0.3f, 0.f, 0.8f);
    food.size = vec_gird_size;
    food.color = glm::vec4(1.f, 0.f, 0.f, 0.9f);

    init_map();
    spawn_food();
}

void Snake1::update(float dt)
{
    next_spawn -= dt;
    if(next_spawn <= 0.f)
        spawn_food();

    accumulator += dt;
    while(accumulator >= step_time)
    {
        for(auto& part: waiting_parts)
        {
            --part.second;
            if(part.second == 1)
                part.first.color = glm::vec4(0.f, 1.f, 0.f, 0.7f);
        }
        if(waiting_parts.size() && waiting_parts.back().second == 0)
        {
            snake_parts.push_back(waiting_parts.back().first);
            waiting_parts.erase(--waiting_parts.end());
        }
        move_snake();
        accumulator -= step_time;
    }
}

void Snake1::move_snake()
{
    was_move_key = false;

    for(auto it = snake_parts.rbegin(); it != snake_parts.rend() - 1; ++it)
        it->position = (it + 1)->position;

    Sprite& head = snake_parts.front();
    head.position += vel_dir * grid_size;
    if(head.position.x < grids.front().position.x)
        head.position.x = grids.back().position.x;
    else if(head.position.y < grids.front().position.y)
        head.position.y = grids.back().position.y;
    else if(head.position.x > grids.back().position.x)
        head.position.x = grids.front().position.x;
    else if(head.position.y > grids.back().position.y)
        head.position.y = grids.front().position.y;

    for(auto& part: snake_parts)
    {
        if(&part != &head && part.position == head.position)
            set_new_scene<Snake1_end_menu>(score, Game_state::over);
    }
    if(head.position == food.position)
    {
        sound_system.play_sample(sample, 10);
        ++score;
        waiting_parts.emplace_front(food, snake_parts.size() + 1);
        spawn_food();
    }
}

void Snake1::spawn_food()
{
    next_spawn = spawn_time;
    std::uniform_int_distribution<std::size_t> dis(0, grids.size() - 1);
    while(true)
    {
        food.position = grids[dis(rn_eng)].position;
        for(auto part: snake_parts)
        {
            if(glm::ivec2(food.position) == glm::ivec2(part.position))
                continue;
        }
        if(waiting_parts.size() && waiting_parts.back().second == 1)
            if(glm::ivec2(food.position) == glm::ivec2(waiting_parts.back().first.position))
                continue;

        break;
    }
}

void Snake1::init_map()
{
    int num_grids = map_size / grid_size;
    for(int i = 0; i < num_grids; ++i)
    {
        for(int j = 0; j < num_grids; ++j)
        {
            Sprite sprite;
            sprite.texture = &tex_grid;
            sprite.texCoords = glm::ivec4(0, 0, sprite.texture->getSize());
            sprite.color = glm::vec4(0.3f, 0.f, 1.f, 0.9f);
            sprite.size = glm::vec2(grid_size);
            sprite.position = glm::vec2(map_pos.x + i * grid_size, map_pos.y + j * grid_size);
            grids.push_back(std::move(sprite));
        }
    }
}

void Snake1::processEvent(const SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN)
    {
        if(event.key.keysym.sym == SDLK_ESCAPE)
            set_new_scene<Snake1_end_menu>(score, Game_state::paused);
        else if(event.key.keysym.sym == SDLK_h)
        {}
        else if(!was_move_key)
        {
            if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
            {
                was_move_key = true;
                if(vel_dir != glm::ivec2(0, 1))
                    vel_dir = glm::ivec2(0, -1);
            }
            else if(event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
            {
                was_move_key = true;
                if(vel_dir != glm::ivec2(0, -1))
                    vel_dir = glm::ivec2(0, 1);
            }
            else if(event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
            {
                was_move_key = true;
                if(vel_dir != glm::ivec2(1, 0))
                    vel_dir = glm::ivec2(-1, 0);
            }
            else if(event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
            {
                was_move_key = true;
                if(vel_dir != glm::ivec2(-1, 0))
                    vel_dir = glm::ivec2(1, 0);
            }
        }
    }
}

void Snake1::render()
{
    pp_unit.begRender();
    renderer.load_projection(camera);
    renderer.beg_batching();
    {
        for(auto& grid: grids)
            renderer.render(grid);
        {
            Text text(Test_scene::handle->font_progy);
            text.text = "press h to shrink / enlarge window";
            text.position = map_pos + glm::ivec2(10.f, 10.f);
            renderer.render(text);
        }
        renderer.render(food);
        for(auto& part: snake_parts)
            renderer.render(part);
        for(auto& part: waiting_parts)
            renderer.render(part.first);
    }
    renderer.end_batching();
    pp_unit.endRender(2);
    pp_unit.render();
}
