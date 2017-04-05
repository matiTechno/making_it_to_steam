#include "camera_scene.hpp"
#include "animation.hpp"

class Set_coll_data: public Camera_scene
{
public:
    Set_coll_data(Frame& frame, Texture& texture, const std::vector<const char*>& coll_group_names);

    void render_ImGui() override;

private:
    Frame& frame;
    Texture& texture;
    glm::vec4 saved_coords;
    int current_name = 0;
    const std::vector<const char*>& coll_group_names;
    std::list<Anim_rect>* active_coll_group;

    void render2() override;
    void processEvent2(const SDL_Event& event) override;
};
