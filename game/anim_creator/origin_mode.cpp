#include "origin_mode.hpp"
#include "preview.hpp"

Origin_mode::Origin_mode(std::list<Anim_rect>& frames, const Texture& texture, float* global_frametime):
    frames(frames),
    texture(texture),
    global_frametime(global_frametime)
{
    is_opaque = false;
    camera.x = frames.front().get_coords().x;
    camera.y = frames.front().get_coords().y;

    for(auto it = ++frames.begin(); it != frames.end(); ++it)
    {
        if(it->get_coords().x < camera.x)
            camera.x = it->get_coords().x;
        if(it->get_coords().y < camera.y)
            camera.y = it->get_coords().y;
    }
    camera.x -= 100.f;
    camera.y -= 100.f;

    origin_pos = glm::ivec2(camera.x, camera.y) + glm::ivec2(camera.z, camera.w) / 2;

    for(auto& frame: frames)
        saved_coords.emplace(frame.id, frame.get_coords());
}

void Origin_mode::render_ImGui()
{
    ImGui::Begin("origin mode");
    ImGui::PushItemWidth(200);
    if(ImGui::Button("quit"))
    {
        for(auto& frame: frames)
            frame.set_position(glm::vec2(saved_coords.at(frame.id).x, saved_coords.at(frame.id).y));

        num_scenes_to_pop = 1;
    }
    if(ImGui::InputFloat("global frametime", global_frametime, 0.01f, 0, 3))
    {
        if(*global_frametime < 0.016f)
            *global_frametime = 0.016f;
        for(auto& frame: frames)
            frame.frametime = *global_frametime;
    }
    if(ImGui::Button("preview"))
    {
        std::list<Anim_rect> preview_frames = frames;
        for(auto& frame: preview_frames)
            frame.set_position(glm::vec2(saved_coords.at(frame.id).x, saved_coords.at(frame.id).y));
        set_new_scene<Preview>(preview_frames, 1.f, texture);
    }

    if(frames.front().is_selected)
    {
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("id: %lu", frames.front().id);
        if(ImGui::InputFloat("frametime", &frames.front().frametime, 0.01f, 0, 3))
            if(frames.front().frametime < 0.016f)
                frames.front().frametime = 0.016f;
        ImGui::Text("orgin: %.2f, %.2f", static_cast<double>(frames.front().origin.x),
                    static_cast<double>(frames.front().origin.y));
        ImGui::SliderFloat("alpha", &frames.front().alpha, 0, 1.f, "%.2f");
    }
    ImGui::End();
}

void Origin_mode::processEvent2(const SDL_Event& event)
{
    if(event.type == SDL_MOUSEMOTION && !get_is_rmb_pressed())
    {
        if(frames.front().is_selected)
            frames.front().on_mouse_motion(event.motion.xrel, event.motion.yrel, get_camera_scale());
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        for(auto it = frames.begin(); it != frames.end(); ++it)
        {
            if(it->on_left_button_press(event.button.x, event.button.y, camera, true))
            {
                if(&*it != &frames.front())
                {
                    frames.push_front(std::move(*it));
                    frames.erase(it);
                }
                break;
            }
        }

    else if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
        frames.front().on_left_button_release();
}

void Origin_mode::render2()
{
    if(!is_on_top())
        return;

    if(ImGui_wants_input)
        frames.front().on_left_button_release();

    for(auto& frame: frames)
        frame.origin = glm::vec2(origin_pos - glm::ivec2(frame.get_coords().x, frame.get_coords().y))
                / glm::vec2(frame.get_coords().z, frame.get_coords().w);

    renderer.load_projection(camera);
    renderer.beg_batching();
    {
        Sprite sprite;
        sprite.position = glm::vec2(origin_pos) - origin_rect_size / 2.f;
        sprite.size = origin_rect_size;
        sprite.color = glm::vec4(1.f, 0, 0, 0.1f);
        renderer.render(sprite);
    }
    for(auto it = frames.rbegin(); it != frames.rend(); ++it)
        it->render(renderer, texture, saved_coords.at(it->id));
    {
        Sprite sprite;
        sprite.position = origin_pos;
        sprite.size = glm::vec2(1.f);
        sprite.color = glm::vec4(1.f, 0, 0, 1.f);
        renderer.render(sprite);
    }
    renderer.end_batching();
}
