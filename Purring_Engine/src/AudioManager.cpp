#include "prpch.h"
#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager()
    : system(nullptr),
    sound1(nullptr),
    sound2(nullptr),
    channel1(nullptr),
    channel2(nullptr)
{
}

AudioManager::~AudioManager()
{
    sound1->release();
    sound2->release();
    system->release();
}

bool AudioManager::Init()
{
    FMOD::System_Create(&system);
    system->init(512, FMOD_INIT_NORMAL, 0);

    system->createSound("Audio/sound1.wav", FMOD_DEFAULT, 0, &sound1);
    system->createSound("Audio/sound2.wav", FMOD_DEFAULT, 0, &sound2);

    return true;
}

void AudioManager::Update()
{
    system->update();
}

void AudioManager::PlaySound(const char* filePath)
{
    if (strcmp(filePath, "Audio/sound1.wav") == 0)
    {
        system->playSound(sound1, 0, false, &channel1);
    }
    else if (strcmp(filePath, "Audio/sound2.wav") == 0)
    {
        system->playSound(sound2, 0, false, &channel2);
    }
}

void AudioManager::StopSound()
{
    channel1->stop();
    channel2->stop();
}
