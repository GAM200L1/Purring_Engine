#include "prpch.h"
#include "AudioComponent.h"
#include "ResourceManager/ResourceManager.h"

namespace PE
{
    AudioComponent::AudioComponent()
    {
        // Constructor implementation
    }

    AudioComponent::~AudioComponent()
    {
        // Destructor implementation
    }

    void AudioComponent::PlayAudioSound(const std::string& id)
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

            FMOD::System* system = AudioManager::GetInstance().GetFMODSystem();  // Get the FMOD system from the AudioManager
            FMOD_RESULT result = system->playSound(it->second->GetSound(), nullptr, false, &channel);
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

    void AudioComponent::SetVolume(const std::string& id, float volume)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setVolume(volume);
        }
    }

    void AudioComponent::PauseSound(const std::string& id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setPaused(true);
        }
    }

    void AudioComponent::ResumeSound(const std::string& id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setPaused(false);
        }
    }

    void AudioComponent::StopSound(const std::string& id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->stop();
        }
    }

}
