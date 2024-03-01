#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Singleton.h"

#include "AudioManager/FMODStudio/FmodStudioManager.h"

namespace PE
{
    namespace Audio
    {
        template<typename T>
        using UniquePtr = std::unique_ptr<T>;

        template<typename T, typename ... Args>
        constexpr UniquePtr<T> CreateUniquePtr(Args&& ... args) {
            return std::make_unique<T>(std::forward<Args>(args)...);
        }

        class AudioSystem : public Singleton<AudioSystem>
        {
        public:
            friend class Singleton<AudioSystem>;

            void Init();
            void Shutdown();
            void OnUpdate(float dt);

            void LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
            void LoadEvent(const std::string& eventName);
            void StopEvent(const std::string& eventName, bool isImmediate = true);
            void LoadSound(const std::string& soundName, bool is3d, bool isLooping, bool isStream);
            void UnloadSound(const std::string& soundName);

            void SetListenerAttributes(int listener, const glm::vec3& pos, const glm::vec3& velocity);

            int PlaySound(const std::string& soundName, const glm::vec3& pos, float volumedB);
            void PlayEvent(const std::string& eventName, bool isImmediate = false);

            void StopAllChannels();
            void SetChannel3dPosition(int channelId, const glm::vec3& position);
            void SetChannelVolume(int channelId, float dB);

            bool IsPlaying(int channelId);
            bool IsEventPlaying(const std::string& eventName);
            float dBToVolume(float dB);
            float VolumeTodB(float volume);

        private:
            AudioSystem();
            ~AudioSystem();

            AudioSystem(const AudioSystem&) = delete;
            AudioSystem& operator=(const AudioSystem&) = delete;
            AudioSystem(AudioSystem&&) = delete;
            AudioSystem& operator=(AudioSystem&&) = delete;

            static UniquePtr<FmodStudioManager> fmod;
        };

    } // namespace Audio

} // namespace PE
