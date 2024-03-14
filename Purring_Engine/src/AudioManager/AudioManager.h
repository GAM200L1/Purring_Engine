/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioManager.h
 \date     10-09-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief	   This file features the AudioManager class that encapsulates the FMOD library API
           for audio operations. It initializes the FMOD system and controls its lifecycle,
           provides a series of methods for sound playback, and interfaces seamlessly with
           the ResourceManager to manage audio assets.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#pragma warning(disable : 4505) // warning C4505: ('FMOD_ErrorString': unreferenced function with internal linkage has been removed)

#include "fmod.hpp"
#include "fmod_errors.h"
#include "Singleton.h"
#include "System.h"
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
             \param     r_path      - Path to the audio file.
             \param     p_system    - FMOD::System pointer for audio system management.
             \return    bool True if the sound is successfully loaded.
            *************************************************************************************/
            bool LoadSound(const std::string& r_path, FMOD::System* p_system);

            /*!***********************************************************************************
             \brief     Getter for FMOD::Sound* object.
             \return    FMOD::Sound* The sound object.
            *************************************************************************************/
            FMOD::Sound* GetSound() const { return p_sound; }

            /*!***********************************************************************************
             \brief     Setter for FMOD::Channel* object.
             \param     channel The channel object.
            *************************************************************************************/
            void SetChannel(FMOD::Channel* channel) { p_channel = channel; }

            /*!***********************************************************************************
             \brief     Getter for FMOD::Channel* object.
             \return    FMOD::Channel* The channel object.
            *************************************************************************************/
            FMOD::Channel* GetChannel() const { return p_channel; }

        private:
            FMOD::Sound* p_sound = nullptr;
            FMOD::Channel* p_channel = nullptr;
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
         \brief     Set master volume level.
         \param     volume - float to set the new volume
        *************************************************************************************/
        void SetMasterVolume(float volume);

        /*!***********************************************************************************
         \brief     Set background music volume level.
         \param     volume - float to set the new volume
        *************************************************************************************/
        void SetBGMVolume(float volume);

        /*!***********************************************************************************
         \brief     Set sound effects volume level.
         \param     volume - float to set the new volume
        *************************************************************************************/
        void SetSFXVolume(float volume);

        /*!***********************************************************************************
         \brief     Set sound effects volume level.
         \param     mute - true to mute, false to unmute
        *************************************************************************************/
        void MuteMaster(bool mute);

        /*!***********************************************************************************
         \brief     Set sound effects volume level.
         \param     mute - true to mute, false to unmute
        *************************************************************************************/
        void MuteBGM(bool mute);

        /*!***********************************************************************************
         \brief     Set sound effects volume level.
         \param     mute - true to mute, false to unmute
        *************************************************************************************/
        void MuteSFX(bool mute);

        /*!***********************************************************************************
         \brief     Getter for BGM channel group.
         \return    FMOD::ChannelGroup* - Pointer to the channel group
        *************************************************************************************/
        FMOD::ChannelGroup* GetBGMGroup() const;

        /*!***********************************************************************************
         \brief     Getter for SFX channel group.
         \return    FMOD::ChannelGroup* - Pointer to the channel group
        *************************************************************************************/
        FMOD::ChannelGroup* GetSFXGroup() const;

        /*!***********************************************************************************
         \brief     Getter for Master Volume.
         \return    float - Current master volume
        *************************************************************************************/
        float GetMasterVolume() const;

        /*!***********************************************************************************
         \brief     Getter for Master Mute.
         \return    bool - true for muted, false for unmuted
        *************************************************************************************/
        bool GetMasterMute() const;

        /*!***********************************************************************************
         \brief     Set the global volume for all sounds.
         \param     volume The new global volume level (0.0 to 1.0).
        *************************************************************************************/
        void SetGlobalVolume(float volume);

        /*!***********************************************************************************
         \brief     Stop all currently playing sounds.
        *************************************************************************************/
        void StopAllSounds();

        /*!***********************************************************************************
         \brief     Get the FMOD system.
         \return    FMOD::System* The current FMOD system.
        *************************************************************************************/
        FMOD::System* GetFMODSystem() { return p_system; }

    private:
        /*!***********************************************************************************
         \brief     Private constructor for Meyer's Singleton.
        *************************************************************************************/
        AudioManager();

        /*!***********************************************************************************
         \brief     Private destructor for Meyer's Singleton.
        *************************************************************************************/
        ~AudioManager();

        // Audio Variables
        FMOD::System* p_system;

        FMOD::ChannelGroup* p_masterGroup;  // Master channel group
        FMOD::ChannelGroup* p_bgmGroup;     // BGM channel group
        FMOD::ChannelGroup* p_sfxGroup;     // SFX channel group

    };
}
