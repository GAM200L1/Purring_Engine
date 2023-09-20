/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioManager.h
 \date     16-09-2023

 \author               You Yang ONG
 \par      email:      youyang.o@digipen.edu

 \brief    This file contains the AudioManager class, which manages
           audio playback and includes FMOD as the underlying audio system.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include "fmod.hpp"
#include "fmod_errors.h"
class AudioManager
{
public:
    // Constructor - Destructor
    AudioManager();
    ~AudioManager();

    bool Init();
    void Update();
    void PlaySound(const char* filePath);
    void StopSound();

private:
    // FMOD system object for managing audio resources
    FMOD::System* system;

    // FMOD sound objects to hold audio data
    FMOD::Sound* sound1;
    FMOD::Sound* sound2;

    // FMOD channel objects to control playback
    FMOD::Channel* channel1;
    FMOD::Channel* channel2;
};
