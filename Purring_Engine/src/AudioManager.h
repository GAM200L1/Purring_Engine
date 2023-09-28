/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioManager.h
 \date     10-09-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu
 \par      code %:     <remove if sole author>
 \par      changes:    <remove if sole author>

 \brief	   This file features the AudioManager class that encapsulates the FMOD library API
           for audio operations. It initializes the FMOD system and controls its lifecycle,
           provides a series of methods for sound playback, and interfaces seamlessly with
           the ResourceManager to manage audio assets.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#pragma warning(disable : 4505) // warning C4505: ('FMOD_ErrorString': unreferenced function with internal linkage has been removed)

#include "prpch.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "Singleton.h"
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

namespace PE
{
    /*!***********************************************************************************
     \brief     AudioManager class for handling FMOD audio system and events.
    *************************************************************************************/
    class AudioManager : public Singleton<AudioManager>
    {
    public:
        friend class Singleton<AudioManager>;

        /*!***********************************************************************************
         \brief     Nested Audio class for handling individual audio data.
        *************************************************************************************/
        class Audio
        {
        public:
            /*!***********************************************************************************
             \brief     Load a sound from a file path.
             \param     path Path to the audio file.
             \param     system FMOD::System pointer for audio system management.
             \return    bool True if the sound is successfully loaded.
            *************************************************************************************/
            bool LoadSound(const std::string& path, FMOD::System* system);

            /*!***********************************************************************************
             \brief     Getter for FMOD::Sound* object.
             \return    FMOD::Sound* The sound object.
            *************************************************************************************/
            FMOD::Sound* GetSound() const { return m_sound; }

            /*!***********************************************************************************
             \brief     Setter for FMOD::Channel* object.
             \param     channel The channel object.
            *************************************************************************************/
            void SetChannel(FMOD::Channel* channel) { m_channel = channel; }

            /*!***********************************************************************************
             \brief     Getter for FMOD::Channel* object.
             \return    FMOD::Channel* The channel object.
            *************************************************************************************/
            FMOD::Channel* GetChannel() const { return m_channel; }

        private:
            FMOD::Sound* m_sound = nullptr;
            FMOD::Channel* m_channel = nullptr;
        };

        /*!***********************************************************************************
         \brief     Initialize the FMOD audio system.
         \return    bool True if initialization is successful.
        *************************************************************************************/
        bool Init();

        /*!***********************************************************************************
         \brief     Update the audio system. Typically called per frame.
        *************************************************************************************/
        void Update();

        /*!***********************************************************************************
         \brief     Play a sound by its identifier.
         \param     id The identifier for the sound to be played.
        *************************************************************************************/
        void PlaySound(const std::string& id);

        /*!***********************************************************************************
         \brief     Set the volume for a specific sound.
         \param     id The identifier for the sound.
         \param     volume The new volume level (0.0 to 1.0).
        *************************************************************************************/
        void SetVolume(const std::string& id, float volume);

        /*!***********************************************************************************
         \brief     Set the global volume for all sounds.
         \param     volume The new global volume level (0.0 to 1.0).
        *************************************************************************************/
        void SetGlobalVolume(float volume);

        /*!***********************************************************************************
         \brief     Pause a specific sound.
         \param     id The identifier for the sound to be paused.
        *************************************************************************************/
        void PauseSound(const std::string& id);

        /*!***********************************************************************************
         \brief     Resume a paused sound.
         \param     id The identifier for the sound to be resumed.
        *************************************************************************************/
        void ResumeSound(const std::string& id);

        /*!***********************************************************************************
         \brief     Stop a specific sound.
         \param     id The identifier for the sound to be stopped.
        *************************************************************************************/
        void StopSound(const std::string& id);

        /*!***********************************************************************************
         \brief     Stop all currently playing sounds.
        *************************************************************************************/
        void StopAllSounds();

        /*!***********************************************************************************
         \brief     Get the FMOD system.
         \return    FMOD::System* The current FMOD system.
        *************************************************************************************/
        FMOD::System* GetFMODSystem() { return m_system; }

    private:
        /*!***********************************************************************************
         \brief     Private constructor for Meyer's Singleton.
        *************************************************************************************/
        AudioManager();

        /*!***********************************************************************************
         \brief     Destructor for cleaning up FMOD resources.
        *************************************************************************************/
        ~AudioManager();

        /*!***********************************************************************************
         \brief     The FMOD audio system.
        *************************************************************************************/
        FMOD::System* m_system;
    };
}
