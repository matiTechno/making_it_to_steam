#include "anim_creator.hpp"
#include "preview.hpp"
#include "origin_mode.hpp"

Anim_creator::Anim_creator():
    tex_filename_input(100, 0),
    anim_name_input(100, 0),
    anim_rename_input(100, 0)
{set_grid();}

void Anim_creator::on_quit_event()
{
    App::should_close = true;
}

void Anim_creator::processEvent2(const SDL_Event& event)
{
    if(!anim)
        return;

    if(event.type == SDL_KEYDOWN && !event.key.repeat)
    {
        if(event.key.keysym.sym == SDLK_n)
        {
            if(anim->frames.size())
            {
                anim->new_frame_size = glm::ivec2(anim->frames.front().get_coords().z, anim->frames.front().get_coords().w);
                anim->frames.front().is_selected = false;
            }
            anim->frames.emplace_front(anim->id, get_cursor_cam_pos(get_cursor_pos().x, get_cursor_pos().y, camera)
                                       - glm::vec2(anim->new_frame_size) / 2.f,
                                       anim->new_frame_size, anim->global_frametime, anim->global_origin);
            ++anim->id;
        }
        else if(event.key.keysym.sym == SDLK_r && anim->frames.size() && anim->frames.front().is_selected)
            anim->frames.erase(anim->frames.begin());
    }
    else if(event.type == SDL_MOUSEMOTION && anim->frames.size() && !get_is_rmb_pressed())
    {
        if(anim->frames.front().is_selected)
            anim->frames.front().on_mouse_motion(event.motion.xrel, event.motion.yrel, get_camera_scale());
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        for(auto it = anim->frames.begin(); it != anim->frames.end(); ++it)
        {
            if(it->on_left_button_press(event.button.x, event.button.y, camera, false))
            {
                if(&*it != &anim->frames.front())
                {
                    anim->frames.push_front(std::move(*it));
                    anim->frames.erase(it);
                }
                break;
            }
        }

    else if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && anim->frames.size())
        anim->frames.front().on_left_button_release();
}

void Anim_creator::render2()
{
    if(ImGui_wants_input && anim && anim->frames.size())
        anim->frames.front().on_left_button_release();

    set_grid();
    renderer.load_projection(glm::vec4(0, 0, coords.size));
    renderer.rend_particles(grids);

    if(!is_on_top())
        return;

    if(texture)
    {
        renderer.load_projection(camera);

        Sprite shadow;
        shadow.position = tex_sprite.position;
        shadow.size = tex_sprite.size;
        shadow.color = glm::vec4(0, 0, 0, 0.1f);
        renderer.render(shadow);

        if(countures)
            tex_sprite.src_alpha = GL_ONE;
        else
            tex_sprite.src_alpha = GL_SRC_ALPHA;
        renderer.render(tex_sprite);
    }
    if(anim)
        for(auto it = anim->frames.rbegin(); it != anim->frames.rend(); ++it)
            it->render(renderer);
}

void Anim_creator::render_ImGui()
{
    //ImGui::ShowTestWindow();

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.7f));
    ImGui::Begin("animation creator", nullptr);
    ImGui::PushItemWidth(200);
    {
        ImGui::Text("Welcome to animation creator!\nm a t i T e c h n o");
        ImGui::Spacing();
        if(ImGui::Button("load example texture"))
            load_texture("res/i8aic.png");
        if(ImGui::InputText("texture filename", tex_filename_input.data(), tex_filename_input.size(),
                            ImGuiInputTextFlags_EnterReturnsTrue))
            load_texture(tex_filename_input.data());

        if(ImGui::BeginPopup("tex_load_failed"))
        {
            ImGui::Text("%s", err_msg.c_str());
            ImGui::Separator();
            if(ImGui::Button("OK"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        if(!texture)
            goto end;

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("texture filename: %s", tex_filename.c_str());
        ImGui::Text("size: %d x %d", texture->getSize().x, texture->getSize().y);
        ImGui::Checkbox("countures", &countures);

        ImGui::Separator();
        ImGui::Spacing();
        if(ImGui::InputText("new animation", anim_name_input.data(), anim_name_input.size(),
                            ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if(anim_name_input.front() == 0)
            {
                ImGui::OpenPopup("anim name error");
                goto out1;
            }
            for(auto& name: store_anim_names)
                if(name == anim_name_input.data())
                {
                    ImGui::OpenPopup("anim name error");
                    goto out1;
                }
            animations.emplace(anim_name_input.data(), Animation{});
            store_anim_names.emplace_back(anim_name_input.data());
            anim_names.emplace_back(store_anim_names.back().c_str());
            current_anim_name = static_cast<int>(anim_names.size() - 1);
            anim_name_input.front() = 0;
        }
out1:
        ImGui::Combo("animations", &current_anim_name, anim_names.data(), static_cast<int>(anim_names.size()));
        if(current_anim_name != -1)
            anim = &animations.at(anim_names[static_cast<std::size_t>(current_anim_name)]);
        else
            goto end;

        if(ImGui::Button("delete"))
        {
            animations.erase(anim_names[static_cast<std::size_t>(current_anim_name)]);
            {
                auto it = store_anim_names.begin();
                std::advance(it, current_anim_name);
                store_anim_names.erase(it);
            }
            {
                auto it = anim_names.begin() + current_anim_name;
                anim_names.erase(it);
            }
            current_anim_name = -1;
            anim = nullptr;
            goto end;
        }
        ImGui::SameLine();
        if(ImGui::InputText("rename", anim_rename_input.data(), anim_rename_input.size(),
                            ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if(std::string(anim_rename_input.data()) == anim_names[static_cast<std::size_t>(current_anim_name)])
            {
                anim_rename_input.front() = 0;
                goto out2;
            }

            if(anim_rename_input.front() == 0)
            {
                ImGui::OpenPopup("anim name error");
                goto out2;
            }
            for(auto& name: store_anim_names)
                if(name == anim_rename_input.data())
                {
                    ImGui::OpenPopup("anim name error");
                    goto out2;
                }
            Animation temp = animations.at(anim_names[static_cast<std::size_t>(current_anim_name)]);
            animations.erase(anim_names[static_cast<std::size_t>(current_anim_name)]);
            auto it = store_anim_names.begin();
            std::advance(it, current_anim_name);
            *it = anim_rename_input.data();
            anim_names[static_cast<std::size_t>(current_anim_name)] = it->c_str();
            anim = &animations.emplace(anim_names[static_cast<std::size_t>(current_anim_name)], temp).first->second;
            anim_rename_input.front() = 0;
        }
out2:
        ImGui::Separator();
        ImGui::Spacing();
        if(ImGui::InputFloat("global frametime", &anim->global_frametime, 0.01f, 0, 3))
        {
            if(anim->global_frametime < 0.016f)
                anim->global_frametime = 0.016f;

            for(auto& frame: anim->frames)
                frame.frametime = anim->global_frametime;
        }
        ImGui::Spacing();
        ImGui::Text("global origin");
        if(ImGui::Button("l-t"))
        {set_origin_for_all(glm::vec2(0.f, 0.f));}
        ImGui::SameLine(); if(ImGui::Button("m-t"))
        {set_origin_for_all(glm::vec2(0.5f, 0.f));}
        ImGui::SameLine(); if(ImGui::Button("r-t"))
        {set_origin_for_all(glm::vec2(1.f, 0.f));}
        if(ImGui::Button("l-m"))
        {set_origin_for_all(glm::vec2(0.f, 0.5f));}
        ImGui::SameLine(); if(ImGui::Button("m-m"))
        {set_origin_for_all(glm::vec2(0.5f, 0.5f));}
        ImGui::SameLine(); if(ImGui::Button("r-m"))
        {set_origin_for_all(glm::vec2(1.f, 0.5f));}
        if(ImGui::Button("l-b"))
        {set_origin_for_all(glm::vec2(0.f, 1.f));}
        ImGui::SameLine(); if(ImGui::Button("m-b"))
        {set_origin_for_all(glm::vec2(0.5f, 1.f));}
        ImGui::SameLine(); if(ImGui::Button("r-b"))
        {set_origin_for_all(glm::vec2(1.f, 1.f));}

        if(!anim->frames.size())
            goto end;

        ImGui::Separator();
        ImGui::Spacing();
        if(ImGui::Button("preview animation"))
            set_new_scene<Preview>(anim->frames, preview_scale, *texture);

        ImGui::SameLine();
        ImGui::InputFloat("scale", &preview_scale, 0, 0, 2);
        if(ImGui::Button("enter origin mode"))
            set_new_scene<Origin_mode>(anim->frames, *texture, &anim->global_frametime);

        if(ImGui::Button("set collision data"))
        {}
        {
            std::vector<const char*> anim_to_compare_names;
            int num_anim_to_compare = 0;
            for(auto& anim: animations)
                if(anim.second.frames.size() && &anim.second != this->anim)
                {
                    ++num_anim_to_compare;
                    anim_to_compare_names.push_back(anim.first.c_str());
                }

            if(num_anim_to_compare > 0)
            {
                if(anim_to_compare < 0)
                    anim_to_compare = 0;
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Text("adjust to");
                ImGui::Combo("##to_compare", &anim_to_compare, anim_to_compare_names.data(), num_anim_to_compare);
                if(ImGui::Button("start"))
                {}
            }
        }
        if(!anim->frames.front().is_selected)
            goto end;

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("id: %lu", anim->frames.front().id);
        if(ImGui::InputFloat("frametime", &anim->frames.front().frametime, 0.01f, 0, 3))
            if(anim->frames.front().frametime < 0.016f)
                anim->frames.front().frametime = 0.016f;
        ImGui::Text("orgin: %.2f, %.2f", static_cast<double>(anim->frames.front().origin.x),
                    static_cast<double>(anim->frames.front().origin.y));
        ImGui::Text("coordinates: %d, %d, %d, %d", anim->frames.front().get_coords().x, anim->frames.front().get_coords().y,
                    anim->frames.front().get_coords().z, anim->frames.front().get_coords().w);
end:
        if(ImGui::BeginPopup("anim name error"))
        {
            ImGui::Text("animation name must be unique and not empty");
            ImGui::Separator();
            if(ImGui::Button("OK"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void Anim_creator::load_texture(const std::string& filename)
{
    try
    {
        clear();
        texture = std::make_unique<Texture>(filename);
        tex_filename = filename;
        set_sprite();
    }
    catch(const std::exception& e)
    {
        err_msg = e.what();
        ImGui::OpenPopup("tex_load_failed");
    }
}

void Anim_creator::set_sprite()
{
    reset_camera();
    tex_sprite.position = glm::vec2(0, 0);
    tex_sprite.size = texture->getSize();
    tex_sprite.sampl_type = Sampl_type::nearest;
    tex_sprite.texCoords = glm::ivec4(0, 0, texture->getSize());
    tex_sprite.texture = texture.get();
}

void Anim_creator::clear()
{
    texture.release();
    animations.clear();
    anim_names.clear();
    store_anim_names.clear();
    current_anim_name = -1;
    anim = nullptr;
}

void Anim_creator::set_origin_for_all(const glm::vec2& origin)
{
    anim->global_origin = origin;
    for(auto& frame: anim->frames)
        frame.origin = origin;
}

void Anim_creator::set_grid()
{
    if(max_fb_size.x >= coords.size.x && max_fb_size.y >= coords.size.y)
        return;
    max_fb_size = coords.size;
    grids.vbo_data.clear();

    int num_in_row = 0;
    glm::ivec2 pos(0, 0);
    glm::vec4 color;
    bool is_dark{};
    bool first_row = true;

    while(pos.x < coords.size.x && pos.y < coords.size.y)
    {
        if(first_row)
            ++num_in_row;
        if(is_dark)
            color = color1;
        else
            color = color2;

        grids.vbo_data.emplace_back(Vbo_p{pos, glm::vec2(grid_size), color});

        is_dark = !is_dark;
        pos.x += grid_size;
        if(pos.x >= coords.size.x)
        {
            pos.x = 0;
            pos.y += grid_size;
            if(num_in_row % 2 == 0)
                is_dark = !is_dark;
            first_row = false;
        }
    }
    grids.num_to_render = grids.vbo_data.size();
}
