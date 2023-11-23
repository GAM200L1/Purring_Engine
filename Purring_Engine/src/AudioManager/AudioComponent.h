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

        static void ShowErrorMessage(const std::string& r_message, const std::string& r_title = "Error");

        void PlayAudioSound(const std::string& r_id);
        void SetVolume(const std::string& r_id, float volume);
        void PauseSound(const std::string& r_id);
        void ResumeSound(const std::string& r_id);
        void StopSound(const std::string& r_id);

        // Gets the current audio file key
        std::string const& GetAudioKey() const;

        // Sets a new audio file key
        void SetAudioKey(std::string const& r_newKey);

        void SetLoop(bool loop) { m_loop = loop; }
        bool IsLooping() const { return m_loop; }

        nlohmann::json ToJson() const;
        static AudioComponent FromJson(const nlohmann::json& r_j);

    private:
        std::string m_audioKey; // Member variable to store the audio file key
        bool m_loop = false;

    };
}
