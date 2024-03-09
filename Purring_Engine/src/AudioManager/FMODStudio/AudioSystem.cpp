/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioSystem.cpp
 \date     2-03-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    FMOD Studio Audio System setup.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include <cmath>
#include "AudioManager/FMODStudio/AudioSystem.h"
#include "Logging/Logger.h"

namespace PE
{
    namespace Audio
    {

        // UniquePtr<FmodStudioManager> AudioSystem::fmod;

        AudioSystem::AudioSystem()
        {
        }

        AudioSystem::~AudioSystem()
        {
        }

        void AudioSystem::Init()
        {
            // fmod = CreateUniquePtr<FmodStudioManager>();
        }

        void AudioSystem::Shutdown()
        {
        }

        void AudioSystem::OnUpdate(float dt)
        {
            FmodStudioManager::GetInstance().OnUpdate(dt);
        }

        void AudioSystem::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
        {
            FmodStudioManager::GetInstance().LoadBank(bankName, flags);
        }

        void AudioSystem::LoadEvent(const std::string& eventName)
        {
            FmodStudioManager::GetInstance().LoadEvent(eventName);
        }

        void AudioSystem::LoadSound(const std::string& soundName, bool is3d, bool isLooping, bool isStream)
        {
            FmodStudioManager::GetInstance().LoadSound(soundName, is3d, isLooping, isStream);
        }

        void AudioSystem::UnloadSound(const std::string& soundName)
        {
            FmodStudioManager::GetInstance().UnloadSound(soundName);
        }

        void AudioSystem::SetListenerAttributes(int listener, const glm::vec3& pos, const glm::vec3& velocity)
        {
            FmodStudioManager::GetInstance().SetListenerAttributes(listener, pos, velocity);
        }

        int AudioSystem::PlaySound(const std::string& soundName, const glm::vec3& pos, float volumedB)
        {
            return FmodStudioManager::GetInstance().PlaySound(soundName, pos, volumedB);
        }

        void AudioSystem::PlayEvent(const std::string& eventName, bool isImmediate)
        {
            FmodStudioManager::GetInstance().PlayEvent(eventName, isImmediate);
        }

        void AudioSystem::StopEvent(const std::string& eventName, bool isImmediate)
        {
            FmodStudioManager::GetInstance().StopEvent(eventName, isImmediate);
        }

        void AudioSystem::StopAllChannels()
        {
            FmodStudioManager::GetInstance().StopAllChannels();
        }

        void AudioSystem::SetChannel3dPosition(int channelId, const glm::vec3& position)
        {
            FmodStudioManager::GetInstance().SetChannel3dPosition(channelId, position);
        }

        void AudioSystem::SetChannelVolume(int channelId, float dB)
        {
            FmodStudioManager::GetInstance().SetChannelVolume(channelId, dB);
        }

        bool AudioSystem::IsPlaying(int channelId)
        {
            return FmodStudioManager::GetInstance().IsPlaying(channelId);
        }

        bool AudioSystem::IsEventPlaying(const std::string& eventName)
        {
            return FmodStudioManager::GetInstance().IsEventPlaying(eventName);
        }

    } // namespace Audio

} // namespace PE
