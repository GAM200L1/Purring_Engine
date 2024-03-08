/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioSource.cpp
 \date     2-03-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    FMOD Studio Audio Source setup.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"

#include "AudioManager/FMODStudio/AudioSource.h"
#include "AudioManager/FMODStudio/AudioSystem.h"

#include <imgui.h>

namespace PE {
    namespace Audio {

        AudioSource::AudioSource()
        {
            // Initialize AudioSource
            if (playOnAwake)
            {
                Play();
            }
        }

        AudioSource::~AudioSource()
        {
            Stop();
        }

        //void AudioSource::Play() {
        //    if (!mute && !clip.clipPath.empty()) {
        //        Stop();  // Stop any currently playing sound
        //        currentChannelId = AudioSystem::PlaySound(clip.clipPath, { 0.0f, 0.0f, 0.0f }, volume);  // Adjust position as needed
        //        isPlaying = true;
        //    }
        //}

        void AudioSource::Pause()
        {
        }

        void AudioSource::Unpause()
        {
        }

        //void AudioSource::PlayDelayed(float delay)
        //{
        //}

        void AudioSource::Stop()
        {
            //if (isPlaying) {
            //    AudioSystem::StopChannel(currentChannelId);
            //    isPlaying = false;
            //}
        }

        void AudioSource::SetClip(const AudioClip& newClip)
        {
            clip = newClip;
        }

        //void AudioSource::SetVolume(float newVolume)
        //{
        //    //volume = newVolume;
        //    //if (isPlaying) {
        //    //    AudioSystem::SetChannelVolume(currentChannelId, volume);
        //    //}
        //}

        void AudioSource::SetPitch(float newPitch)
        {
            pitch = newPitch;
        }

        void AudioSource::SetLoop(bool shouldLoop)
        {
            loop = shouldLoop;
        }

    } // namespace Audio

} // namespace PE
