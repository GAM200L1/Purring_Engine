/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FmodStudioManager.h
 \date     2-03-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    FMOD Studio FmodStudioManager setup.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <map>
#include <string>
#include <glm/glm.hpp>

#include "Singleton.h"

namespace PE
{
    namespace Audio
    {

        class FmodStudioManager : public Singleton<FmodStudioManager>
        {
            friend class Singleton<FmodStudioManager>;
        public:
            static FmodStudioManager& GetInstance()
            {
                static FmodStudioManager instance;
                return instance;
            }

            void OnUpdate(float dt);
            FMOD::Studio::Bank* GetBank(const std::string& bankName) const;
            void LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);

            void LoadEvent(const std::string& eventName);
            void LoadSound(const std::string& soundName, bool is3d, bool isLooping, bool isStream);
            void UnloadSound(const std::string& soundName);

            void SetListenerAttributes(int listener, const glm::vec3& pos, const glm::vec3& velocity);
            int PlaySound(const std::string& soundName, const glm::vec3& pos, float volumedB);
            void PlayEvent(const std::string& eventName, bool isImmediate);
            void StopEvent(const std::string& eventName, bool isImmediate);
            void StopAllChannels();
            void SetChannel3dPosition(int channelId, const glm::vec3& position);
            void SetChannelVolume(int channelId, float dB);

            bool IsPlaying(int channelId);
            bool IsEventPlaying(const std::string& eventName);
            float dBToVolume(float dB);
            float VolumeTodB(float volume);


        protected:
            FmodStudioManager();
            ~FmodStudioManager();

        private:

            FMOD::System* coreSystem{ nullptr };
            FMOD::Studio::System* studioSystem{ nullptr };

            int nextChannelId{ 0 };

            using SoundMap = std::map<std::string, FMOD::Sound*>;
            using ChannelMap = std::map<int, FMOD::Channel*>;
            using EventMap = std::map<std::string, FMOD::Studio::EventInstance*>;
            using BankMap = std::map<std::string, FMOD::Studio::Bank*>;

            SoundMap sounds;
            ChannelMap channels;
            EventMap events;
            BankMap banks;
        };

    } // namespace Audio

} // namespace PE
