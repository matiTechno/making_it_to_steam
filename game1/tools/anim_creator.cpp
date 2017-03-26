#include "anim_creator.hpp"
#include <glm/exponential.hpp>

static void ShowHelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(450.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

Anim_creator::Anim_creator():
    input(100, 0)
{set_coords();}

void Anim_creator::processEvent(const SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN)
    {
        if(!is_point_in_im_win(cursor_pos))
        {
            if(event.key.keysym.sym == SDLK_n)
            {
                glm::vec2 size(50.f, 50.f);
                if(frames.size())
                {
                    size = glm::vec2(frames.front()->getCoords().z, frames.front()->getCoords().w);
                    frames.front()->deselect();
                }
                frames.push_front(std::make_unique<Anim_frame>(get_cursor_cam_pos(cursor_pos.x, cursor_pos.y, camera)
                                                               - size / 2.f, size, id));
                ++id;
            }
            else if(event.key.keysym.sym == SDLK_DELETE)
            {
                if(frames.size() && frames.front()->get_is_selected())
                    frames.erase(frames.begin());
            }
        }
    }
    else if(event.type == SDL_MOUSEMOTION)
    {
        cursor_pos = glm::ivec2(event.motion.x, event.motion.y);
        if(is_rb_pressed)
        {
            camera.x -= event.motion.xrel * camera_scale;
            camera.y -= event.motion.yrel * camera_scale;
        }
        if(frames.size() && !is_rb_pressed)
            frames.front()->on_mouse_motion(event.motion.xrel, event.motion.yrel, camera_scale);
    }
    else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
        set_coords();

    else if(event.type == SDL_MOUSEWHEEL)
    {
        if(!is_point_in_im_win(cursor_pos))
        {
            glm::vec2 prev_pos = get_cursor_cam_pos(cursor_pos.x, cursor_pos.y, camera);
            float zoom = glm::pow(zoom_factor, static_cast<float>(-event.wheel.y));
            camera_scale *= zoom;
            camera.z *= zoom;
            camera.w *= zoom;
            glm::vec2 pos = get_cursor_cam_pos(cursor_pos.x, cursor_pos.y, camera);
            camera.x -= pos.x - prev_pos.x;
            camera.y -= pos.y - prev_pos.y;;
        }
    }
    else if(event.type == SDL_MOUSEBUTTONUP)
    {
        if(event.button.button == SDL_BUTTON_RIGHT)
            is_rb_pressed = false;
        else if(event.button.button == SDL_BUTTON_LEFT)
            if(frames.size())
                frames.front()->on_left_button_release();
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        if(!is_point_in_im_win(cursor_pos))
        {
            if(event.button.button == SDL_BUTTON_RIGHT)
                is_rb_pressed = true;
            else if(event.button.button == SDL_BUTTON_LEFT)
            {
                for(auto it = frames.begin(); it != frames.end(); ++it)
                {
                    if((*it)->on_left_button_press(event.button.x, event.button.y, camera))
                    {
                        if(&*it != &frames.front())
                        {
                            frames.push_front(std::move(*it));
                            frames.erase(it);
                        }
                        break;
                    }
                }
            }
        }
    }
}

void Anim_creator::render()
{
    renderer.beg_batching();
    renderer.load_projection(glm::vec4(0.f, 0.f, coords.size));
    for(auto& grid: grids)
        renderer.render(grid);
    renderer.end_batching();
    renderer.beg_batching();
    renderer.load_projection(camera);
    if(texture)
    {
        Sprite shadow;
        shadow.size = tex_sprite.size;
        shadow.position = tex_sprite.position;
        shadow.color = glm::vec4(0.f, 0.f, 0.f, 0.1f);
        renderer.render(shadow);
        if(show_countures)
            tex_sprite.src_alpha = GL_ONE;
        else
            tex_sprite.src_alpha = GL_SRC_ALPHA;
        renderer.render(tex_sprite);
    }
    for(auto it = frames.rbegin(); it != frames.rend(); ++it)
    {(*it)->render(renderer);}
    renderer.end_batching();
}

void Anim_creator::set_sprite()
{
    set_camera();
    tex_sprite.sampl_type = Sampl_type::nearest;
    tex_sprite.texture = &*texture;
    tex_sprite.size = texture->getSize();
    tex_sprite.texCoords = glm::ivec4(0, 0, texture->getSize());
    tex_sprite.position = glm::vec2(camera.x, camera.y);
}

void Anim_creator::set_coords()
{
    coords.size = App::get_fb_size();
    set_camera();

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

bool Anim_creator::is_point_in_im_win(const glm::vec2& point)
{
    if(point.x >= w_pos.x && point.x < w_pos.x + w_size.x &&
            point.y >= w_pos.y && point.y < w_pos.y + w_size.y)
        return true;
    return false;
}

void Anim_creator::render_ImGui()
{
    ImGui::Begin("animation creator", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    w_pos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
    w_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
    {
        ImGui::Text("Welcome to animation creator!");
        ImGui::Text("m a t i T e c h n o\n"
                    "note: when writing to ImGui window\n"
                    "keep cursor in it's bounds or you will\n"
                    "trigger some hotkeys\n"
                    "i will find solution soon\n"
                    "N - new frame\n"
                    "DEL - delete selected frame\n"
                    "LMB - operate on frames\n"
                    "RMB - move camera\n"
                    "scroll - zoom");
        if(ImGui::Button("load test texture"))
        {
            try
            {
                texture = std::make_unique<Texture>("game1/res/explo.png");
                set_sprite();
            }
            catch(const std::exception& e)
            {
                ImGui::OpenPopup("error");
                err_msg = e.what();
            }
        }
        if(ImGui::InputText("filename", input.data(), input.size(), ImGuiInputTextFlags_EnterReturnsTrue))
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
        if(ImGui::Button("show / hide countures"))
            show_countures = !show_countures;

        if(ImGui::BeginPopup("error"))
        {
            ImGui::Text("%s", err_msg.c_str());
            ImGui::Separator();
            if(ImGui::Button("OK"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        {
            ImGui::InputFloat("global frametime", &ft_for_all, 0.f, 0.f, 3);
            if(ImGui::Button("set"))
            {
                for(auto& frame: frames)
                    *(frame->get_frametime_ptr()) = ft_for_all;
            }
            ImGui::SameLine();ShowHelpMarker("after adding new frames you probably\nwant to reuse it");
        }
        {
            ImGui::Text("set global origin");
            if(ImGui::Button("l-top"))
            {set_origin_for_all(glm::vec2(0.f, 0.f));}
            ImGui::SameLine(); if(ImGui::Button("mid-top"))
            {set_origin_for_all(glm::vec2(0.5f, 0.f));}
            ImGui::SameLine(); if(ImGui::Button("r-top"))
            {set_origin_for_all(glm::vec2(1.f, 0.f));}
            if(ImGui::Button("l-mid"))
            {set_origin_for_all(glm::vec2(0.f, 0.5f));}
            ImGui::SameLine(); if(ImGui::Button("mid"))
            {set_origin_for_all(glm::vec2(0.5f, 0.5f));}
            ImGui::SameLine(); if(ImGui::Button("r-mid"))
            {set_origin_for_all(glm::vec2(1.f, 0.5f));}
            if(ImGui::Button("l-bot"))
            {set_origin_for_all(glm::vec2(0.f, 1.f));}
            ImGui::SameLine(); if(ImGui::Button("mid-bot"))
            {set_origin_for_all(glm::vec2(0.5f, 1.f));}
            ImGui::SameLine(); if(ImGui::Button("r-bot"))
            {set_origin_for_all(glm::vec2(1.f, 1.f));}
        }
        if(texture)
        {
            ImGui::Separator();
            ImGui::Text("texture size: %d x %d", texture->getSize().x, texture->getSize().y);
        }
        if(frames.size() && frames.front()->get_is_selected())
        {
            Anim_frame& frame = *frames.front();
            ImGui::Separator();
            ImGui::Text("id: %d", frame.get_id());
            ImGui::Text("texCoords: %d, %d, %d, %d", frame.getCoords().x, frame.getCoords().y,
                        frame.getCoords().z, frame.getCoords().w);
            ImGui::InputFloat("frametime", frame.get_frametime_ptr(), 0.01f, 5.f, 3);
            ImGui::Text("origin: %.2f, %.2f", static_cast<double>(frame.get_origin().x),
                        static_cast<double>(frame.get_origin().y));
        }
    }
    ImGui::End();
}

void Anim_creator::set_camera()
{
    camera.x = coords.pos.x;
    camera.y = coords.pos.y;
    camera.z = coords.size.x;
    camera.w = coords.size.y;
    camera_scale = 1.f;
}

void Anim_creator::set_origin_for_all(const glm::vec2& origin)
{
    for(auto& frame: frames)
        frame->set_origin(origin);
}
