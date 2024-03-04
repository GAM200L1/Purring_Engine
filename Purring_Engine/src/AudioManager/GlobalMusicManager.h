#pragma once
#include "AudioManager.h"

#include "AudioComponent.h"
#include "Singleton.h"
#include <map>
#include <memory>
namespace PE
{
    struct AudioState
    {
        std::string trackKey;
        unsigned int position;
    };

    class GlobalMusicManager : public Singleton<GlobalMusicManager>
    {
    public:
        friend class Singleton<GlobalMusicManager>;

        GlobalMusicManager();
        ~GlobalMusicManager();

        void PlayBackgroundMusic(const std::string& trackKey, bool loop);
        void PauseBackgroundMusic();
        void ResumeBackgroundMusic();
        void StopBackgroundMusic();
        void SetBackgroundMusicVolume(float volume);

        void StoreCurrentState();
        void ResumeFromState(const AudioState& state);

    private:
        std::map<std::string, std::shared_ptr<AudioComponent>> m_audioComponents;  // Map store audio components
        std::string m_currentTrackKey;
        bool m_isPaused;
        AudioState currentState;

        std::shared_ptr<AudioComponent> GetOrCreateAudioComponent(const std::string& trackKey);
    };
}
