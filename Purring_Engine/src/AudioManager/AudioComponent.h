#pragma once
#include <string>
#include "AudioManager.h"

namespace PE
{
    class AudioComponent
    {
    public:
        AudioComponent();
        ~AudioComponent();

        void PlayAudioSound(const std::string& id);
        void SetVolume(const std::string& id, float volume);
        void PauseSound(const std::string& id);
        void ResumeSound(const std::string& id);
        void StopSound(const std::string& id);

    private:

    };
}
