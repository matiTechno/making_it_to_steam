#include <MITS/scene_common.hpp>
class Emitter;

class Particle_editor: public Scene
{
public:
    Particle_editor();
    ~Particle_editor();

    void render_ImGui() override;

    void render() override;

private:
    float colors[3] = {0, 0, 0};
    std::unique_ptr<Emitter> emitter;
    Texture texture;
};
