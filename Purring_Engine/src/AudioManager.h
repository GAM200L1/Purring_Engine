#pragma once
#include "fmod.hpp"
#include "fmod_errors.h"
class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    bool Init();
    void Update();
    void PlaySound(const char* filePath);
    void StopSound();

private:
    FMOD::System* system;
    FMOD::Sound* sound1;
    FMOD::Sound* sound2;
    FMOD::Channel* channel1;
    FMOD::Channel* channel2;
};
