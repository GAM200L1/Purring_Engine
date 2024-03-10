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

        void AudioSystem::LoadBank(const std::string& r_bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
        {
            FmodStudioManager::GetInstance().LoadBank(r_bankName, flags);
        }

        void AudioSystem::LoadEvent(const std::string& r_eventName)
        {
            FmodStudioManager::GetInstance().LoadEvent(r_eventName);
        }

        void AudioSystem::LoadSound(const std::string& r_soundName, bool is3d, bool isLooping, bool isStream)
        {
            FmodStudioManager::GetInstance().LoadSound(r_soundName, is3d, isLooping, isStream);
        }

        void AudioSystem::UnloadSound(const std::string& r_soundName)
        {
            FmodStudioManager::GetInstance().UnloadSound(r_soundName);
        }

        void AudioSystem::SetListenerAttributes(int listener, const glm::vec3& r_pos, const glm::vec3& r_velocity)
        {
            FmodStudioManager::GetInstance().SetListenerAttributes(listener, r_pos, r_velocity);
        }

        int AudioSystem::PlaySound(const std::string& r_soundName, const glm::vec3& r_pos, float volumedB)
        {
            return FmodStudioManager::GetInstance().PlaySound(r_soundName, r_pos, volumedB);
        }

        void AudioSystem::PlayEvent(const std::string& r_eventName, bool isImmediate)
        {
            FmodStudioManager::GetInstance().PlayEvent(r_eventName, isImmediate);
        }

        void AudioSystem::StopEvent(const std::string& r_eventName, bool isImmediate)
        {
            FmodStudioManager::GetInstance().StopEvent(r_eventName, isImmediate);
        }

        void AudioSystem::StopAllChannels()
        {
            FmodStudioManager::GetInstance().StopAllChannels();
        }

        void AudioSystem::SetChannel3dPosition(int channelId, const glm::vec3& r_position)
        {
            FmodStudioManager::GetInstance().SetChannel3dPosition(channelId, r_position);
        }

        void AudioSystem::SetChannelVolume(int channelId, float dB)
        {
            FmodStudioManager::GetInstance().SetChannelVolume(channelId, dB);
        }

        bool AudioSystem::IsPlaying(int channelId)
        {
            return FmodStudioManager::GetInstance().IsPlaying(channelId);
        }

        bool AudioSystem::IsEventPlaying(const std::string& r_eventName)
        {
            return FmodStudioManager::GetInstance().IsEventPlaying(r_eventName);
        }

    } // namespace Audio

} // namespace PE
