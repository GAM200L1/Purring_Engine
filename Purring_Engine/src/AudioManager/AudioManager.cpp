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
        : m_system(nullptr)  // Initialize FMOD system to nullptr
    {
        // Constructor.
    }

    AudioManager::~AudioManager()
    {
        if (m_system)
            m_system->release();
    }

    bool AudioManager::Init()
    {
        FMOD_RESULT result = FMOD::System_Create(&m_system);
        if (result != FMOD_OK)
        {
            std::cout << "FMOD System_Create failed: " << FMOD_ErrorString(result) << "\n";
            return false;
        }

        result = m_system->init(512, FMOD_INIT_NORMAL, nullptr);
        if (result != FMOD_OK)
        {
            std::cout << "FMOD init failed: " << FMOD_ErrorString(result) << "\n";
            return false;
        }

        return true;
    }

    void AudioManager::Update()
    {
        m_system->update();
    }

/*                                                                                                    Audio Controls
--------------------------------------------------------------------------------------------------------------------- */
    bool AudioManager::Audio::LoadSound(const std::string& path, FMOD::System* system)
    {
        FMOD_RESULT result = system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &m_sound);
        return (result == FMOD_OK);
    }

    void AudioManager::PlaySound(const std::string& id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end())
        {
            FMOD::Channel* channel = it->second->GetChannel();
            if (channel)
            {
                bool isPlaying = false;
                channel->isPlaying(&isPlaying);
                if (isPlaying)
                {
                    return;  // Exit the function as sound is already playing
                }
            }

            FMOD_RESULT result = m_system->playSound(it->second->GetSound(), nullptr, false, &channel);
            if (result == FMOD_OK)
            {
                it->second->SetChannel(channel);
            }
            else
            {
                std::cout << "Failed to play sound: " << FMOD_ErrorString(result) << "\n";
            }
        }
        else
        {
            std::cout << "Sound not found for id: " << id << "\n";
        }
    }

    void AudioManager::SetVolume(const std::string& id, float volume)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setVolume(volume);
        }
    }

    void AudioManager::SetGlobalVolume(float volume)
    {
        for (auto& pair : ResourceManager::GetInstance().Sounds)
        {
            FMOD::Channel* channel = pair.second->GetChannel();
            if (channel)
            {
                channel->setVolume(volume);
            }
        }
    }

    void AudioManager::PauseSound(const std::string& id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setPaused(true);
        }
    }

    void AudioManager::ResumeSound(const std::string& id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setPaused(false);
        }
    }

    void AudioManager::StopSound(const std::string& id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->stop();
        }
    }

    void AudioManager::StopAllSounds()
    {
        for (auto& pair : ResourceManager::GetInstance().Sounds)
        {
            FMOD::Channel* channel = pair.second->GetChannel();
            if (channel)
            {
                channel->stop();
            }
        }
    }
}
