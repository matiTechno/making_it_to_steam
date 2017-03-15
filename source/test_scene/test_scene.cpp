#include "test_scene.hpp"
#include "../common_scene.hpp"
#ifdef USE_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl_gl3.h"
#endif

static std::string res_path = "source/test_scene/res/";

Test_scene::Test_scene():
    font(font_loader.loadFont(res_path + "Inconsolata-Regular.ttf", 40)),
    font_progy(font_loader.loadFont(res_path + "ProggyClean.ttf", 15)),
    texture(res_path + "Candies_Jerom_CCBYSA3.png", true),
    music(res_path + "Path to Lake Land.ogg"),
    sample(res_path + "sfx_exp_cluster1.wav"),
    v_sync(SDL_GL_GetSwapInterval()),
    num_frames(0),
    acc_time(0),
    red_effect("shaders/shader_fb.vert", "shaders/shader_fb_test_red.frag", std::string(),
               true, "red_effect")
{
    sound_system.play_music(music, true, 10);
}

void Test_scene::update(float dt)
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
//#include <random>

void Test_scene::render()
{
    int width, height;
    SDL_GL_GetDrawableSize(sdl_win_handle, &width, &height);
    glm::mat4 proj = glm::ortho(0.f, static_cast<float>(width),
                                static_cast<float>(height), 0.f);
    renderer.load_projection(proj);

    pp_unit.begRender();
    renderer.beg_batching();
    {
        Sprite sprite;
        sprite.position = glm::vec2(100.f, 100.f);
        sprite.size = glm::vec2(100.f, 100.f);
        sprite.rotation_point = sprite.size / 2.f;
        sprite.rotation = glm::pi<float>() / 4.f;
        sprite.texture = &texture;
        sprite.texCoords = glm::ivec4(0, 0, texture.getSize());
        sprite.bloom = true;
        renderer.render(sprite);

        sprite.position = glm::vec2(100.f, 150.f);
        renderer.render(sprite);
        sprite.position = glm::vec2(100.f, 200.f);
        renderer.render(sprite);
        sprite.position = glm::vec2(100.f, 250.f);
        renderer.render(sprite);
        sprite.position = glm::vec2(60.f, 280.f);
        sprite.bloom = false;
        renderer.render(sprite);
        sprite.size = glm::vec2(300.f, 300.f);
        sprite.position = glm::vec2(100.f, 280.f);
        renderer.render(sprite);
        sprite.position = glm::vec2(400.f, 280.f);
        sprite.blend_dfactor = GL_ONE;
        renderer.render(sprite);

        // FOR PERFORMANCE TEST
        //        sprite.size = glm::vec2(5.f, 5.f);
        //        sprite.rotation_point = sprite.size / 2.f;
        //        sprite.bloom = true;
        //        sprite.color.a = 0.1f;
        //        std::random_device rd;
        //        std::mt19937 mt(rd());
        //        std::uniform_real_distribution<float> uni_x(0.f, width);
        //        std::uniform_real_distribution<float> uni_y(0.f, height);
        //        for(int i = 0; i < 20000; ++i)
        //        {
        //            sprite.position = glm::vec2(uni_x(mt), uni_y(mt));
        //            renderer.render(sprite);
        //        }

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
        renderer.render(bb_text);

        renderer.render(text);

        Text text_fps(&font);
        text_fps.text = std::to_string(frame_time) + " v_sync:";
        if(v_sync)
            text_fps.text += "ON";
        else
            text_fps.text += "OFF";
        text_fps.position = glm::vec2(20.f, 20.f);
        text_fps.color = glm::vec4(1.f, 0.f, 1.f, 0.5f);
        renderer.render(text_fps);

        Text small_t(&font_progy);
        small_t.text = "is small font looking good? i'm curious.\n"
                       "it's been a long time,  fam.\n"
                       "can we see tomorrow?";
        small_t.position = glm::vec2(100.f, 200.f);
        Sprite small_t_bb;
        small_t_bb.color = glm::vec4(0.f, 0.f, 0.f, 1.f);
        small_t_bb.position = small_t.position - glm::vec2(10.f, 10.f);
        small_t_bb.size = small_t.getSize() + glm::vec2(20.f, 20.f);
        renderer.render(small_t_bb);
        renderer.render(small_t);
    }
    renderer.end_batching();
    pp_unit.endRender(2);
    pp_unit.apply_effect(red_effect);
    pp_unit.apply_effect(red_effect);
    pp_unit.render(false);

#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiSetCond_::ImGuiSetCond_Once);
    ImGui::ShowTestWindow();
    ImGui::Render();
#endif
}

void Test_scene::process_event(SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN && !event.key.repeat)
    {
        if(event.key.keysym.sym == SDLK_1)
        {
            v_sync = !v_sync;
            SDL_GL_SetSwapInterval(v_sync);
        }
        else if(event.key.keysym.sym == SDLK_2)
            sound_system.play_sample(sample, 10);
        else if(event.key.keysym.sym == SDLK_ESCAPE)
            num_scenes_to_pop = 1;
    }
}
