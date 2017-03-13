#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H

#include "res_class.h"
#include <string>
struct Mix_Chunk;
typedef struct _Mix_Music Mix_Music;

// volume range [0, 128]
// freeing any sample will halt all channels
// in Sound_system
// it seems weird but it simplifies things
class Sample: public Res_class<Mix_Chunk*>
{
public:
    friend class Sound_system;

    Sample(const std::string& filename);
};

class Music: public Res_class<Mix_Music*>
{
public:
    friend class Sound_system;

    Music(const std::string& filename);

    // returns false if halted
    // returns true even if paused
    bool isActive();
};

// Sample and Music classes can be used
// only between Sound_system creation and destruction
class Sound_system
{
public:
    // e.g.
    // MIX MIX_INIT_MP3 | MIX_INIT_OGG
    Sound_system(int init_flags = 0);
    ~Sound_system();
    Sound_system(const Sound_system&) = delete;

    // samples (sound effects)
    void play_sample(const Sample& sample, int volume = 128) const;
    void pause_samples() const;
    void resume_samples() const;
    static void stop_samples();

    // music
    // only one at a time
    void play_music(const Music& music, bool loop, int volume = 128) const;
    void pause_music() const;
    void resume_music() const;
    void stop_music() const;

private:
    static bool isCurrent;
};

#endif // SOUND_SYSTEM_H
