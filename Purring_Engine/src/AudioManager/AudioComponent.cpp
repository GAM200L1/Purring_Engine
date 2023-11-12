#include "prpch.h"
#include "AudioComponent.h"
#include "ResourceManager/ResourceManager.h"

namespace PE
{
    AudioComponent::AudioComponent()
    {

    }

    AudioComponent::~AudioComponent()
    {

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
                    return;
                }
            }

            FMOD::System* system = AudioManager::GetInstance().GetFMODSystem();
            FMOD_RESULT result = system->playSound(it->second->GetSound(), nullptr, false, &channel);

            if (result == FMOD_OK)
            {
                it->second->SetChannel(channel);

                // Set the loop mode based on the m_loop flag
                FMOD_MODE loopMode = m_loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
                channel->setMode(loopMode);

                // If looping, set the loop count to -1 for infinite looping
                if (m_loop)
                {
                    channel->setLoopCount(-1);
                }
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

    AudioComponent AudioComponent::FromJson(const nlohmann::json& j)
    {
        AudioComponent audioComponent;
        if (j.contains("audioKey"))
        {
            audioComponent.SetAudioKey(j["audioKey"].get<std::string>());
        }
        if (j.contains("loop"))
        {
            audioComponent.SetLoop(j["loop"].get<bool>());
        }
        return audioComponent;
    }

}
