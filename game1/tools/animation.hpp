#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "../../source/common_scene.hpp"

struct Frame_data
{
    glm::ivec4 texCoords;
    float time;
    glm::vec2 origin;
};

// todo: other sprite params
// blending, sampling...
class Animation
{
public:
    Animation(const std::vector<Frame_data>& frames, Texture* texture, float scale,
              glm::vec2 pos, bool repeat);

    void update(float dt);

    Sprite get_sprite() const;

    bool is_dead();

private:
    std::vector<Frame_data> frames;
    Texture* texture;
    float scale;
    glm::vec2 pos;
    float accumulator = 0.f;
    std::size_t id = 0, prev_id;
    bool repeat;

    glm::vec2 get_origin_dist(std::size_t id);
};

#endif // ANIMATION_HPP
