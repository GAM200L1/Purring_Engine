/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GlobalMusicManager.h
 \date     2-03-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    Control global music.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include "AudioManager.h"
#include "Data/SerializationManager.h"

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

        void PlayAudioPrefab(const std::string& prefabPath, bool loop);
        void PauseBackgroundMusic();
        void ResumeBackgroundMusic();
        void StopBackgroundMusic();
        void SetBackgroundMusicVolume(float volume);

        void StartFadeIn(float duration = 1.0f);                        // Default fade duration is 1 second
        void StartFadeOut(float duration = 1.0f);
        void StartFadeToVolume(float targetVolume, float duration);

        void StoreCurrentState();
        void ResumeFromState(const AudioState& state);
        const AudioState& GetCurrentState() const { return currentState; }

        void Update(float deltaTime);

    private:
        std::map<std::string, std::shared_ptr<AudioComponent>> m_audioComponents;  // Map store audio components
        std::map<std::string, float> m_originalVolumes;  // Map to store original volumes of audio components

        std::string m_currentTrackKey;
        bool m_isPaused;
        AudioState currentState;

        // Fade-related members
        float fadeDuration;
        float fadeProgress;
        bool isFading;
        bool isFadingIn;

        SerializationManager m_serializationManager;
        float m_originalVolume = 1.0f;  // Assuming the full volume is 1.0


        std::shared_ptr<AudioComponent> GetOrCreateAudioComponent(const std::string& trackKey);
    };
}