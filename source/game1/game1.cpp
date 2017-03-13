#include "game1.h"
#include "../app.hpp"
#include "../systems.h"

Game1::Game1(const Systems& systems, bool is_opaque):
    Scene(systems, is_opaque),
    font(systems.font_loader->loadFont("res/Inconsolata-Regular.ttf", 40)),
    texture("res/Candies_Jerom_CCBYSA3.png", true),
    music("res/Path to Lake Land.ogg"),
    sample("res/sfx_exp_cluster1.wav"),
    v_sync(SDL_GL_GetSwapInterval()),
    num_frames(0),
    acc_time(0)
{
    systems.sound_system->play_music(music, true, 10);
}

void Game1::on_quit_event()
{
    App::should_close = true;
}

void Game1::update(float dt)
{
    ++num_frames;
    acc_time += dt;
    if(acc_time > 1.f)
    {
        frame_time = acc_time / num_frames;
        num_frames = 0;
        acc_time = 0;
    }
}

#include "../rendering/sprite.hpp"
#include "../rendering/text.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

void Game1::render()
{
    int width, height;
    SDL_GL_GetDrawableSize(systems.window, &width, &height);
    glm::mat4 proj = glm::ortho(0.f, static_cast<float>(width),
                                static_cast<float>(height), 0.f);
    systems.renderer_2D->load_projection(proj);

    systems.pp_unit->begRender();
    systems.renderer_2D->beg_batching();
    {
        Sprite sprite;
        sprite.position = glm::vec2(100.f, 100.f);
        sprite.size = glm::vec2(100.f, 100.f);
        sprite.rotation_point = sprite.size / 2.f;
        sprite.rotation = glm::pi<float>() / 4.f;
        sprite.texture = &texture;
        sprite.texCoords = glm::ivec4(0, 0, texture.getSize());
        sprite.bloom = true;
        renderer_2D->render(sprite);

        for(int i = 0; i < 1000; ++i)
            renderer_2D->render(sprite);

        sprite.position = glm::vec2(100.f, 150.f);
        renderer_2D->render(sprite);
        sprite.position = glm::vec2(100.f, 200.f);
        renderer_2D->render(sprite);
        sprite.position = glm::vec2(100.f, 250.f);
        renderer_2D->render(sprite);
        sprite.position = glm::vec2(60.f, 280.f);
        sprite.bloom = false;
        renderer_2D->render(sprite);
        sprite.size = glm::vec2(300.f, 300.f);
        sprite.position = glm::vec2(100.f, 280.f);
        renderer_2D->render(sprite);
        sprite.position = glm::vec2(400.f, 280.f);
        sprite.blend_dfactor = GL_ONE;
        renderer_2D->render(sprite);

        Text text(&font);
        text.position = glm::vec2(150, 300);
        text.text = "Hello World (OpenGL edition)!\nnew game coming\n"
                    "1 - switch v_sync\n2 - play sound\nEsc - exit";
        text.color = glm::vec4(0.f, 1.f, 0.f, 1.f);
        text.bloom = true;

        Sprite bb_text;
        bb_text.color = glm::vec4(0.f, 0.f, 0.f, 0.95f);
        bb_text.position = text.position;
        bb_text.size = text.getSize();
        bb_text.rotation = text.rotation;
        bb_text.rotation_point = text.rotation_point;
        renderer_2D->render(bb_text);

        renderer_2D->render(text);

        Text text_fps(&font);
        text_fps.text = std::to_string(frame_time) + " v_sync:";
        if(v_sync)
            text_fps.text += "ON";
        else
            text_fps.text += "OFF";
        text_fps.position = glm::vec2(20.f, 20.f);
        text_fps.color = glm::vec4(1.f, 0.f, 1.f, 0.1f);
        renderer_2D->render(text_fps);
    }
    systems.renderer_2D->end_batching();
    systems.pp_unit->endRender(2, true);
    systems.pp_unit->render(false);
}

void Game1::process_event(SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN && !event.key.repeat)
    {
        if(event.key.keysym.sym == SDLK_1)
        {
            v_sync = !v_sync;
            SDL_GL_SetSwapInterval(v_sync);
        }
        else if(event.key.keysym.sym == SDLK_2)
            systems.sound_system->play_sample(sample, 10);
        else if(event.key.keysym.sym == SDLK_ESCAPE)
            App::should_close = true;
    }
}
