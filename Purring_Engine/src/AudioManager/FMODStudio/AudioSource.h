/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioSource.h
 \date     2-03-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    FMOD Studio Audio Source setup.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include "AudioManager/FMODStudio/AudioClip.h"

namespace PE
{
    namespace Audio
    {
        class AudioSource
        {
        public:
            AudioSource();
            ~AudioSource();

            /*!***********************************************************************************
             \brief Plays the audio clip associated with this audio source.
            *************************************************************************************/
            void Play();

            /*!***********************************************************************************
             \brief Pauses the currently playing audio clip.
            *************************************************************************************/
            void Pause();

            /*!***********************************************************************************
             \brief Unpauses the currently paused audio clip, resuming playback.
            *************************************************************************************/
            void Unpause();

            /*!***********************************************************************************
             \brief Plays the audio clip after a specified delay.

             \param[in] delay - The delay in seconds before the audio clip plays.
            *************************************************************************************/
            void PlayDelayed(float delay);

            /*!***********************************************************************************
             \brief Stops the currently playing audio clip.
            *************************************************************************************/
            void Stop();

            /*!***********************************************************************************
             \brief Sets the audio clip to be played by this audio source.

             \param[in] clip - The audio clip to play.
            *************************************************************************************/
            void SetClip(const AudioClip& r_clip);

            /*!***********************************************************************************
             \brief Sets the volume of the audio source.

             \param[in] volume - The volume level, where 1.0 is the maximum.
            *************************************************************************************/
            void SetVolume(float volume);

            /*!***********************************************************************************
             \brief Sets the pitch of the audio source.

             \param[in] pitch - The pitch level, where 1.0 is the normal pitch.
            *************************************************************************************/
            void SetPitch(float pitch);

            /*!***********************************************************************************
             \brief Sets whether the audio source should loop the audio clip.

             \param[in] loop - True to loop the clip, false to play it once.
            *************************************************************************************/
            void SetLoop(bool loop);

        private:
            AudioClip m_clip;

            bool m_isPlaying = false;
            bool m_mute = false;
            bool m_playOnAwake = false;
            bool m_loop = false;

            uint32_t m_priority = 128;
            float m_volume = 1.0f;
            float m_pitch = 1.0f;
            float m_stereoPan = 0.0f;

            int m_currentChannelId = -1;
        };

    } // namespace Audio

} // namespace PE
