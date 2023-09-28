/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioManager.h
 \date     16-09-2023
 \last updated 24-09-2023

 \author               You Yang ONG
 \par      email:      youyang.o@digipen.edu

 \brief    This file contains the AudioManager class, which manages
           audio playback and includes FMOD as the underlying audio system.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include "prpch.h"
#include "Singleton.h"
#include "fmod.hpp"
#include "fmod_errors.h"
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

namespace PE
{
    class AudioManager
    {
    public:
        // Nested Audio class
        class Audio
        {
        public:
            bool LoadSound(const std::string& path, FMOD::System* system);
            FMOD::Sound* GetSound() const { return m_sound; }
            void SetChannel(FMOD::Channel* channel) { m_channel = channel; }
            FMOD::Channel* GetChannel() const { return m_channel; }

        private:
            FMOD::Sound* m_sound = nullptr;
            FMOD::Channel* m_channel = nullptr;
        };

        static AudioManager& GetInstance();                 // Singleton accessor
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;

        bool Init();
        void Update();
        void PlaySound(const std::string& id);
        void SetVolume(const std::string& id, float volume);
        void SetGlobalVolume(float volume);
        void PauseSound(const std::string& id);
        void ResumeSound(const std::string& id);
        void StopSound(const std::string& id);
        void StopAllSounds();
        FMOD::System* GetFMODSystem() { return m_system; }


    private:
        AudioManager();                                     // Private constructor for Meyer's Singleton
        ~AudioManager();

        FMOD::System* m_system;
    };
}

