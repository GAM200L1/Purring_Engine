#pragma once
#include <string>
#include "AudioManager.h"
#include "Data/json.hpp"

namespace PE
{
    class AudioComponent
    {
    public:
        AudioComponent();
        ~AudioComponent();

        static void ShowErrorMessage(const std::string& message, const std::string& title = "Error");

        void PlayAudioSound(const std::string& id);
        void SetVolume(const std::string& id, float volume);
        void PauseSound(const std::string& id);
        void ResumeSound(const std::string& id);
        void StopSound(const std::string& id);

        // Gets the current audio file key
        std::string const& GetAudioKey() const;

        // Sets a new audio file key
        void SetAudioKey(std::string const& newKey);

        void SetLoop(bool loop) { m_loop = loop; }
        bool IsLooping() const { return m_loop; }

        nlohmann::json ToJson() const;
        static AudioComponent FromJson(const nlohmann::json& j);

    private:
        std::string m_audioKey; // Member variable to store the audio file key
        bool m_loop = false;

    };
}
