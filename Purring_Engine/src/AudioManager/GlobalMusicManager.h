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
    /*!***********************************************************************************
     \brief Holds information about the current audio state, including the track key and the
            current position in the track.
    *************************************************************************************/
    struct AudioState
    {
        std::string trackKey;
        unsigned int position;
    };

    class GlobalMusicManager : public Singleton<GlobalMusicManager>
    {
    public:
        friend class Singleton<GlobalMusicManager>;

        /*!***********************************************************************************
         \brief Contructor for GlobalMusicManager
        *************************************************************************************/
        GlobalMusicManager();
        /*!***********************************************************************************
         \brief Destructor for GlobalMusicManager
        *************************************************************************************/
        ~GlobalMusicManager();

        /*!***********************************************************************************
         \brief Regularly updates the state of the global music manager, handling any ongoing
                music transitions or fade effects.

         \param[in] deltaTime - The time elapsed since the last frame.
        *************************************************************************************/
        void Update(float deltaTime);

        /*!***********************************************************************************
         \brief Plays BGM prefab with the option to loop it.

         \param[in] prefabPath - The path to the audio prefab.
         \param[in] loop       - Whether the audio should loop.
        *************************************************************************************/
        void PlayBGM(const std::string& r_prefabPath, bool loop, float fadeInDuration = 0.0f);

        /*!***********************************************************************************
         \brief Plays SFX prefab with the option to loop it.

         \param[in] prefabPath - The path to the audio prefab.
         \param[in] loop       - Whether the audio should loop.
        *************************************************************************************/
        void PlaySFX(const std::string& r_prefabPath, bool loop);

        /*!***********************************************************************************
         \brief Pauses the currently playing background music.
        *************************************************************************************/
        void PauseBackgroundMusic();

        /*!***********************************************************************************
         \brief Resumes playing the currently paused background music.
        *************************************************************************************/
        void ResumeBackgroundMusic();

        /*!***********************************************************************************
         \brief Stops the currently playing background music.
        *************************************************************************************/
        void StopBackgroundMusic();

        /*!***********************************************************************************
         \brief Sets the volume of the background music.

         \param[in] volume - The desired volume level.
        *************************************************************************************/
        void SetBackgroundMusicVolume(float volume);

        /*!***********************************************************************************
         \brief Starts a fade-in effect for the background music over a specified duration.

         \param[in] duration - The duration of the fade-in effect in seconds.
        *************************************************************************************/
        void StartFadeIn(float duration = 1.0f);                        // Default fade duration is 1 second

        /*!***********************************************************************************
         \brief Starts a fade-out effect for the background music over a specified duration.

         \param[in] duration - The duration of the fade-out effect in seconds.
        *************************************************************************************/
        void StartFadeOut(const std::string& r_trackKey, float duration);

        /*!***********************************************************************************
         \brief Fades out all audio over a specified duration.

         \param[in] duration - Time in seconds for audio to fade to silence.
        *************************************************************************************/
        void StartFadeOutAllAudio(float duration);

        /*!***********************************************************************************
         \brief Stores the current state of the audio, including track key and position.
        *************************************************************************************/
        void StoreCurrentState();

        /*!***********************************************************************************
         \brief Resumes audio playback from a previously stored state.

         \param[in] state - The audio state to resume from.
        *************************************************************************************/
        void ResumeFromState(const AudioState& r_state);

        /*!***********************************************************************************
         \brief Returns the current audio state, including track key and position.

         \return The current audio state.
        *************************************************************************************/
        const AudioState& GetCurrentState() const;

        /*!***********************************************************************************
         \brief Stops all audio
        *************************************************************************************/
        void StopAllAudio();

    private:
        std::map<std::string, std::shared_ptr<AudioComponent>> m_audioComponents;  // Map store audio components
        std::map<std::string, float> m_originalVolumes;  // Map to store original volumes of audio components

        std::string m_currentTrackKey;
        bool m_isPaused;
        AudioState m_currentState;

        // Fade-related members
        float m_fadeDuration;
        float m_fadeProgress;
        bool m_isFading;
        bool m_isFadingIn;
        float m_maxVolume;

        SerializationManager m_serializationManager;
        float m_originalVolume = 1.0f;  // Assuming the full volume is 1.0

        /*!***********************************************************************************
         \brief Retrieves or creates a new audio component associated with a given track key.

         \param[in] trackKey - The key identifying the track.
         \return A shared pointer to the requested audio component.
        *************************************************************************************/
        std::shared_ptr<AudioComponent> GetOrCreateAudioComponent(const std::string& r_trackKey);
    };
}