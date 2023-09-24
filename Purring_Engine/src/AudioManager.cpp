/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioManager.cpp
 \date     16-09-2023
 \last updated 24-09-2023

 \author               You Yang ONG
 \par      email:      youyang.o@digipen.edu

 \brief    This file contains the AudioManager class, which manages
           audio playback and includes FMOD as the underlying audio system.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "AudioManager.h"
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------
/// <summary>
/// Utilizes the Meyer's Singleton approach for the AudioManager class.
/// This function enforces the Singleton design pattern to guarantee a single
/// AudioManager instance within the system at all times.
/// </summary>
///
/// <returns>
/// Provides the unique instance of AudioManager.
/// </returns>
---------------------------------------------------------------------------*/
AudioManager& AudioManager::GetInstance()
{
    static AudioManager instance;
    return instance;
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Constructor for the AudioManager class.
/// Initializes the FMOD system to nullptr.
/// </summary>
----------------------------------------------------------------------------- */
AudioManager::AudioManager()
    : m_system(nullptr)  // Initialize FMOD system to nullptr
{
    // Constructor.
    // Additional code for future..
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Destructor for the AudioManager class.
/// Releases the FMOD system resources if they have been allocated.
/// </summary>
----------------------------------------------------------------------------- */
AudioManager::~AudioManager()
{
    if (m_system)
        m_system->release();
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Initializes the FMOD audio system for the AudioManager class.
/// </summary>
///
/// <returns>
/// Returns true if the FMOD audio system is successfully initialized, false otherwise.
/// </returns>
----------------------------------------------------------------------------- */
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



/*-----------------------------------------------------------------------------
/// <summary>
/// Updates the FMOD audio system for the AudioManager class.
/// This function should be called regularly to ensure the audio system
/// processes its internal updates.
/// </summary>
----------------------------------------------------------------------------- */
void AudioManager::Update()
{
    m_system->update();
}



/*                                                                                                    Audio Controls
--------------------------------------------------------------------------------------------------------------------- */

/*-----------------------------------------------------------------------------
/// <summary>
/// Loads a sound file into an FMOD sound object.
/// </summary>
///
/// <param name="path">
/// The file path of the sound to be loaded.
/// </param>
///
/// <param name="system">
/// A pointer to the FMOD System object.
/// </param>
///
/// <returns>
/// Returns true if the sound was successfully loaded, false otherwise.
/// </returns>
----------------------------------------------------------------------------- */
bool AudioManager::Audio::LoadSound(const std::string& path, FMOD::System* system)
{
    FMOD_RESULT result = system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &m_sound);
    return (result == FMOD_OK);
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Loads an audio file and associates it with a unique identifier.
/// </summary>
///
/// <param name="id">
/// The unique identifier for the audio file.
/// </param>
///
/// <param name="path">
/// The file path of the audio file to load.
/// </param>
///
/// <returns>
/// Returns true if the audio file is successfully loaded and associated with the given ID, false otherwise.
/// </returns>
----------------------------------------------------------------------------- */
bool AudioManager::LoadAudio(const std::string& id, const std::string& path)
{
    Audio audio;
    if (audio.LoadSound(path, m_system))
    {
        m_audioMap[id] = std::move(audio);
        return true;
    }
    return false;
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Plays a sound associated with a given identifier.
/// </summary>
///
/// <param name="id">
/// The unique identifier for the audio to be played.
/// </param>
///
/// <remarks>
/// If the sound is not found, an error message will be printed to the console.
/// If the sound cannot be played, an FMOD-specific error message will be printed.
/// </remarks>
----------------------------------------------------------------------------- */
void AudioManager::PlaySound(const std::string& id)
{
    auto it = m_audioMap.find(id);
    if (it != m_audioMap.end())
    {
        FMOD::Channel* channel = nullptr;
        FMOD_RESULT result = m_system->playSound(it->second.GetSound(), nullptr, false, &channel);
        if (result == FMOD_OK)
        {
            it->second.SetChannel(channel);
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



/*-----------------------------------------------------------------------------
/// <summary>
/// Sets the volume for a sound associated with a given identifier.
/// </summary>
///
/// <param name="id">
/// The unique identifier for the audio whose volume is to be set.
/// </param>
///
/// <param name="volume">
/// The new volume level (as a float) to be set for the audio.
/// </param>
///
/// <remarks>
/// If the sound corresponding to the identifier is not found, no operation is performed.
/// If the channel corresponding to the sound is not active, no operation is performed.
/// </remarks>
----------------------------------------------------------------------------- */
void AudioManager::SetVolume(const std::string& id, float volume)
{
    auto it = m_audioMap.find(id);
    if (it != m_audioMap.end() && it->second.GetChannel())
    {
        it->second.GetChannel()->setVolume(volume);
    }
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Sets the global volume for all currently active sounds.
/// </summary>
///
/// <param name="volume">
/// The new global volume level (as a float) to be set for all active audios.
/// </param>
///
/// <remarks>
/// Iterates through all entries in the audio map. If a channel is active for
/// a particular sound, its volume will be set to the new global volume level.
/// </remarks>
----------------------------------------------------------------------------- */
void AudioManager::SetGlobalVolume(float volume)
{
    for (auto& pair : m_audioMap)
    {
        FMOD::Channel* channel = pair.second.GetChannel();
        if (channel)
        {
            channel->setVolume(volume);
        }
    }
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Pauses the sound associated with a given identifier.
/// </summary>
///
/// <param name="id">
/// The unique identifier for the audio to be paused.
/// </param>
///
/// <remarks>
/// Searches for the sound using the provided identifier in the audio map.
/// If the sound is found and its channel is active, the sound will be paused.
/// </remarks>
----------------------------------------------------------------------------- */
void AudioManager::PauseSound(const std::string& id)
{
    auto it = m_audioMap.find(id);
    if (it != m_audioMap.end() && it->second.GetChannel())
    {
        it->second.GetChannel()->setPaused(true);
    }
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Resumes the sound associated with a given identifier.
/// </summary>
///
/// <param name="id">
/// The unique identifier for the audio to be resumed.
/// </param>
///
/// <remarks>
/// Searches for the sound using the provided identifier in the audio map.
/// If the sound is found and its channel is active, the sound will be resumed.
/// </remarks>
----------------------------------------------------------------------------- */
void AudioManager::ResumeSound(const std::string& id)
{
    auto it = m_audioMap.find(id);
    if (it != m_audioMap.end() && it->second.GetChannel())
    {
        it->second.GetChannel()->setPaused(false);
    }
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Stops playing the sound associated with a given identifier.
/// </summary>
///
/// <param name="id">
/// The unique identifier for the audio to be stopped.
/// </param>
///
/// <remarks>
/// The function will only attempt to stop the sound if it exists in the audio map and has an associated channel.
/// </remarks>
----------------------------------------------------------------------------- */
void AudioManager::StopSound(const std::string& id)
{
    auto it = m_audioMap.find(id);
    if (it != m_audioMap.end() && it->second.GetChannel())
    {
        it->second.GetChannel()->stop();
    }
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Stops ALL currently playing sounds.
/// </summary>
///
/// <remarks>
/// This function iterates through the audio map and stops any sounds that are currently playing and have an associated channel.
/// </remarks>
----------------------------------------------------------------------------- */
void AudioManager::StopAllSounds()
{
    for (auto& pair : m_audioMap)
    {
        FMOD::Channel* channel = pair.second.GetChannel();
        if (channel)
        {
            channel->stop();
        }
    }
}
