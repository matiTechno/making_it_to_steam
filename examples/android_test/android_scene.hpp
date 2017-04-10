#include <MITS/scene_common.hpp>

class Android_scene: public Scene
{
public:
   void render() override
   {
       renderer.load_projection(glm::vec4(0, 0, coords.size));
       Sprite sprite;
       sprite.position = glm::vec2(0, 0);
       sprite.size = glm::vec2(100.f, 100.f);
       sprite.color = glm::vec4(0, 1.f, 1.f, 1.f);
       renderer.render(sprite);
   }
};
