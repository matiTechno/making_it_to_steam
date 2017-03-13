#ifndef GAME1_H
#define GAME1_H

#include "../scene.h"
#include "../rendering/font_loader.hpp"
#include "../sound_system.h"

class Game1: public Scene
{
public:
    Game1(const Systems& systems, bool is_opaque);

    void on_quit_event() override;

    void update(float dt) override;

    void render() override;

private:
    void process_event(SDL_Event& event) override;

    Font font;
    Texture texture;
    Music music;
    Sample sample;
    bool v_sync;
    int num_frames;
    float acc_time;
    float frame_time;
};

#endif // GAME1_H
