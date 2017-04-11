#include <MITS/scene_common.hpp>

class Particle_editor: public Scene
{
public:
    void render_ImGui() override;

private:
    float colors[3] = {0, 0, 0};
};
