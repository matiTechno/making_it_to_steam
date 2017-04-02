#include "anim_creator.hpp"

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
{(void)event;}

void Anim_creator::render2()
{
    set_grid();
    renderer.load_projection(glm::vec4(0, 0, coords.size));
    renderer.rend_particles(grids);
    if(texture)
    {
        renderer.load_projection(get_camera());

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
}

void Anim_creator::render_ImGui()
{
    ImGui::ShowTestWindow();

    ImGui::Begin("animation creator", nullptr);
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
            goto anim_error;

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
anim_error:
        if(ImGui::BeginPopup("anim name error"))
        {
            ImGui::Text("animation name must be unique and not empty");
            ImGui::Separator();
            if(ImGui::Button("OK"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }
end:
    ImGui::End();
}

void Anim_creator::load_texture(const std::string& filename)
{
    try
    {
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
