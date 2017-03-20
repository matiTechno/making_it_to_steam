#include "snake1_end_menu.hpp"
#include "test_scene.hpp"
#include "snake1.hpp"

Snake1_end_menu::Snake1_end_menu(int score, Game_state state):
    num_title_texts(2),
    current_option(num_title_texts),
    border_size(0.f, 0.f),
    spacing(10.f),
    state(state)
{
    is_opaque = false;
    coords = glm::ivec4(300, 120, 400, 400);

    Text text(&Test_scene::handle->font);
    text.color = glm::vec4(1.f, 1.f, 0.f, 1.f);
    text.text = "score: " + std::to_string(score);
    vec_text.push_back(text);
    if(state == Game_state::over)
        text.text = "game over";
    else
        text.text = "game paused";
    text.text += "\n";
    vec_text.push_back(text);
    text.color = glm::vec4(0.f, 1.f, 0.f, 1.f);
    text.text = "play";
    vec_text.push_back(text);
    text.text = "go to test_scene";
    vec_text.push_back(text);
    text.text = "exit";
    vec_text.push_back(text);

    for(auto& text: vec_text)
    {
        if(text.getSize().x > border_size.x)
            border_size.x = text.getSize().x;
        border_size.y += text.getSize().y;
    }
    border_size.x += 2 * spacing;
    border_size.y += (vec_text.size() + 1) * spacing;

    coords.z = border_size.x;
    coords.w = border_size.y;
}

void Snake1_end_menu::processEvent(const SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN)
    {
        if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
        {
            --current_option;
            if(current_option < num_title_texts)
                current_option = vec_text.size() - 1;
        }
        else if(event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
        {
            ++current_option;
            if(current_option > vec_text.size() - 1)
                current_option = num_title_texts;
        }
        else if(event.key.keysym.sym == SDLK_PAUSE || event.key.keysym.sym == SDLK_RETURN)
        {
            if(current_option == num_title_texts)
            {
                if(state == Game_state::over)
                {
                    num_scenes_to_pop = 2;
                    set_new_scene<Snake1>();
                }
                else
                    num_scenes_to_pop = 1;
            }
            else if(current_option == num_title_texts + 1)
            {
                num_scenes_to_pop = 2;
            }
            else if(current_option == num_title_texts + 2)
                App::should_close = true;
        }
    }
}

void Snake1_end_menu::render()
{
//    renderer.beg_batching();
//    {
//        int w, h;
//        SDL_GL_GetDrawableSize(sdl_win_handle, &w, &h);

//        Sprite sprite;
//        sprite.size = border_size;
//        sprite.position = glm::vec2(w / 2.f - border_size.x /2.f,
//                                    h / 2.f - border_size.y / 2.f);
//        sprite.color = glm::vec4(0.f, 0.f, 0.f, 0.8f);
//        renderer.render(sprite);

//        float pos_y = sprite.position.y + spacing;
//        for(std::size_t i = 0; i < vec_text.size(); ++i)
//        {
//            vec_text[i].position = glm::vec2(w / 2.f - vec_text[i].getSize().x / 2.f, pos_y);
//            if(i == current_option)
//                vec_text[i].color.a = 1.f;
//            else if(i >= num_title_texts)
//                vec_text[i].color.a = 0.3f;
//            renderer.render(vec_text[i]);
//            pos_y += spacing + vec_text[i].getSize().y;
//        }
//    }
//    renderer.end_batching();
pp_unit.begRender();
    renderer.load_projection(glm::vec4(0, 0, coords.z, coords.w));
    renderer.beg_batching();
    {
//        int w, h;
//        SDL_GL_GetDrawableSize(sdl_win_handle, &w, &h);

        Sprite sprite;
        sprite.size = border_size;
//        sprite.position = glm::vec2(w / 2.f - border_size.x /2.f,
//                                    h / 2.f - border_size.y / 2.f);
        sprite.position = glm::vec2(0.f, 0.f);
        sprite.color = glm::vec4(0.f, 0.f, 0.f, 0.7f);
        renderer.render(sprite);

        float pos_y = sprite.position.y + spacing;
        for(std::size_t i = 0; i < vec_text.size(); ++i)
        {
            vec_text[i].position = glm::vec2(border_size.x / 2.f - vec_text[i].getSize().x / 2.f, pos_y);
            if(i == current_option)
                vec_text[i].bloom = true;
            else if(i >= num_title_texts)
                vec_text[i].bloom = false;
            renderer.render(vec_text[i]);
            pos_y += spacing + vec_text[i].getSize().y;
        }
    }
    renderer.end_batching();
    pp_unit.endRender(2);
    //pp_unit.apply_effect(Test_scene::handle->red_effect);
    //pp_unit.apply_effect(Test_scene::handle->red_effect);
    pp_unit.render();
}
