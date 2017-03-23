#include "snake1.hpp"
#include "snake1_end_menu.hpp"
#include "test_scene.hpp"
#include <glm/exponential.hpp>

Snake1::Snake1():
    score(0),
    map_pos(0, 0),
    map_size(400),
    grid_size(20),
    tex_grid("source/test_scene/res/tile.png"),
    step_time(0.2f),
    spawn_time(6.f),
    accumulator(0.f),
    vel_dir(0, -1),
    was_move_key(false),
    sample("source/test_scene/res/sfx_sounds_powerup6.wav")
{
    assert(map_size % grid_size == 0);

    is_opaque = false;
    coords.size = glm::ivec2(map_size);
    camera = glm::vec4(map_pos, map_size, map_size);

    std::random_device rd;
    rn_eng.seed(rd());

    glm::ivec2 vec_gird_size(grid_size);
    snake_parts.emplace_back();
    snake_parts.back().position = map_pos + glm::ivec2(map_size / 2);
    snake_parts.back().size = vec_gird_size;
    snake_parts.back().color = glm::vec4(1.f, 0.3f, 0.f, 0.8f);
    food.size = vec_gird_size;
    food.color = glm::vec4(1.f, 0.f, 0.f, 0.7f);

    init_map();
    spawn_food();

    // on key press slope is reversed
    slope = times_bigger * glm::vec2(coords.size) / target_time;
    if(times_bigger > 1.f)
        slope *= -1.f;
}

void Snake1::update()
{
    next_spawn -= dt;

    accumulator += dt;
    while(accumulator >= step_time)
    {
        for(auto& part: waiting_parts)
        {
            --part.second;
            if(part.second == 1)
                part.first.color = glm::vec4(0.f, 1.f, 0.f, 0.5f);
        }
        if(waiting_parts.size() && waiting_parts.back().second == 0)
        {
            snake_parts.push_back(waiting_parts.back().first);
            waiting_parts.erase(--waiting_parts.end());
        }
        move_snake();

        if(next_spawn <= 0.f)
            spawn_food();

        accumulator -= step_time;
    }
}

void Snake1::update_coords()
{
    if(is_resizing)
    {
        time += dt;
        if(time > target_time)
        {
            is_resizing = false;
            time = target_time;
        }
        coords.size = coords_init.size + glm::ivec2(slope * time);
    }
    coords.pos = App::get_fb_size() / 2 - coords.size / 2;
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
        if(&part != &head && is_collision(head, part))
            set_new_scene<Snake1_end_menu>(score, Game_state::over);
    }
    if(waiting_parts.size() && waiting_parts.back().second == 1)
        if(is_collision(head, waiting_parts.back().first))
            set_new_scene<Snake1_end_menu>(score, Game_state::over);

    if(is_collision(head, food))
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
    std::vector<std::size_t> covered_grids;
    std::size_t rand;
    std::uniform_int_distribution<std::size_t> dis(0, grids.size() - 1);
    while(true)
    {
        rand = dis(rn_eng);
        for(auto& i: covered_grids)
            if(rand == i)
                goto repeat;

        food.position = grids[rand].position;

        for(auto& part: snake_parts)
        {
            if(is_collision(part, food))
            {
                covered_grids.push_back(rand);
                goto repeat;
            }
        }
        for(auto& part: waiting_parts)
        {
            if(is_collision(part.first, food))
            {
                covered_grids.push_back(rand);
                goto repeat;
            }
        }
        break;
repeat:;
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
            sprite.color = glm::vec4(0.3f, 0.f, 1.f, 0.7f);
            sprite.size = glm::vec2(static_cast<float>(grid_size));
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
        else if(event.key.keysym.sym == SDLK_1 && is_resizing == false)
        {
            slope *= -1.f;
            is_resizing = true;
            time = 0.f;
            coords_init = coords;
        }
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
    renderer.load_projection(camera);
    renderer.beg_batching();
    {
        for(auto& grid: grids)
            renderer.render(grid);
        {
            Text text(Test_scene::handle->font_progy);
            text.text = "press 1 to shrink / enlarge window";
            text.position = map_pos + glm::ivec2(10.f, 10.f);
            renderer.render(text);
        }
        renderer.render(food);
        for(auto& part: waiting_parts)
            renderer.render(part.first);
        for(auto& part: snake_parts)
            renderer.render(part);
    }
    renderer.end_batching();
}

bool Snake1::is_collision(Sprite& sprite1, Sprite& sprite2)
{
    bool x = sprite1.position.x + sprite1.size.x > sprite2.position.x
            && sprite2.position.x + sprite2.size.x > sprite1.position.x;

    bool y = sprite1.position.y + sprite1.size.y > sprite2.position.y
            && sprite2.position.y + sprite2.size.y > sprite1.position.y;

    return x && y;
}
