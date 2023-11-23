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

    void AudioComponent::ShowErrorMessage(const std::string& message, const std::string& title)
    {
        MessageBoxA(NULL, message.c_str(), title.c_str(), MB_ICONERROR | MB_OK);
    }

    void AudioComponent::PlayAudioSound(const std::string& id)
    {
        std::cout << "[PlayAudioSound] Attempting to play sound with id: " << id << std::endl;

        // Check the file extension
        std::string fileExtension = id.substr(id.find_last_of('.') + 1);
        if (fileExtension != "mp3")
        {
            AudioComponent::ShowErrorMessage("Error: Invalid file type. Expected '.mp3', but got '." + fileExtension + "' for id: " + id, "File Type Error");
            return;
        }

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
                    std::cout << "Sound with id: " << id << " is already playing." << std::endl;
                    return;
                }
            }

            FMOD::System* system = AudioManager::GetInstance().GetFMODSystem();
            FMOD_RESULT result = system->playSound(it->second->GetSound(), nullptr, false, &channel);

            if (result == FMOD_OK)
            {
                it->second->SetChannel(channel);
                std::cout << "Sound played successfully with id: " << id << std::endl;

                // Set the loop mode based on the m_loop flag
                FMOD_MODE loopMode = m_loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
                channel->setMode(loopMode);

                // If looping, set the loop count to -1 for infinite looping
                if (m_loop)
                {
                    channel->setLoopCount(-1);
                    std::cout << "Looping enabled for sound with id: " << id << std::endl;
                }
            }
            else
            {
                std::string errorStr = FMOD_ErrorString(result);
                ShowErrorMessage("Failed to play sound with id: " + id + ". Error: " + errorStr, "Playback Error");
            }
        }
        else
        {
            ShowErrorMessage("Sound not found in ResourceManager for id: " + id, "Resource Error");
        }
    }

    void AudioComponent::SetVolume(const std::string& id, float volume)
    {
        std::cout << "Setting volume for sound with id: " << id << " to " << volume << std::endl;

        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setVolume(volume);
            std::cout << "Volume set successfully for sound with id: " << id << std::endl;
        }
        else
        {
            std::cout << "Failed to set volume for sound with id: " << id << " (sound or channel not found)" << std::endl;
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
