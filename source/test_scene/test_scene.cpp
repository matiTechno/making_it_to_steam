#include "test_scene.hpp"
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#define SP_TOP_Y 50.f
#define SP_SIZE_Y 200.f
#define SP_X 400.f
#define PD_TOP_Y 100.f
#define PD_SIZE_Y 300.f
#define PD_X 500
#define PDT_TOP_Y 50.f
#define PDT_SIZE_Y 400.f
#define PDT_X 600.f

static std::string res_path = "source/test_scene/res/";

Test_scene::Test_scene():
    font(font_loader.loadFont(res_path + "Inconsolata-Regular.ttf", 40)),
    font_progy(font_loader.loadFont(res_path + "ProggyClean.ttf", 20)),
    tex_sprite(res_path + "Candies_Jerom_CCBYSA3.png", true),
    tex_parti(res_path + "particle.png", false),
    tex_tC_parti(res_path + "explo.png", true),
    music(res_path + "Path to Lake Land.ogg"),
    sample(res_path + "sfx_exp_cluster1.wav"),
    v_sync(SDL_GL_GetSwapInterval()),
    is_pp(true),
    num_frames(0),
    acc_time(0),
    red_effect("shaders/shader_fb.vert", "shaders/shader_fb_test_red.frag", std::string(),
               true, "red_effect")
{
    sound_system.play_music(music, true, 10);

    std::random_device rd;
    std::mt19937 mt(rd());
    {
        vec_sprites.reserve(1000);
        glm::vec2 position(SP_X, SP_TOP_Y);
        std::uniform_real_distribution<float> x(0.f, 50.f);
        std::uniform_real_distribution<float> y(0.f, SP_SIZE_Y);
        for(std::size_t i = 0; i < vec_sprites.capacity(); ++i)
        {
            Sprite sprite;
            sprite.position = position + glm::vec2(x(mt), y(mt));
            sprite.size = glm::vec2(2.f, 2.f);
            sprite.bloom = true;
            vec_sprites.push_back(std::move(sprite));
        }
    }
    {
        p_data.vbo_data.reserve(100000);
        p_data.num_to_render = p_data.vbo_data.capacity();
        p_data.bloom = true;
        p_data.texture = &tex_parti;
        p_data.texCoords = glm::ivec4(0.f, 0.f, p_data.texture->getSize());
        p_data.blend_dfactor = GL_ONE;
        glm::vec2 position(PD_X, PD_TOP_Y);
        std::uniform_real_distribution<float> x(0.f, 100.f);
        std::uniform_real_distribution<float> y(0.f, PD_SIZE_Y);
        for(std::size_t i = 0; i < p_data.vbo_data.capacity(); ++i)
        {
            Vbo_p p;
            p.color = glm::vec4(1.f, 1.f, 1.f, 0.1f);
            p.size = glm::vec2(1.f, 1.f);
            p.pos = position + glm::vec2(x(mt), y(mt));
            p_data.vbo_data.push_back(std::move(p));
        }
    }
    {
        p_data_tcs.vbo_data.reserve(100);
        p_data_tcs.num_to_render = p_data_tcs.vbo_data.capacity();
        p_data_tcs.texture = &tex_tC_parti;
        glm::vec2 position(PDT_X, PDT_TOP_Y);
        std::uniform_real_distribution<float> x(0.f, 100.f);
        std::uniform_real_distribution<float> y(0.f, PDT_SIZE_Y);
        std::uniform_int_distribution<int> z(0, 2);
        for(std::size_t i = 0; i < p_data_tcs.vbo_data.capacity(); ++i)
        {
            Vbo_p_tCs p;
            int r = z(mt);
            if(r == 0)
            {
                p.texCoords = glm::ivec4(208, 18, 63, 65);
                p.bloom = true;
            }
            else if(r == 1)
            {
                p.texCoords = glm::ivec4(487, 6, 77, 76);
                p.bloom = false;
            }
            else
            {
                p.texCoords = glm::ivec4(672, 0, 92, 86);
                p.bloom = false;
            }
            p.size = glm::vec2(40.f);
            p.color = glm::vec4(1.f, 1.f, 1.f, 1.f);
            p.pos = position + glm::vec2(x(mt), y(mt));
            p_data_tcs.vbo_data.push_back(std::move(p));
        }
    }
}

void Test_scene::update(float dt)
{
    ++num_frames;
    acc_time += dt;
    if(acc_time > 1.f)
    {
        frametime = acc_time / num_frames;
        num_frames = 0;
        acc_time = 0;
    }

    for(auto& sp: vec_sprites)
    {
        sp.position.y -= 20.f * dt;
        if(sp.position.y < SP_TOP_Y)
            sp.position.y = SP_TOP_Y + SP_SIZE_Y;
    }
    for(auto& p: p_data.vbo_data)
    {
        p.pos.y -= 50.f * dt;
        if(p.pos.y < PD_TOP_Y)
            p.pos.y = PD_TOP_Y + PD_SIZE_Y;
    }
    for(auto& p: p_data_tcs.vbo_data)
    {
        p.pos.y -= 30.f * dt;
        if(p.pos.y < PDT_TOP_Y)
            p.pos.y = PDT_TOP_Y + PDT_SIZE_Y;
    }
}

void Test_scene::render()
{
    int width, height;
    SDL_GL_GetDrawableSize(sdl_win_handle, &width, &height);
    glm::mat4 proj = glm::ortho(0.f, static_cast<float>(width),
                                static_cast<float>(height), 0.f);
    renderer.load_projection(proj);

    if(is_pp)
        pp_unit.begRender();
    renderer.beg_batching();
    {
        {
            Text text(&font);
            text.text = "rotation for Text class supported :)";
            text.rotation = -glm::pi<float>() / 4.f;
            text.rotation_point = text.getSize() / 2.f;
            text.position = glm::vec2(10.f, 280.f);
            text.color = glm::vec4(1.f, 1.f, 0.f, 1.f);
            Sprite sprite;
            sprite.color = glm::vec4(0.f, 0.f, 0.f, 1.f);
            sprite.rotation_point = text.rotation_point;
            sprite.rotation = text.rotation;
            sprite.position = text.position;
            sprite.size = text.getSize();
            renderer.render(sprite);
            renderer.render(text);
        }
        {
            Text text(&font_progy);
            text.text = "press:\n"
                        "* 1 to switch v_sync\n"
                        "* 2 to play sample\n"
                        "* 3 to switch postprocessing on/off";
            text.color = glm::vec4(0.3f, 1.f, 0.f, 1.f);
            text.bloom = true;
            text.position = glm::vec2(10.f, 50.f);
            Sprite sprite;
            sprite.color = glm::vec4(0.f, 0.f, 0.f, 0.99f);
            sprite.position = text.position;
            sprite.size = text.getSize();
            renderer.render(sprite);
            renderer.render(text);
        }
        {
            Text text(&font_progy);
            text.text = "fjefpekfpmvapqr\n"
                        "j3288402nld felwf";
            text.position = glm::vec2(10.f, 130.f);
            text.color = glm::vec4(1.f, 0.f, 1.f, 1.f);
            text.render_quads = true;
            text.bloom = true;
            renderer.render(text);
        }
        {
            Sprite sprite;
            sprite.texture = &tex_sprite;
            sprite.texCoords = glm::ivec4(0.f, 0.f, sprite.texture->getSize());
            sprite.position = glm::vec2(10.f, 300.f);
            sprite.size = glm::vec2(200.f, 200.f);
            renderer.render(sprite);
            sprite.position = glm::vec2(30.f, 200.f);
            sprite.bloom = true;
            renderer.render(sprite);
            sprite.bloom = false;
            sprite.blend_dfactor = GL_ONE;
            sprite.position = glm::vec2(60.f, 450.f);
            renderer.render(sprite);
        }
        for(auto& sp: vec_sprites)
            renderer.render(sp);
        renderer.rend_particles(p_data);
        renderer.rend_particles(p_data_tcs);
        {
            Text text(&font_progy);
            text.position = glm::vec2(SP_X, SP_TOP_Y + SP_SIZE_Y + 5.f);
            text.text = "normal sprites\n"
                        "with mat4x4 model matrix";
            Sprite sprite;
            sprite.color = glm::vec4(0.f, 0.f, 0.f, 0.9f);
            sprite.position = text.position;
            sprite.size = text.getSize() + glm::vec2(10.f, 10.f);
            renderer.render(sprite);
            renderer.render(text);
        }
        {
            Text text(&font_progy);
            text.position = glm::vec2(PD_X, PD_TOP_Y - 35.f);
            text.text = "100k P_data particles\n"
                        "with activate texture";
            Sprite sprite;
            sprite.color = glm::vec4(0.f, 0.f, 0.f, 0.9f);
            sprite.position = text.position;
            sprite.size = text.getSize() + glm::vec2(10.f, 10.f);
            renderer.render(sprite);
            renderer.render(text);
        }
        {
            Text text(&font_progy);
            text.position = glm::vec2(PDT_X - 50.f, PDT_TOP_Y + 100.f);
            text.text = "P_data_tCs particles\n"
                        "(multiple texCoords vecs)";
            Sprite sprite;
            sprite.color = glm::vec4(0.f, 0.f, 0.f, 1.f);
            sprite.position = text.position - glm::vec2(10.f, 10.f);
            sprite.size = text.getSize() + glm::vec2(20.f, 20.f);
            renderer.render(sprite);
            renderer.render(text);
        }
    }
    renderer.end_batching();
    if(is_pp)
    {
        pp_unit.endRender(2);
        pp_unit.apply_effect(red_effect);
        pp_unit.apply_effect(red_effect);
        pp_unit.render(false);
    }

    {
        renderer.beg_batching();
        Text text(&font);
        text.text = "frametime = " + std::to_string(frametime)
                + " v_sync:";
        if(v_sync)
            text.text += "ON";
        else
            text.text += "OFF";
        text.position = glm::vec2(10.f, 10.f);

        Sprite sprite;
        sprite.color = glm::vec4(0.f, 0.f, 0.f, 0.3f);
        sprite.position = text.position;
        sprite.size = text.getSize();
        renderer.render(sprite);
        renderer.render(text);
        renderer.end_batching();
    }

#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(150.f, 450.f), ImGuiSetCond_::ImGuiSetCond_Once);
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
        else if(event.key.keysym.sym == SDLK_3)
            is_pp = !is_pp;
        else if(event.key.keysym.sym == SDLK_2)
            sound_system.play_sample(sample, 10);
        else if(event.key.keysym.sym == SDLK_ESCAPE)
            App::should_close = true;
    }
}
