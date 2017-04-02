#ifndef CAMERA_SCENE_HPP
#define CAMERA_SCENE_HPP

#include <MITS/scene_common.hpp>

class Camera_scene: public Scene
{
public:
    Camera_scene();

    void processEvent(const SDL_Event& event) final;

    void update_coords() final;

    void render() final;

private:
    virtual void processEvent2(const SDL_Event& event);
    virtual void update_coords2();
    virtual void render2();

    glm::vec4 camera;
    float camera_scale = 1.f;
    const float zoom_factor = 1.2f;
    bool is_rmb_pressed = false;
    glm::ivec2 cursor_pos;

protected:
    void reset_camera();
    glm::vec4 get_camera(){return camera;}
};

glm::vec2 get_cursor_cam_pos(int x, int y, const glm::vec4& camera);

#endif // CAMERA_SCENE_HPP
