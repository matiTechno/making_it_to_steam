#include "camera_scene.hpp"
#include <glm/exponential.hpp>

Camera_scene::Camera_scene()
{
    reset_camera();
}

void Camera_scene::processEvent(const SDL_Event& event)
{
    if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        glm::vec2 scale = glm::vec2(App::get_fb_size()) / glm::vec2(coords.size);
        coords.size = App::get_fb_size();
        camera.z *= scale.x;
        camera.w *= scale.y;
    }

    if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
        is_rmb_pressed = true;

    else if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT)
        is_rmb_pressed = false;

    else if(event.type == SDL_MOUSEMOTION && is_rmb_pressed)
    {
        cursor_pos = glm::ivec2(event.motion.x, event.motion.y);
        if(is_rmb_pressed)
        {
            camera.x -= camera_scale * event.motion.xrel;
            camera.y -= camera_scale * event.motion.yrel;
        }
    }
    else if(event.type == SDL_MOUSEWHEEL)
    {
        glm::vec2 prev_pos = get_cursor_cam_pos(cursor_pos.x, cursor_pos.y, camera);
        float zoom = glm::pow(zoom_factor, static_cast<float>(-event.wheel.y));
        camera_scale *= zoom;
        camera.z *= zoom;
        camera.w *= zoom;
        glm::vec2 pos = get_cursor_cam_pos(cursor_pos.x, cursor_pos.y, camera);
        camera.x -= pos.x - prev_pos.x;
        camera.y -= pos.y - prev_pos.y;
    }

    processEvent2(event);
}

void Camera_scene::render()
{
    if(ImGui_wants_input)
        is_rmb_pressed = false;

    render2();
}

void Camera_scene::reset_camera()
{
    camera_scale = 1.f;
    camera = glm::vec4(0, 0, App::get_fb_size());
}

glm::vec2 get_cursor_cam_pos(int x, int y, const glm::vec4& camera)
{
    return glm::vec2(camera.x, camera.y) +
            glm::vec2(camera.z, camera.w) / glm::vec2(App::get_fb_size()) * glm::vec2(x, y);
}
void Camera_scene::render2()
{}

void Camera_scene::processEvent2(const SDL_Event& event)
{(void)event;}
