#include <MITS/sound_system.hpp>
#include <iostream>
#include <stdexcept>
#include <assert.h>
#include <SDL2/SDL_mixer.h>

Music::Music(const std::string& filename):
    Res_class([](Mix_Music* id){if(id)Mix_FreeMusic(id);})
{
id = Mix_LoadMUS(filename.c_str());
if(!id)
throw std::runtime_error("Mix_LoadMUS failed: " + filename + ": " + Mix_GetError());
}

bool Music::isActive()
{
    return Mix_PlayingMusic();
}

Sample::Sample(const std::string& filename, int volume):
    Res_class([](Mix_Chunk* id){
    if(id){
        Sound_system::stop_samples();
        Mix_FreeChunk(id);}
})
{
id = Mix_LoadWAV(filename.c_str());
if(!id)
throw std::runtime_error("Mix_LoadWAV failed: " + filename + ": " + Mix_GetError());
Mix_VolumeChunk(id, volume);
}

int Sample::get_volume() const
{
    return Mix_VolumeChunk(id, -1);
}

void Sample::set_volume(int volume)
{
    Mix_VolumeChunk(id, volume);
}

bool Sound_system::isCurrent = false;

Sound_system::Sound_system(int init_flags)
{
    assert(!isCurrent);
    isCurrent = true;

    SDL_version ver_compiled;
    SDL_MIXER_VERSION(&ver_compiled);
    const SDL_version* ver_linked = Mix_Linked_Version();

    std::cout << "compiled against SDL_mixer " << int(ver_compiled.major) << '.' <<
                 int(ver_compiled.minor) << '.' << int(ver_compiled.patch) << std::endl;
    std::cout << "linked against SDL_mixer " << int(ver_linked->major) << '.' <<
                 int(ver_linked->minor) << '.' << int(ver_linked->patch) << std::endl;

    if(init_flags)
    {
        int initted = Mix_Init(init_flags);
        if(init_flags != initted)
            throw std::runtime_error("Mix_Init failed (init_flags = " +
                                     std::to_string(init_flags) + "): " + std::string(Mix_GetError()));
    }

    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) != 0)
        throw std::runtime_error("Mix_OpenAudio failed: " + std::string(Mix_GetError()));
}

Sound_system::~Sound_system()
{
    Mix_CloseAudio();
}

void Sound_system::play_sample(const Sample& sample, int volume) const
{
    int channel = Mix_PlayChannel(-1, sample.id, 0);
    if(channel == -1)
    {
        Mix_AllocateChannels(Mix_AllocateChannels(-1) + 1);
        channel = Mix_PlayChannel(-1, sample.id, 0);
        if(channel == -1)
        {
            std::cout << "Mix_PlayChannel error: " + std::string(Mix_GetError()) << std::endl;
            return;
        }
    }

    Mix_Volume(channel, volume);
}

void Sound_system::pause_samples() const
{
    Mix_Pause(-1);
}

void Sound_system::resume_samples() const
{
    Mix_Resume(-1);
}

void Sound_system::stop_samples()
{
    assert(isCurrent);
    Mix_HaltChannel(-1);
}

void Sound_system::play_music(const Music& music, bool loop, int volume) const
{
    int r_code;
    if(loop)
        r_code = Mix_PlayMusic(music.id, -1);
    else
        r_code = Mix_PlayMusic(music.id, 1);

    if(r_code == -1)
    {
        std::cout << "Mix_PlayMusic error: " + std::string(Mix_GetError()) << std::endl;
        return;
    }

    Mix_VolumeMusic(volume);
}

void Sound_system::pause_music() const
{
    if(Mix_PlayingMusic())
        Mix_PauseMusic();
}

void Sound_system::resume_music() const
{
    Mix_ResumeMusic();
}

void Sound_system::stop_music() const
{
    Mix_HaltMusic();
}
