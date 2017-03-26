#include "animation.hpp"

Animation::Animation(const std::vector<Frame_data>& frames, Texture* texture, float scale,
                     glm::vec2 pos, bool repeat):
    frames(frames),
    texture(texture),
    scale(scale),
    prev_id(frames.size() - 1),
    repeat(repeat)
{
    assert(frames.size());
    this->pos = pos - get_origin_dist(0);
}

void Animation::update(float dt)
{
    accumulator += dt;
    while(accumulator >= frames[id].time)
    {
        if(is_dead())
            return;

        accumulator -= frames[id].time;
        ++id;
        ++prev_id;
        if(id > frames.size() - 1)
            id = 0;
        if(prev_id > frames.size() - 1)
            prev_id = 0;

        pos += get_origin_dist(prev_id) - get_origin_dist(id);
    }
}

Sprite Animation::get_sprite() const
{
    Sprite sprite;
    sprite.position = pos;
    sprite.size = scale * glm::vec2(frames[id].texCoords.z,
                                    frames[id].texCoords.w);
    sprite.texCoords = frames[id].texCoords;
    sprite.texture = texture;
    return sprite;
}

bool Animation::is_dead()
{
    if(!repeat && id == frames.size() - 1)
        return true;
    return false;
}

glm::vec2 Animation::get_origin_dist(std::size_t id)
{
    return glm::vec2(frames[id].origin * glm::vec2(frames[id].texCoords.z, frames[id].texCoords.w) * scale);
}
