#pragma once

#include "AudioManager/FMODStudio/AudioClip.h"

namespace PE {
    namespace Audio {

        class AudioSource {
        public:
            AudioSource();
            ~AudioSource();

            void Play();
            void Pause();
            void Unpause();
            void PlayDelayed(float delay);
            // void PlayOneShot(const AudioClip& clip, float volumeScale = 1.0f);
            void Stop();

            void SetClip(const AudioClip& clip);
            void SetVolume(float volume);
            void SetPitch(float pitch);
            void SetLoop(bool loop);

        private:
            AudioClip clip;
            bool isPlaying = false;
            bool mute = false;
            bool playOnAwake = false;
            bool loop = false;

            uint32_t priority = 128;
            float volume = 1.0f;
            float pitch = 1.0f;
            float stereoPan = 0.0f;

            // Store the channel ID or sound instance from FmodStudioManager for control
            int currentChannelId = -1;
        };

    } // namespace Audio

} // namespace PE
