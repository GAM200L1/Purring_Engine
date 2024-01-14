/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioManager.cpp
 \date     10-09-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief	   This file features the AudioManager class that encapsulates the FMOD library API
           for audio operations. It initializes the FMOD system and controls its lifecycle,
           provides a series of methods for sound playback, and interfaces seamlessly with
           the ResourceManager to manage audio assets.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "AudioManager.h"
#include "ResourceManager/ResourceManager.h"
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

namespace PE
{
    AudioManager::AudioManager()
        : m_system(nullptr)
    {}

    AudioManager::~AudioManager()
    {
        if (m_system)
        {
            m_system->release();
        }
    }

    bool AudioManager::Init()
    {
        FMOD_RESULT result;

        // Create the Studio system
        result = FMOD::Studio::System::create(&m_system);
        if (result != FMOD_OK)
        {
            std::cout << "FMOD Studio System_Create failed: " << FMOD_ErrorString(result) << "\n";
            return false;
        }

        // Initialize the system
        result = m_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
        if (result != FMOD_OK)
        {
            std::cout << "FMOD Studio init failed: " << FMOD_ErrorString(result) << "\n";
            return false;
        }

        return true;
    }


    void AudioManager::Update()
    {
        if (m_system)
        {
            m_system->update();
        }
    }



    /*                                                                                                    Audio Controls
    --------------------------------------------------------------------------------------------------------------------- */
    bool AudioManager::Audio::LoadSound(const std::string& r_path, FMOD::System* p_system)
    {
        FMOD_RESULT result = p_system->createSound(r_path.c_str(), FMOD_DEFAULT, nullptr, &m_sound);
        return (result == FMOD_OK);
    }

    void AudioManager::SetGlobalVolume(float volume)
    {
        for (auto& pair : ResourceManager::GetInstance().Sounds)
        {
            FMOD::Channel* p_channel = pair.second->GetChannel();
            if (p_channel)
            {
                p_channel->setVolume(volume);
            }
        }
    }

    void AudioManager::StopAllSounds()
    {
        for (auto& pair : ResourceManager::GetInstance().Sounds)
        {
            FMOD::Channel* p_channel = pair.second->GetChannel();
            if (p_channel)
            {
                p_channel->stop();
            }
        }
    }

    FMOD::System* AudioManager::GetFMODSystem()
    {
        FMOD::System* lowLevelSystem = nullptr;
        if (m_system) {
            m_system->getCoreSystem(&lowLevelSystem);
        }
        return lowLevelSystem;
    }



}
