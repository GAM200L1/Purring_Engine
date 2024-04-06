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
    AudioComponent::AudioComponent() : m_originalVolume(1.0f)
    {

    }

    AudioComponent::~AudioComponent()
    {

    }

    void AudioComponent::ShowErrorMessage(const std::string& r_message, const std::string& r_title)
    {
        MessageBoxA(NULL, r_message.c_str(), r_title.c_str(), MB_ICONERROR | MB_OK);
    }

    void AudioComponent::PlayAudioSound(AudioType type)
    {
        std::shared_ptr<AudioManager::Audio> audio = ResourceManager::GetInstance().GetAudio(m_audioKey);
        if (audio)
        {
            FMOD::System* p_system = AudioManager::GetInstance().GetFMODSystem();
            FMOD::Channel* p_channel = nullptr;
            FMOD::ChannelGroup* targetGroup = nullptr;

            if (type == AudioType::BGM)
            {
                targetGroup = AudioManager::GetInstance().GetBGMGroup();
            }
            else // type == AudioType::SFX
            {
                targetGroup = AudioManager::GetInstance().GetSFXGroup();
            }

            FMOD_MODE mode = FMOD_DEFAULT;
            mode |= m_loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

            FMOD_RESULT result = p_system->playSound(audio->GetSound(), targetGroup, false, &p_channel);
            if (result == FMOD_OK)
            {
                p_channel->setMode(mode); // Apply looping mode
                audio->SetChannel(p_channel);
            }
            else
            {
                // Handle playback error
            }
        }
        else
        {
            // Handle case where audio is not found
        }
    }

    bool AudioComponent::IsPlaying() const
    {
        bool isPlaying = false;
        if (auto audio = ResourceManager::GetInstance().GetAudio(m_audioKey))
        {
            if (auto p_channel = audio->GetChannel())
            {
                p_channel->isPlaying(&isPlaying);
            }
        }
        return isPlaying;
    }

    void AudioComponent::SetVolume(float volume)
    {
        //std::cout << "Setting volume for sound with id: " << m_audioKey << " to " << volume << std::endl;

        std::shared_ptr<AudioManager::Audio> audio = ResourceManager::GetInstance().GetAudio(m_audioKey);
        if (audio && audio->GetChannel())
        {
            audio->GetChannel()->setVolume(volume);
            //std::cout << "Volume set successfully for sound with id: " << m_audioKey << std::endl;
        }
        else
        {
            //std::cout << "Failed to set volume for sound with id: " << m_audioKey << " (sound or p_channel not found)" << std::endl;
        }
    }

    void AudioComponent::PauseSound()
    {
        std::shared_ptr<AudioManager::Audio> audio = ResourceManager::GetInstance().GetAudio(m_audioKey);
        if (audio && audio->GetChannel())
        {
            audio->GetChannel()->setPaused(true);
            //std::cout << "AudioComponent [" << m_audioKey << "] Paused\n";
        }
        m_isPaused = true;
    }

    void AudioComponent::ResumeSound()
    {
        std::shared_ptr<AudioManager::Audio> audio = ResourceManager::GetInstance().GetAudio(m_audioKey);
        if (audio && audio->GetChannel())
        {
            audio->GetChannel()->setPaused(false);
            //std::cout << "AudioComponent [" << m_audioKey << "] Resumed\n";
        }
        m_isPaused = false;
    }

    void AudioComponent::StopSound()
    {
        std::shared_ptr<AudioManager::Audio> audio = ResourceManager::GetInstance().GetAudio(m_audioKey);
        if (audio && audio->GetChannel())
        {
            audio->GetChannel()->stop();
        }
        // Reset pause state
        m_isPaused = false;
    }

    std::string const& AudioComponent::GetAudioKey() const
    {
        return m_audioKey;
    }

    void AudioComponent::SetAudioKey(std::string const& r_audioKey)
    {
        m_audioKey = r_audioKey;
    }

    void AudioComponent::SetLoop(bool loop)
    {
        m_loop = loop;
    }

    FMOD::Channel* AudioComponent::GetChannel() const
    {
        if (auto audio = ResourceManager::GetInstance().GetAudio(m_audioKey))
        {
            return audio->GetChannel();
        }
        return nullptr;
    }

    float AudioComponent::GetVolume() const
    {
        float volume = 0.0f;
        if (auto audio = ResourceManager::GetInstance().GetAudio(m_audioKey))
        {
            if (auto p_channel = audio->GetChannel())
            {
                p_channel->getVolume(&volume);
            }
        }
        return volume;
    }

    void AudioComponent::UpdateIndividualFade(float deltaTime)
    {
        if (!m_isFadingIndividual) return;

        m_fadeProgressIndividual += deltaTime / m_fadeDurationIndividual;
        m_fadeProgressIndividual = std::min(m_fadeProgressIndividual, 1.0f);

        float newVolume;
        if (m_isFadingInIndividual)
        {
            // For fade in, interpolate from 0 to the target volume
            newVolume = m_fadeProgressIndividual * m_targetVolumeIndividual;
        }
        else
        {
            // For fade out, interpolate from the current volume to the target volume
            newVolume = (1.0f - m_fadeProgressIndividual) * m_originalVolume + m_fadeProgressIndividual * m_targetVolumeIndividual;
        }

        SetVolume(newVolume);

        if (m_fadeProgressIndividual >= 1.0f) m_isFadingIndividual = false;
    }

    void AudioComponent::StartIndividualFadeIn(float targetVolume, float duration)
    {
        m_targetVolumeIndividual = targetVolume;
        m_originalVolume = GetVolume(); // Capture the current volume as the original volume
        m_isFadingIndividual = true;
        m_isFadingInIndividual = true;
        m_fadeDurationIndividual = duration;
        m_fadeProgressIndividual = 0.0f; // Reset fade progress
    }


    void AudioComponent::StartIndividualFadeOut(float targetVolume, float duration)
    {
        m_targetVolumeIndividual = targetVolume;
        m_isFadingIndividual = true;
        m_isFadingInIndividual = false;
        m_fadeDurationIndividual = duration;
        m_fadeProgressIndividual = 0.0f;
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
