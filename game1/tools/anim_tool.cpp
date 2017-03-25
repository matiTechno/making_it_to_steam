#include "anim_tool.hpp"
#include <glm/exponential.hpp>

Anim_tool::Anim_tool():
    font(font_loader.loadFont("game1/res/ProggyClean.ttf", 20)),
    input(100, 0)
{
    set_scene();
}

void Anim_tool::processEvent(const SDL_Event& event)
{
    if(event.type == SDL_MOUSEMOTION)
        frame.on_mouse_motion(event.motion.xrel, event.motion.yrel, camera_scale);
    if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
        frame.on_left_button_release();
    if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        frame.on_left_button_press(event.button.x, event.button.y, camera);

//    // selection rects
//    if(rects.size())
//    {
//        if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && is_in_tex(event.button.x, event.button.y))
//        {
//            auto it = rects.rbegin();
//            while(it != rects.rend())
//            {
//                if(it->left_button_press_event(glm::ivec2(event.button.x, event.button.y), camera))
//                {
//                    if(it != rects.rbegin())
//                    {
//                        rects.emplace_back(*it);
//                        rects.erase(std::next(it).base());
//                    }
//                    goto lolz;
//                }
//                ++it;
//            }
//        }
//        else
//        {
//lolz:
//            if(rects.back().is_active)
//                rects.back().processInput_when_active(event, camera_scale);
//        }
//    }
//    // ...
    if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
    {
        if(is_in_tex(event.button.x, event.button.y))
            is_rb_pressed = true;
    }
    else if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT)
        is_rb_pressed = false;

    else if(event.type == SDL_MOUSEMOTION)
    {
        if(is_rb_pressed)
        {
            camera.x -= event.motion.xrel * camera_scale;
            camera.y -= event.motion.yrel * camera_scale;
        }
        cursor_pos = glm::vec2(event.motion.x, event.motion.y);
    }
    else if(event.type == SDL_WINDOWEVENT)
    {
        if(event.window.event == SDL_WINDOWEVENT_RESIZED)
            set_scene();
    }
    else if(event.type == SDL_MOUSEWHEEL && is_in_tex(cursor_pos.x, cursor_pos.y))
    {
        glm::vec2 prev_pos = get_cursor_camera_space(cursor_pos.x, cursor_pos.y);

        float zoom = glm::pow(zoom_factor, static_cast<float>(-event.wheel.y));
        camera_scale *= zoom;
        camera.z *= zoom;
        camera.w *= zoom;

        glm::vec2 pos = get_cursor_camera_space(cursor_pos.x, cursor_pos.y);

        camera.x -= pos.x - prev_pos.x;
        camera.y -= pos.y - prev_pos.y;;
    }
//    else if(event.type == SDL_KEYDOWN && !event.key.repeat && event.key.keysym.sym == SDLK_n)
//    {
//        glm::vec2 size(50.f, 50.f);
//        if(rects.size())
//        {
//            rects.back().is_active = false;
//            size  = glm::vec2(rects.back().getCoords().z, rects.back().getCoords().w);
//        }
//        rects.emplace_back(get_cursor_camera_space(cursor_pos.x, cursor_pos.y) - size / 2.f, size, rects.size());
//    }
//    else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DELETE)
//    {
//        if(rects.size() && rects.back().is_active == true)
//            rects.erase(rects.end() - 1);
//    }
}

void Anim_tool::render()
{
    renderer.load_projection(glm::vec4(coords.pos, coords.size));
    renderer.beg_batching();
    {
        for(auto& grid: grids)
            renderer.render(grid);
    }
    {
        Text text(font);
        text.position = glm::vec2(10.f, 10.f);
        text.text = "matiTechno animation tool. e n j o y.";
        Sprite sprite;
        sprite.size = text.getSize() + 20.f;
        sprite.position = text.position - 10.f;
        sprite.color = glm::vec4(0.f, 0.f, 0.f, 0.3f);
        renderer.render(sprite);
        renderer.render(text);
    }
    renderer.end_batching();
    renderer.load_projection(camera);
    renderer.beg_batching();
    if(texture)
    {
        {
            Sprite shadow;
            shadow.color = glm::vec4(0.f, 0.f, 0.f, 0.1f);
            shadow.position = sprite.position;
            shadow.size = sprite.size;
            renderer.render(shadow);
        }
        renderer.render(sprite);
    }
//    {
//        for(auto& rect: rects)
//            rect.render(renderer);
//    }
    frame.render(renderer);

    renderer.end_batching();
}

void Anim_tool::render_ImGui()
{
    //ImGui::ShowTestWindow();
    ImGui::SetNextWindowPosCenter(ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiSetCond_FirstUseEver);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(55, 32, 41, 179));
    ImGui::Begin("animation creator");
    {
        window_size = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
        window_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

        ImGui::Text("Welcome to animation creator!\n");
        if(ImGui::InputText("texture filename", input.data(), input.size(), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            try
            {
                texture = std::make_unique<Texture>(input.data());
                set_sprite();
            }
            catch(const std::exception& e)
            {
                ImGui::OpenPopup("error");
                err_msg = e.what();
            }
        }
        if(ImGui::BeginPopupModal("error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", err_msg.c_str());
            if(ImGui::Button("OK", ImVec2(120, 0)))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        if(ImGui::Button("set texture contours visible"))
        {
            sprite.src_alpha = GL_ONE;
            sprite.dst_alpha = GL_ONE_MINUS_SRC_ALPHA;
        }
        if(ImGui::Button("revert"))
        {
            sprite.src_alpha = GL_SRC_ALPHA;
            sprite.dst_alpha = GL_ONE_MINUS_SRC_ALPHA;
        }
        if(ImGui::Button("test_texture"))
        {
            texture = std::make_unique<Texture>("game1/res/explo.png");
            set_sprite();
        }
//        if(rects.size() && rects.back().is_active)
//        {
//            ImGui::Separator();
//            ImGui::Text("SELECTED FRAME");
//            std::string string("frame index = " + std::to_string(rects.back().id));
//            ImGui::BulletText("%s", string.c_str());
//            ImGui::BulletText("coords: %d, %d, %d, %d", int(rects.back().b_main_body.pos.x), int(rects.back().b_main_body.pos.y),
//                              int(rects.back().b_main_body.size.x), int(rects.back().b_main_body.size.y));
//            //ImGui::InputFloat("frametime", &rects.back().frametime);
//        }
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

bool Anim_tool::is_in_tex(int x, int y)
{
    glm::vec2 pos = get_cursor_camera_space(x, y);
    if(pos.x >= sprite.position.x && pos.x <=  sprite.size.x &&
            pos.y >= sprite.position.y && pos.y <= sprite.size.y)
        return true;
    return false;
}

void Anim_tool::set_sprite()
{
    sprite.sampl_type = Sampl_type::nearest;
    sprite.position = glm::vec2(0.f, 0.f);
    sprite.size = texture->getSize();
    sprite.texCoords = glm::ivec4(0, 0, texture->getSize());
    sprite.texture = &*texture;
    camera.x = sprite.position.x;
    camera.y = sprite.position.y;
    camera.z = coords.size.x;
    camera.w = coords.size.y;
    camera_scale = 1.f;
}

void Anim_tool::set_scene()
{
    coords.size = App::get_fb_size();
    camera.z = coords.size.x;
    camera.w = coords.size.y;
    if(camera_scale != 1.f)
    {
        camera_scale = 1.f;
        camera.x = sprite.position.x;
        camera.y = sprite.position.y;
    }

    // grid
    grids.clear();
    int num_in_row = 0;
    Sprite grid;
    grid.size = glm::vec2(grid_size);
    grid.position = coords.pos;
    bool is_dark = false;

    while(grid.position.x < coords.pos.x + coords.size.x &&
          grid.position.y < coords.pos.y + coords.size.y)
    {
        ++num_in_row;
        if(is_dark)
            grid.color = grid_grey;
        else
            grid.color = grid_white;
        grids.push_back(grid);

        is_dark = !is_dark;
        grid.position.x += grid_size;
        if(grid.position.x >= coords.pos.x + coords.size.x)
        {
            grid.position.x = coords.pos.x;
            grid.position.y += grid_size;
            if(num_in_row % 2 == 0)
                is_dark = !is_dark;
            num_in_row = 0;
        }
    }
}

glm::vec2 Anim_tool::get_cursor_camera_space(int x, int y)
{
    return glm::vec2(camera.x, camera.y) + glm::vec2(camera.z, camera.w) / glm::vec2(coords.size) * glm::vec2(x, y);
}

