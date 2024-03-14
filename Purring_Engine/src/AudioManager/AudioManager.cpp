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
        : p_system(nullptr)  // Initialize FMOD system to nullptr
    {}

    AudioManager::~AudioManager()
    {
        if (p_system)
            p_system->release();
    }

    bool AudioManager::Init()
    {
        FMOD_RESULT result = FMOD::System_Create(&p_system);
        if (result != FMOD_OK)
        {
            //std::cout << "FMOD Systep_Create failed: " << FMOD_ErrorString(result) << "\n";
            return false;
        }

        result = p_system->init(512, FMOD_INIT_NORMAL, nullptr);
        if (result != FMOD_OK)
        {
            //std::cout << "FMOD init failed: " << FMOD_ErrorString(result) << "\n";
            return false;
        }

        // Master Channel Group
        result = p_system->createChannelGroup("MasterGroup", &p_masterGroup);
        if (result != FMOD_OK)
        {
            return false;
        }

        // BGM Channel Group
        result = p_system->createChannelGroup("BGMGroup", &p_bgmGroup);
        if (result != FMOD_OK)
        {
            return false;
        }
        p_masterGroup->addGroup(p_bgmGroup); // Add BGM group to Master group

        // SFX Channel Group
        result = p_system->createChannelGroup("SFXGroup", &p_sfxGroup);
        if (result != FMOD_OK)
        {
            return false;
        }
        p_masterGroup->addGroup(p_sfxGroup); // Add SFX group to Master group

        return true;
    }

    void AudioManager::Update()
    {
        p_system->update();
    }

/*                                                                                                    Audio Controls
--------------------------------------------------------------------------------------------------------------------- */
    bool AudioManager::Audio::LoadSound(const std::string& r_path, FMOD::System* p_system)
    {
        FMOD_RESULT result = p_system->createSound(r_path.c_str(), FMOD_DEFAULT, nullptr, &p_sound);
        return (result == FMOD_OK);
    }

    void AudioManager::SetMasterVolume(float volume)
    {
        p_masterGroup->setVolume(volume);
    }

    void AudioManager::SetBGMVolume(float volume)
    {
        p_bgmGroup->setVolume(volume);
    }

    void AudioManager::SetSFXVolume(float volume)
    {
        p_sfxGroup->setVolume(volume);
    }

    float AudioManager::GetBGMVolume() const
    {
        float volume;
        FMOD_RESULT result = p_bgmGroup->getVolume(&volume);
        if (result == FMOD_OK)
        {
            return volume;
        }
        else
        {
            // Handle error.
            return 0.0f; // 0.0 cause safe default.
        }
    }

    float AudioManager::GetSFXVolume() const
    {
        float volume;
        FMOD_RESULT result = p_sfxGroup->getVolume(&volume);
        if (result == FMOD_OK)
        {
            return volume;
        }
        else
        {
            // Handle error.
            return 0.0f; // 0.0 cause safe default.
        }
    }

    float AudioManager::GetBGMVolume() const
    {
        float volume;
        FMOD_RESULT result = m_bgmGroup->getVolume(&volume);
        if (result == FMOD_OK)
        {
            return volume;
        }
        else
        {
            // Handle error.
            return 0.0f; // 0.0 cause safe default.
        }
    }

    float AudioManager::GetSFXVolume() const
    {
        float volume;
        FMOD_RESULT result = m_sfxGroup->getVolume(&volume);
        if (result == FMOD_OK)
        {
            return volume;
        }
        else
        {
            // Handle error.
            return 0.0f; // 0.0 cause safe default.
        }
    }

    void AudioManager::MuteMaster(bool mute)
    {
        p_masterGroup->setMute(mute);
    }

    void AudioManager::MuteBGM(bool mute)
    {
        p_bgmGroup->setMute(mute);
    }

    void AudioManager::MuteSFX(bool mute)
    {
        p_sfxGroup->setMute(mute);
    }

    FMOD::ChannelGroup* AudioManager::GetBGMGroup() const
    {
        return p_bgmGroup;
    }

    FMOD::ChannelGroup* AudioManager::GetSFXGroup() const
    {
        return p_sfxGroup;
    }

    float AudioManager::GetMasterVolume() const
    {
        float volume;
        FMOD_RESULT result = p_masterGroup->getVolume(&volume);
        if (result == FMOD_OK)
        {
            return volume;
        }
        else
        {
            // Handle error.
            return 0.0f; // 0.0 cause safe default.
        }
    }

    bool AudioManager::GetMasterMute() const
    {
        bool mute;
        FMOD_RESULT result = p_masterGroup->getMute(&mute);
        if (result == FMOD_OK)
        {
            return mute;
        }
        else
        {
            // Handle error.
            return false;
        }
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

}
