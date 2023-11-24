/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioComponent.cpp
 \date     2-10-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    This file provides the implementation details for the AudioComponent class 
           of the Purring Engine. It includes functionalities for managing audio playback, 
           such as playing, pausing, resuming, and stopping audio tracks, as well as 
           adjusting volume and setting looping preferences. The file also handles the 
           serialization and deserialization of audio component states using JSON format. 
           It ensures audio resources are effectively managed and integrates with the 
           ResourceManager and AudioManager for efficient audio handling in the engine.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "AudioComponent.h"
#include "ResourceManager/ResourceManager.h"
#include <Windows.h>

namespace PE
{
    AudioComponent::AudioComponent()
    {

    }

    AudioComponent::~AudioComponent()
    {

    }

    void AudioComponent::ShowErrorMessage(const std::string& r_message, const std::string& r_title)
    {
        MessageBoxA(NULL, r_message.c_str(), r_title.c_str(), MB_ICONERROR | MB_OK);
    }

    void AudioComponent::PlayAudioSound(const std::string& r_id)
    {
        std::cout << "[PlayAudioSound] Attempting to play sound with id: " << r_id << std::endl;

        // Check the file extension
        std::string fileExtension = r_id.substr(r_id.find_last_of('.') + 1);
        if (fileExtension != "mp3")
        {
            AudioComponent::ShowErrorMessage("Error: Invalid file type. Expected '.mp3', but got '." + fileExtension + "' for id: " + r_id, "File Type Error");
            return;
        }

        auto it = ResourceManager::GetInstance().Sounds.find(r_id);
        if (it != ResourceManager::GetInstance().Sounds.end())
        {
            FMOD::Channel* channel = it->second->GetChannel();
            if (channel)
            {
                bool isPlaying = false;
                channel->isPlaying(&isPlaying);
                if (isPlaying)
                {
                    std::cout << "Sound with id: " << r_id << " is already playing." << std::endl;
                    return;
                }
            }

            FMOD::System* system = AudioManager::GetInstance().GetFMODSystem();
            FMOD_RESULT result = system->playSound(it->second->GetSound(), nullptr, false, &channel);

            if (result == FMOD_OK)
            {
                it->second->SetChannel(channel);
                std::cout << "Sound played successfully with id: " << r_id << std::endl;

                // Set the loop mode based on the m_loop flag
                FMOD_MODE loopMode = m_loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
                channel->setMode(loopMode);

                // If looping, set the loop count to -1 for infinite looping
                if (m_loop)
                {
                    channel->setLoopCount(-1);
                    std::cout << "Looping enabled for sound with id: " << r_id << std::endl;
                }
            }
            else
            {
                std::string errorStr = FMOD_ErrorString(result);
                ShowErrorMessage("Failed to play sound with id: " + r_id + ". Error: " + errorStr, "Playback Error");
            }
        }
        else
        {
            ShowErrorMessage("Sound not found in ResourceManager for id: " + r_id, "Resource Error");
        }
    }

    void AudioComponent::SetVolume(const std::string& r_id, float volume)
    {
        std::cout << "Setting volume for sound with id: " << r_id << " to " << volume << std::endl;

        auto it = ResourceManager::GetInstance().Sounds.find(r_id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setVolume(volume);
            std::cout << "Volume set successfully for sound with id: " << r_id << std::endl;
        }
        else
        {
            std::cout << "Failed to set volume for sound with id: " << r_id << " (sound or channel not found)" << std::endl;
        }
    }

    void AudioComponent::PauseSound(const std::string& r_id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(r_id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setPaused(true);
        }
    }

    void AudioComponent::ResumeSound(const std::string& r_id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(r_id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setPaused(false);
        }
    }

    void AudioComponent::StopSound(const std::string& r_id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(r_id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->stop();
        }
    }

    std::string const& AudioComponent::GetAudioKey() const
    {
        return m_audioKey;
    }

    void AudioComponent::SetAudioKey(std::string const& newKey)
    {
        m_audioKey = newKey;
    }

    nlohmann::json AudioComponent::ToJson() const
    {
        nlohmann::json j;
        j["audioKey"] = m_audioKey;
        j["loop"] = m_loop;
        return j;
    }

    AudioComponent AudioComponent::FromJson(const nlohmann::json& r_j)
    {
        AudioComponent audioComponent;
        if (r_j.contains("audioKey"))
        {
            audioComponent.SetAudioKey(r_j["audioKey"].get<std::string>());
        }
        if (r_j.contains("loop"))
        {
            audioComponent.SetLoop(r_j["loop"].get<bool>());
        }
        return audioComponent;
    }

}
