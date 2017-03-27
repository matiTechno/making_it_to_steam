#include "anim_creator.hpp"
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>

Anim_creator::Anim_creator():
    input(100, 0)
{
    set_coords();
    reset_camera();
}

// advance origin setting:
// * N and DEL keys disabled -- done
// * texture rendering disabled -- done
// * if new texture is loaded quit adv origin --- done
// * saved previous frames coords --- done
// * animating when in edit mode --- done
// * keep order of frames after quiting preview in adv origin mode --- done
// set frames in adv origin mode:
// - use texture --- done
// - no small boxes visible --- done
// - inactive box color is 1111 --- done
// - active box color is the same --- done
// - no snapping to grid --- done
// - and useable (box resizing) -- done
// - disable move lock -- done
// * exit advance origin button --- done
// * enter origin button --- done
// * make clean when texture is on and when not too many states --- done

// * when origin point for frame gets updated - when pressed s on focused frame, and also when other press frame sets
// itself to origin
// * when preview anim show frame property, stop animation etc

// * new panel with boxes transparencies
// * in origin edit mode --- ad option to set new origin for all
// * also reload preview on go
// * also frame selection panel - when one frame covers other you know
// * make diffrent functions for diffrent stages/ animation / adv origin
// * saving to fails / warnings / etc

void Anim_creator::processEvent(const SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN && !animation && !is_adv_origin)
    {
        if(!is_point_in_im_win(cursor_pos))
        {
            if(event.key.keysym.sym == SDLK_n)
            {
                glm::vec2 size(100.f, 100.f);
                if(frames.size())
                {
                    size = glm::vec2(frames.front()->getCoords().z, frames.front()->getCoords().w);
                    frames.front()->deselect();
                }
                frames.push_front(std::make_unique<Anim_frame>(get_cursor_cam_pos(cursor_pos.x, cursor_pos.y, camera)
                                                               - size / 2.f, size, id, ft_for_all, origin_for_all));
                ++id;
            }
            else if(event.key.keysym.sym == SDLK_DELETE)
                if(frames.size() && frames.front()->get_is_selected())
                    frames.erase(frames.begin());
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
    {
        Scene_coords prev_coords = coords;
        set_coords();
        glm::vec2 scale = glm::vec2(coords.size) / glm::vec2(prev_coords.size);
        camera.z *= scale.x;
        camera.w *= scale.y;
    }

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
            else if(event.button.button == SDL_BUTTON_LEFT && !animation)
            {
                for(auto it = frames.begin(); it != frames.end(); ++it)
                {
                    if((*it)->on_left_button_press(event.button.x, event.button.y, camera, is_adv_origin))
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
    if(is_adv_origin)
    {

    }
}

void Anim_creator::update()
{
    if(is_adv_origin)
    {
        for(auto& frame: frames)
        {
            frame->set_origin(glm::vec2(glm::length(frame->getCoords().x - adv_origin_pos.x) / frame->getCoords().z,
                                        glm::length(frame->getCoords().y - adv_origin_pos.y) / frame->getCoords().w));
        }
    }
}

void Anim_creator::render()
{
    renderer.beg_batching();
    renderer.load_projection(glm::vec4(0.f, 0.f, coords.size));
    for(auto& grid: grids)
        renderer.render(grid);
    if(animation)
    {
        Sprite sprite;
        sprite.position = coords.pos;
        sprite.size = coords.size;
        sprite.color = glm::vec4(0.f, 0.f, 0.f, 0.5f);
        renderer.render(sprite);
    }
    renderer.end_batching();
    renderer.beg_batching();
    renderer.load_projection(camera);
    if(!animation)
    {
        if(texture && !is_adv_origin)
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
        if(!is_adv_origin)
            for(auto it = frames.rbegin(); it != frames.rend(); ++it)
            {(*it)->render(renderer);}
        else
            for(auto it = frames.rbegin(); it != frames.rend(); ++it)
            {
                Sprite sprite = (*it)->get_sprite();
                sprite.texture = &*texture;
                sprite.texCoords = saved_coords.at((*it)->get_id());

                if((*it)->get_is_selected())
                {
                    Sprite rect;
                    rect.position = sprite.position;
                    rect.size = sprite.size;
                    rect.color = glm::vec4(0.f, 1.f, 0.f, 0.3f);
                    renderer.render(rect);
                }
                renderer.render(sprite);
            }
        if(is_adv_origin)
        {
            Sprite sprite;
            sprite.size = glm::vec2(adv_origin_border_size);
            sprite.position = adv_origin_pos - adv_origin_border_size / 2.f;
            sprite.color = glm::vec4(1.f, 0.f, 0.f, 0.3f);
            renderer.render(sprite);
            sprite.size = glm::vec2(1.f, 1.f);
            sprite.position = glm::vec2(adv_origin_pos - 0.5f);
            sprite.color.a = 1.f;
            renderer.render(sprite);
        }
    }
    else
    {
        animation->update(dt);
        renderer.render(animation->get_sprite());
    }
    renderer.end_batching();
}

void Anim_creator::set_sprite()
{
    reset_camera();
    tex_sprite.sampl_type = Sampl_type::nearest;
    tex_sprite.texture = &*texture;
    tex_sprite.size = texture->getSize();
    tex_sprite.texCoords = glm::ivec4(0, 0, texture->getSize());
    tex_sprite.position = glm::vec2(camera.x, camera.y);
}

void Anim_creator::set_coords()
{
    coords.size = App::get_fb_size();

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
                    "DEL - delete frame\n"
                    "LMB - operate on frames\n"
                    "RMB - move camera\n"
                    "scroll - zoom");
        if(ImGui::Button("load test texture"))
            load_texture("game1/res/explo.png");
        if(ImGui::InputText("filename", input.data(), input.size(), ImGuiInputTextFlags_EnterReturnsTrue))
            load_texture(input.data());
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
        if(ImGui::InputFloat("global frametime", &ft_for_all, 0.01f, 0, 3))
        {
            if(ft_for_all < min_ft)
                ft_for_all = min_ft;
            for(auto& frame: frames)
                *frame->get_frametime_ptr() = ft_for_all;
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
        {
            ImGui::Separator();
            ImGui::InputFloat("scale", &animation_scale, 0, 0, 2);
            if(ImGui::Button("preview animation") && texture && frames.size())
            {


                std::vector<Frame_data> data;
                data.reserve(frames.size());
                frames.front()->deselect();
                frames.sort([](const std::unique_ptr<Anim_frame>& f1, const std::unique_ptr<Anim_frame>& f2)
                {return f1->get_id() < f2->get_id();});
                for(auto& frame: frames)
                {
                    if(!is_adv_origin)
                        data.emplace_back(Frame_data{frame->getCoords(), *frame->get_frametime_ptr(), frame->get_origin()});
                    else
                        data.emplace_back(Frame_data{saved_coords.at(frame->get_id()),
                                                     *frame->get_frametime_ptr(), frame->get_origin()});
                }

                glm::vec2 cam_middle(camera.x + camera.z / 2.f, camera.y + camera.w / 2.f);
                reset_camera();
                camera.x = cam_middle.x - camera.z / 2.f;
                camera.y = cam_middle.y - camera.w / 2.f;

                animation = std::make_unique<Animation>(data, &*texture, animation_scale,
                                                        glm::vec2(camera.x + camera.z / 3.f, camera.y + camera.w / 3.f),
                                                        true);


            }
            if(ImGui::Button("end preview"))
            {
                if(is_adv_origin)
                    frames.reverse();
                animation.reset();
            }
        }
        if(texture)
        {
            ImGui::Separator();
            ImGui::Text("texture size: %d x %d", texture->getSize().x, texture->getSize().y);
        }
        if(ImGui::Button("enter advanced origin setting") && texture)
        {
            saved_coords.clear();
            is_adv_origin = true;
            adv_origin_pos = glm::vec2(camera.x + camera.z / 3.f, camera.y + camera.w / 3.f);
            for(auto& frame: frames)
                saved_coords.emplace(frame->get_id(), frame->getCoords());
        }
        if(ImGui::Button("quit origin setting"))
            quit_adv_origin_setting();

        if(frames.size() && frames.front()->get_is_selected())
        {
            Anim_frame& frame = *frames.front();
            ImGui::Separator();
            ImGui::Text("id: %lu", frame.get_id());
            ImGui::Text("texCoords: %d, %d, %d, %d", frame.getCoords().x, frame.getCoords().y,
                        frame.getCoords().z, frame.getCoords().w);
            ImGui::InputFloat("frametime", frame.get_frametime_ptr(), 0.01f, 0, 3);
            if(*frame.get_frametime_ptr() < min_ft)
                *frame.get_frametime_ptr() = min_ft;
            ImGui::Text("origin: %.2f, %.2f", static_cast<double>(frame.get_origin().x),
                        static_cast<double>(frame.get_origin().y));
        }
    }
    ImGui::End();
}

void Anim_creator::set_origin_for_all(const glm::vec2& origin)
{
    for(auto& frame: frames)
        frame->set_origin(origin);
    origin_for_all = origin;
}

void Anim_creator::reset_camera()
{
    camera_scale = 1.f;
    camera = glm::vec4(0.f, 0.f, coords.size.x, coords.size.y);
}

void Anim_creator::load_texture(const std::string& filename)
{
    try
    {
        if(animation)
            animation.reset();
        if(is_adv_origin)
            quit_adv_origin_setting();

        texture = std::make_unique<Texture>(filename);
        set_sprite();
    }
    catch(const std::exception& e)
    {
        ImGui::OpenPopup("error");
        err_msg = e.what();
    }
}

void Anim_creator::quit_adv_origin_setting()
{
    is_adv_origin = false;
    for(auto& frame: frames)
        frame->set_position(glm::vec2(saved_coords.at(frame->get_id()).x, saved_coords.at(frame->get_id()).y));

    if(frames.size())
        frames.front()->deselect();
}
