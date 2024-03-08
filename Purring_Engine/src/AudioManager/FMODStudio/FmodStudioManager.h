/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FmodStudioManager.h
 \date     2-03-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    FMOD Studio FmodStudioManager setup.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <map>
#include <string>
#include <glm/glm.hpp>

#include "Singleton.h"

namespace PE
{
    namespace Audio
    {

        class FmodStudioManager : public Singleton<FmodStudioManager>
        {
            friend class Singleton<FmodStudioManager>;
        public:

            /*!***********************************************************************************
             \brief Retrieves the singleton instance of the FmodStudioManager.

             \return A reference to the FmodStudioManager instance.
            *************************************************************************************/
            static FmodStudioManager& GetInstance()
            {
                static FmodStudioManager instance;
                return instance;
            }
            
            /*!***********************************************************************************
             \brief Updates FMOD systems and processes any pending audio tasks.

             \param[in] dt - The time elapsed since the last frame.
            *************************************************************************************/
            void OnUpdate(float dt);

            /*!***********************************************************************************
             \brief Retrieves an FMOD Studio Bank object by name.

             \param[in] bankName - The name of the bank to retrieve.
             \return A pointer to the FMOD::Studio::Bank object.
            *************************************************************************************/
            FMOD::Studio::Bank* GetBank(const std::string& bankName) const;

            /*!***********************************************************************************
             \brief Loads an FMOD Studio bank file.

             \param[in] bankName - The name of the bank to load.
             \param[in] flags    - Flags specifying how the bank is to be loaded.
            *************************************************************************************/
            void LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);

            /*!***********************************************************************************
             \brief Loads an audio event by name.

             \param[in] eventName - The name of the event to load.
            *************************************************************************************/
            void LoadEvent(const std::string& eventName);

            /*!***********************************************************************************
             \brief Loads a sound from file, with options for 3D spatialization, looping, and streaming.

             \param[in] soundName - The name of the sound file to load.
             \param[in] is3d      - Whether the sound should be spatialized.
             \param[in] isLooping - Whether the sound should loop.
             \param[in] isStream  - Whether the sound should be streamed.
            *************************************************************************************/
            void LoadSound(const std::string& soundName, bool is3d, bool isLooping, bool isStream);

            /*!***********************************************************************************
             \brief Unloads a previously loaded sound.

             \param[in] soundName - The name of the sound to unload.
            *************************************************************************************/
            void UnloadSound(const std::string& soundName);

            /*!***********************************************************************************
             \brief Sets attributes for a listener in the 3D audio environment.

             \param[in] listener - The index of the listener to modify.
             \param[in] pos      - The position vector of the listener.
             \param[in] velocity - The velocity vector of the listener.
            *************************************************************************************/
            void SetListenerAttributes(int listener, const glm::vec3& pos, const glm::vec3& velocity);

            /*!***********************************************************************************
             \brief Plays a sound at a specified position in the 3D world.

             \param[in] soundName - The name of the sound to play.
             \param[in] pos       - The position in the 3D world where the sound should originate.
             \param[in] volumedB  - The volume of the sound in decibels.
             \return An integer channel ID for the sound instance.
            *************************************************************************************/
            int PlaySound(const std::string& soundName, const glm::vec3& pos, float volumedB);

            /*!***********************************************************************************
             \brief Plays an FMOD Studio event.

             \param[in] eventName   - The name of the event to play.
             \param[in] isImmediate - Whether the event should play immediately without fade.
            *************************************************************************************/
            void PlayEvent(const std::string& eventName, bool isImmediate);

            /*!***********************************************************************************
             \brief Stops an FMOD Studio event.

             \param[in] eventName   - The name of the event to stop.
             \param[in] isImmediate - Whether the event should stop immediately without fade.
            *************************************************************************************/
            void StopEvent(const std::string& eventName, bool isImmediate);

            /*!***********************************************************************************
             \brief Stops all currently playing audio channels.
            *************************************************************************************/
            void StopAllChannels();

            /*!***********************************************************************************
             \brief Sets the 3D position of a channel in the audio environment.

             \param[in] channelId - The ID of the channel to modify.
             \param[in] position  - The new position vector for the channel.
            *************************************************************************************/
            void SetChannel3dPosition(int channelId, const glm::vec3& position);

            /*!***********************************************************************************
             \brief Sets the volume for an audio channel.

             \param[in] channelId - The ID of the channel to modify.
             \param[in] dB        - The volume level in decibels.
            *************************************************************************************/
            void SetChannelVolume(int channelId, float dB);

            /*!***********************************************************************************
             \brief Checks if a channel is currently playing a sound.

             \param[in] channelId - The ID of the channel to check.
             \return True if the channel is playing a sound, otherwise false.
            *************************************************************************************/
            bool IsPlaying(int channelId);

            /*!***********************************************************************************
             \brief Checks if an FMOD Studio event is currently playing.

             \param[in] eventName - The name of the event to check.
             \return True if the event is playing, otherwise false.
            *************************************************************************************/
            bool IsEventPlaying(const std::string& eventName);

            /*!***********************************************************************************
             \brief Converts a volume level in decibels to a linear scale.

             \param[in] dB - The volume level in decibels.
             \return The volume on a linear scale.
            *************************************************************************************/
            float dBToVolume(float dB);

            /*!***********************************************************************************
             \brief Converts a linear volume level to decibels.

             \param[in] volume - The volume on a linear scale.
             \return The volume level in decibels.
            *************************************************************************************/
            float VolumeTodB(float volume);

        protected:
            FmodStudioManager();
            ~FmodStudioManager();

        private:
            FMOD::System* coreSystem{ nullptr };
            FMOD::Studio::System* studioSystem{ nullptr };

            int nextChannelId{ 0 };

            using SoundMap = std::map<std::string, FMOD::Sound*>;
            using ChannelMap = std::map<int, FMOD::Channel*>;
            using EventMap = std::map<std::string, FMOD::Studio::EventInstance*>;
            using BankMap = std::map<std::string, FMOD::Studio::Bank*>;

            // Maps to keep track of loaded sounds, channels, events, and banks.
            SoundMap sounds;
            ChannelMap channels;
            EventMap events;
            BankMap banks;
        };

    } // namespace Audio

} // namespace PE
