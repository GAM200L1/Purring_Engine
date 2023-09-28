/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioManager.cpp
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

#include "prpch.h"
#include "AudioManager.h"
#include "ResourceManager/ResourceManager.h"
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

namespace PE
{
    /*!***********************************************************************************
    \brief     Constructor for the AudioManager class, initializes FMOD system to nullptr.

    \tparam T          This function does not use a template.
    \return void       Constructor does not return a value.
    *************************************************************************************/
    AudioManager::AudioManager()
        : m_system(nullptr)  // Initialize FMOD system to nullptr
    {
        // Constructor.
        // Additional code for future..
    }



    /*!***********************************************************************************
     \brief     Destructor for the AudioManager class, releases the FMOD system if it exists.

     \tparam T          This function does not use a template.
     \return void       Destructor does not return a value.
    *************************************************************************************/
    AudioManager::~AudioManager()
    {
        if (m_system)
            m_system->release();
    }



    /*!***********************************************************************************
    \brief     Initializes the AudioManager class by creating an FMOD system and setting it up.

    \tparam T          This function does not use a template.
    \return bool       Returns true if FMOD system initialization is successful, false otherwise.
    *************************************************************************************/
    bool AudioManager::Init()
    {
        FMOD_RESULT result = FMOD::System_Create(&m_system);
        if (result != FMOD_OK)
        {
            std::cout << "FMOD System_Create failed: " << FMOD_ErrorString(result) << "\n";
            return false;
        }

        result = m_system->init(512, FMOD_INIT_NORMAL, nullptr);
        if (result != FMOD_OK)
        {
            std::cout << "FMOD init failed: " << FMOD_ErrorString(result) << "\n";
            return false;
        }

        return true;
    }



    /*!***********************************************************************************
     \brief     Updates the FMOD system, usually called once per frame.

     \tparam T          This function does not use a template.
     \return void       Does not return a value.
    *************************************************************************************/
    void AudioManager::Update()
    {
        m_system->update();
    }



/*                                                                                                    Audio Controls
--------------------------------------------------------------------------------------------------------------------- */
    /*!***********************************************************************************
    \brief     Loads a sound from a specified path into the FMOD system.

    \tparam T          This function does not use a template.
    \param[in] path    The path to the sound file to be loaded.
    \param[in] system  Pointer to the FMOD::System that will manage the sound.
    \return bool       Returns true if the sound was loaded successfully, otherwise false.
    *************************************************************************************/
    bool AudioManager::Audio::LoadSound(const std::string& path, FMOD::System* system)
    {
        FMOD_RESULT result = system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &m_sound);
        return (result == FMOD_OK);
    }



    /*!***********************************************************************************
     \brief     Plays a sound associated with a given identifier string.

     \tparam T          This function does not use a template.
     \param[in] id      The identifier string associated with the sound to be played.
     \return void       Does not return a value.
    *************************************************************************************/
    void AudioManager::PlaySound(const std::string& id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end())
        {
            FMOD::Channel* channel = it->second->GetChannel();
            if (channel)
            {
                bool isPlaying = false;
                channel->isPlaying(&isPlaying);
                if (isPlaying)
                {
                    return;  // Exit the function as sound is already playing
                }
            }

            FMOD_RESULT result = m_system->playSound(it->second->GetSound(), nullptr, false, &channel);
            if (result == FMOD_OK)
            {
                it->second->SetChannel(channel);
            }
            else
            {
                std::cout << "Failed to play sound: " << FMOD_ErrorString(result) << "\n";
            }
        }
        else
        {
            std::cout << "Sound not found for id: " << id << "\n";
        }
    }



    /*!***********************************************************************************
     \brief     Sets the volume for a sound associated with a given identifier string.

     \tparam T          This function does not use a template.
     \param[in] id      The identifier string associated with the sound for which the volume will be set.
     \param[in] volume  The volume level to set for the sound, typically between 0.0 (mute) and 1.0 (full volume).
     \return void       Does not return a value.
    *************************************************************************************/
    void AudioManager::SetVolume(const std::string& id, float volume)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setVolume(volume);
        }
    }



    /*!***********************************************************************************
     \brief     Sets the global volume for all sounds currently managed by the ResourceManager.

     \tparam T          This function does not use a template.
     \param[in] volume  The volume level to set for all sounds, typically between 0.0 (mute) and 1.0 (full volume).
     \return void       Does not return a value.
    *************************************************************************************/
    void AudioManager::SetGlobalVolume(float volume)
    {
        for (auto& pair : ResourceManager::GetInstance().Sounds)
        {
            FMOD::Channel* channel = pair.second->GetChannel();
            if (channel)
            {
                channel->setVolume(volume);
            }
        }
    }



    /*!***********************************************************************************
     \brief     Pauses a sound associated with a given identifier string if it is currently playing.

     \tparam T          This function does not use a template.
     \param[in] id      The identifier string associated with the sound to be paused.
     \return void       Does not return a value.
    *************************************************************************************/
    void AudioManager::PauseSound(const std::string& id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setPaused(true);
        }
    }



    /*!***********************************************************************************
     \brief     Resumes a paused sound associated with a given identifier string if it is currently paused.

     \tparam T          This function does not use a template.
     \param[in] id      The identifier string associated with the sound to be resumed.
     \return void       Does not return a value.
    *************************************************************************************/
    void AudioManager::ResumeSound(const std::string& id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->setPaused(false);
        }
    }



    /*!***********************************************************************************
     \brief     Stops a sound associated with a given identifier string if it is currently playing.

     \tparam T          This function does not use a template.
     \param[in] id      The identifier string associated with the sound to be stopped.
     \return void       Does not return a value.
    *************************************************************************************/
    void AudioManager::StopSound(const std::string& id)
    {
        auto it = ResourceManager::GetInstance().Sounds.find(id);
        if (it != ResourceManager::GetInstance().Sounds.end() && it->second->GetChannel())
        {
            it->second->GetChannel()->stop();
        }
    }


    /*!***********************************************************************************
     \brief     Stops all sounds that are currently being managed by the ResourceManager.

     \tparam T          This function does not use a template.
     \return void       Does not return a value.
    *************************************************************************************/
    void AudioManager::StopAllSounds()
    {
        for (auto& pair : ResourceManager::GetInstance().Sounds)
        {
            FMOD::Channel* channel = pair.second->GetChannel();
            if (channel)
            {
                channel->stop();
            }
        }
    }
}
