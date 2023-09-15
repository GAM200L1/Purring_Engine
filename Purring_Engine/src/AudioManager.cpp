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
    if (sound1) sound1->release();
    if (sound2) sound2->release();
    if (system) system->release();
}

bool AudioManager::Init()
{
    FMOD_RESULT result = FMOD::System_Create(&system);
    if (result != FMOD_OK) {
        std::cout << "FMOD System_Create failed: " << FMOD_ErrorString(result) << "\n";
        return false;
    }

    result = system->init(512, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) {
        std::cout << "FMOD init failed: " << FMOD_ErrorString(result) << "\n";
        return false;
    }

    result = system->createSound("Audio/sound1.wav", FMOD_DEFAULT, 0, &sound1);
    if (result != FMOD_OK) {
        std::cout << "FMOD createSound failed for sound1: " << FMOD_ErrorString(result) << "\n";
        return false;
    }

    result = system->createSound("Audio/sound2.wav", FMOD_DEFAULT, 0, &sound2);
    if (result != FMOD_OK) {
        std::cout << "FMOD createSound failed for sound2: " << FMOD_ErrorString(result) << "\n";
        return false;
    }

    return true;
}

void AudioManager::Update()
{
    system->update();
}

void AudioManager::PlaySound(const char* filePath)
{
    FMOD::Sound* sound = nullptr;

    if (strcmp(filePath, "Audio/sound1.wav") == 0) {
        sound = sound1;
    }
    else if (strcmp(filePath, "Audio/sound2.wav") == 0) {
        sound = sound2;
    }

    if (sound) {
        FMOD_RESULT result = system->playSound(sound, 0, false, &channel1);
        if (result != FMOD_OK) {
            std::cout << "Failed to play sound: " << FMOD_ErrorString(result) << "\n";
        }
    }
    else {
        std::cout << "Sound not found for filePath: " << filePath << "\n";
    }
}

void AudioManager::StopSound()
{
    if (channel1) channel1->stop();
    if (channel2) channel2->stop();
}
