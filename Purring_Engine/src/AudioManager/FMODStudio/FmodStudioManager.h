#pragma once

#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <map>
#include <string>
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
