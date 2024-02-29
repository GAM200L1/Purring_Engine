#pragma once

#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <map>
#include <string>

namespace PE
{
    namespace Audio
    {

        class FmodStudioManager
        {
        public:
            FmodStudioManager();
            ~FmodStudioManager();

            void OnUpdate(float dt);

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
