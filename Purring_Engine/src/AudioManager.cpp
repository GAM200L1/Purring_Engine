/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioManager.cpp
 \date     16-09-2023

 \author               You Yang ONG
 \par      email:      youyang.o@digipen.edu

 \brief    This file contains the AudioManager class, which manages
           audio playback and includes FMOD as the underlying audio system.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "AudioManager.h"
#include <iostream>
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

//single static instance of imguiwindow 
std::unique_ptr<AudioManager> AudioManager::s_Instance = nullptr;



/*-----------------------------------------------------------------------------
/// <summary>
/// Default constructor for the AudioManager class.
/// Initializes member pointers to nullptr, ensuring they are in a safe state.
/// FMOD system, sound, and channel resources will be allocated later in Init().
/// </summary>
----------------------------------------------------------------------------- */
AudioManager::AudioManager()
    : system(nullptr),
    sound1(nullptr),
    sound2(nullptr),
    channel1(nullptr),
    channel2(nullptr)
{
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Default constructor for the AudioManager class.
/// Initializes member pointers to nullptr, ensuring they are in a safe state.
/// FMOD system, sound, and channel resources will be allocated later in Init().
/// </summary>
----------------------------------------------------------------------------- */
AudioManager::~AudioManager()
{
    if (sound1) sound1->release();
    if (sound2) sound2->release();
    if (system) system->release();
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Initializes the FMOD audio system and sound resources.
/// This method will create the FMOD System object, initialize it,
/// and also create Sound objects for sound1 and sound2.
/// </summary>
/// <returns>
/// Returns true if all initializations are successful, otherwise returns false.
/// </returns>
----------------------------------------------------------------------------- */
bool AudioManager::Init()
{
    // Create the FMOD System object
    FMOD_RESULT result = FMOD::System_Create(&system);

    // Check if FMOD System object was successfully created
    if (result != FMOD_OK)
    {
        std::cout << "FMOD System_Create failed: " << FMOD_ErrorString(result) << "\n";
        return false;                           // Return false if init failed
    }

    // Initialize the FMOD System object with 512 channels
    result = system->init(512, FMOD_INIT_NORMAL, 0);

    // Check if FMOD System was successfully initialized
    if (result != FMOD_OK)
    {
        std::cout << "FMOD init failed: " << FMOD_ErrorString(result) << "\n";
        return false;                           // Return false if init failed
    }

    // Create the sound object for sound1
    result = system->createSound("Audio/sound1.wav", FMOD_DEFAULT, 0, &sound1);

    // Check if sound1 was successfully created
    if (result != FMOD_OK)
    {
        std::cout << "FMOD createSound failed for sound1: " << FMOD_ErrorString(result) << "\n";
        return false;                           // Return false if init failed
    }

    // Create the sound object for sound2
    result = system->createSound("Audio/sound2.wav", FMOD_DEFAULT, 0, &sound2);

    // Check if sound2 was successfully created
    if (result != FMOD_OK)
    {
        std::cout << "FMOD createSound failed for sound2: " << FMOD_ErrorString(result) << "\n";
        return false;                           // Return false if init failed
    }

    return true;
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Updates the FMOD audio system.
/// This method calles once per frame keeping the FMOD system updated.
/// </summary>
----------------------------------------------------------------------------- */
void AudioManager::Update()
{
    system->update();
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Plays a sound specified by the file path.
/// The method maps the given filePath to a preloaded FMOD::Sound object and plays it.
/// </summary>
/// <param name="filePath">
/// The file path of the sound to be played.
/// </param>
----------------------------------------------------------------------------- */
void AudioManager::PlaySound(const char* filePath)
{
    FMOD::Sound* sound = nullptr;

    if (strcmp(filePath, "Audio/sound1.wav") == 0) {
        sound = sound1;
    }
    else if (strcmp(filePath, "Audio/sound2.wav") == 0) {
        sound = sound2;
    }

    if (sound) {
        FMOD_RESULT result = system->playSound(sound, 0, false, &channel1);
        if (result != FMOD_OK) {
            std::cout << "Failed to play sound: " << FMOD_ErrorString(result) << "\n";
        }
    }
    else {
        std::cout << "Sound not found for filePath: " << filePath << "\n";
    }
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Stops any sound currently being played on channel1 and channel2.
/// This method checks if the channels are initialized and then stops the sound playback.
/// </summary>
----------------------------------------------------------------------------- */
void AudioManager::StopSound()
{
    if (channel1) channel1->stop();
    if (channel2) channel2->stop();
}

AudioManager* AudioManager::GetInstance()
{
    //may need to make another function to manually allocate memory for this 
    if (!s_Instance)
        s_Instance = std::make_unique<AudioManager>();

    return s_Instance.get();
}
