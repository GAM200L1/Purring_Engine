/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioComponent.h
 \date     2-10-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    The AudioComponent class is a part of the Purring Engine, designed to manage
           audio playback functionalities for game entities. It handles audio operations
           such as playing, pausing, resuming, and stopping sounds. Additionally, this class
           provides capabilities to adjust volume, set looping, and maintain audio keys for
           individual entities. It also includes serialization support for converting audio
           component states to and from JSON format, facilitating easy data management and
           integration with the engine's audio systems.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include <string>
#include "Data/json.hpp"
#include "AudioManager.h"


namespace PE
{
    /*!***********************************************************************************
     \brief     AudioComponent class is used to store the audio file key for an entity.
    *************************************************************************************/
    class AudioComponent
    {
    public:
        /*!***********************************************************************************
        \brief     Constructor for the AudioComponent class.
        *************************************************************************************/
        AudioComponent();

        /*!***********************************************************************************
        \brief     Destructor for the AudioComponent class.
        *************************************************************************************/
        ~AudioComponent();

        /*!***********************************************************************************
        \brief     Displays an audio error message.
        \param     r_message The error message to be displayed.
        \param     r_title The title of the error message box. Default is "Error".
        *************************************************************************************/
        static void ShowErrorMessage(const std::string& r_message, const std::string& r_title = "Error");

        /*!***********************************************************************************
        \brief     Plays an audio sound associated with the given identifier.
        \param     r_id The identifier of the audio sound.
        *************************************************************************************/
        void PlayAudioSound();

        /*!***********************************************************************************
        \brief     Checks if the audio sound with the specified identifier is currently playing.
        \param     r_id The identifier of the audio sound.
        *************************************************************************************/
        bool IsPlaying() const;

        /*!***********************************************************************************
        \brief     Sets the volume for the audio sound with the specified identifier.
        \param     r_id The identifier of the audio sound.
        \param     volume The volume level to set.
        *************************************************************************************/
        void SetVolume(float volume);

        /*!***********************************************************************************
        \brief     Pauses the audio sound with the given identifier.
        \param     r_id The identifier of the audio sound.
        *************************************************************************************/
        void PauseSound();

        /*!***********************************************************************************
        \brief     Resumes the paused audio sound with the specified identifier.
        \param     r_id The identifier of the audio sound.
        *************************************************************************************/
        void ResumeSound();

        /*!***********************************************************************************
        \brief     Stops the audio sound associated with the given identifier.
        \param     r_id The identifier of the audio sound.
        *************************************************************************************/
        void StopSound();

        /*!***********************************************************************************
        \brief     Gets the current audio file key.
        \return    The audio file key.
        *************************************************************************************/
        std::string const& GetAudioKey() const;

        /*!***********************************************************************************
        \brief     Sets a new audio file key.
        \param     r_newKey The new audio file key to set.
        *************************************************************************************/
        void SetAudioKey(const std::string& audiokey);

        /*!***********************************************************************************
        \brief     Sets the audio to loop or not.
        \param     loop True if the audio should loop, false otherwise.
        *************************************************************************************/
        void SetLoop(bool loop) { m_loop = loop; }

        /*!***********************************************************************************
        \brief     Gets whether the audio is looping or not.
        \return    True if the audio is looping, false otherwise.
        *************************************************************************************/
        bool IsLooping() const { return m_loop; }

         /*!***********************************************************************************
        \brief     Sets the audio to loop or not.
        \param     loop True if the audio should loop, false otherwise.
        *************************************************************************************/
        void SetPause(bool p) { isPaused = p; }

        /*!***********************************************************************************
        \brief     Gets whether the audio is looping or not.
        \return    True if the audio is looping, false otherwise.
        *************************************************************************************/
        bool IsPaused() const { return isPaused; }

        /*!***********************************************************************************
        \brief     Gets the audio channel.
        \return    The audio channel.
        *************************************************************************************/
        FMOD::Channel* GetChannel() const;

        /*!***********************************************************************************
        \brief     Converts the AudioComponent state to JSON format.
        \return    JSON representation of the AudioComponent state.
        *************************************************************************************/
        nlohmann::json ToJson() const;

        /*!***********************************************************************************
        \brief     Constructs an AudioComponent from its JSON representation.
        \param     r_j JSON object representing the AudioComponent.
        \return    An instance of AudioComponent.
        *************************************************************************************/
        static AudioComponent FromJson(const nlohmann::json& r_j);

    private:
        std::string m_audioKey;         // The audio file key
        bool m_loop = false;            // Whether the audio should loop or not
        bool isPaused = false;          // Whether the audio is paused or not
    };
}
