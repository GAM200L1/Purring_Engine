/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioSystem.h
 \date     2-03-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    FMOD Studio Audio System setup.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Singleton.h"

#include "AudioManager/FMODStudio/FmodStudioManager.h"

namespace PE
{
    namespace Audio
    {
        /*!***********************************************************************************
         \brief A utility template for creating unique pointers to objects.
        *************************************************************************************/
        template<typename T>
        using UniquePtr = std::unique_ptr<T>;

        /*!***********************************************************************************
         \brief A utility function to simplify the creation of unique pointers.

         \param[in] args - Arguments to forward to the constructor of the object.
         \return A unique pointer to the newly created object.
        *************************************************************************************/
        template<typename T, typename ... Args>
        constexpr UniquePtr<T> CreateUniquePtr(Args&& ... args)
        {
            return std::make_unique<T>(std::forward<Args>(args)...);
        }

        class AudioSystem : public Singleton<AudioSystem> {
        public:
            friend class Singleton<AudioSystem>;

            /*!***********************************************************************************
             \brief Initializes the audio system and sets up necessary resources.
            *************************************************************************************/
            void Init();

            /*!***********************************************************************************
             \brief Shuts down the audio system and releases all resources.
            *************************************************************************************/
            void Shutdown();

            /*!***********************************************************************************
             \brief Updates the audio system, processing audio events and managing playback.

             \param[in] dt - The time elapsed since the last frame.
            *************************************************************************************/
            void OnUpdate(float dt);

            /*!***********************************************************************************
             \brief Loads an audio bank with specified flags.

             \param[in] bankName - The name of the bank to load.
             \param[in] flags    - Flags specifying how the bank should be loaded.
            *************************************************************************************/
            void LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);

            /*!***********************************************************************************
             \brief Loads an audio event by name.

             \param[in] eventName - The name of the event to load.
            *************************************************************************************/
            void LoadEvent(const std::string& eventName);

            /*!***********************************************************************************
             \brief Stops a currently playing event.

             \param[in] eventName   - The name of the event to stop.
             \param[in] isImmediate - Whether to stop the event immediately.
            *************************************************************************************/
            void StopEvent(const std::string& eventName, bool isImmediate = true);

            /*!***********************************************************************************
             \brief Loads a sound with specified properties.

             \param[in] soundName - The name of the sound to load.
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
             \brief Sets the attributes for the audio listener in the 3D audio environment.

             \param[in] listener - The index of the listener.
             \param[in] pos      - The position vector of the listener.
             \param[in] velocity - The velocity vector of the listener.
            *************************************************************************************/
            void SetListenerAttributes(int listener, const glm::vec3& pos, const glm::vec3& velocity);

            /*!***********************************************************************************
             \brief Plays a sound at a specified position with a given volume.

             \param[in] soundName - The name of the sound to play.
             \param[in] pos       - The position in the 3D world where the sound should originate.
             \param[in] volumedB  - The volume of the sound in decibels.
             \return An integer representing the channel ID of the played sound.
            *************************************************************************************/
            int PlaySound(const std::string& soundName, const glm::vec3& pos, float volumedB);

            /*!***********************************************************************************
             \brief Plays an audio event.

             \param[in] eventName   - The name of the event to play.
             \param[in] isImmediate - Whether the event should play immediately.
            *************************************************************************************/
            void PlayEvent(const std::string& eventName, bool isImmediate = false);

            /*!***********************************************************************************
             \brief Stops all currently playing channels.
            *************************************************************************************/
            void StopAllChannels();

            /*!***********************************************************************************
             \brief Sets the 3D position of a channel in the audio environment.

             \param[in] channelId - The ID of the channel.
             \param[in] position  - The new position vector for the channel.
            *************************************************************************************/
            void SetChannel3dPosition(int channelId, const glm::vec3& position);

            /*!***********************************************************************************
             \brief Sets the volume for a channel.

             \param[in] channelId - The ID of the channel.
             \param[in] dB        - The volume level in decibels.
            *************************************************************************************/
            void SetChannelVolume(int channelId, float dB);

            /*!***********************************************************************************
             \brief Checks if a channel is currently playing.

             \param[in] channelId - The ID of the channel to check.
             \return True if the channel is playing a sound, false otherwise.
            *************************************************************************************/
            bool IsPlaying(int channelId);

            /*!***********************************************************************************
             \brief Checks if an audio event is currently playing.

             \param[in] eventName - The name of the event to check.
             \return True if the event is playing, false otherwise.
            *************************************************************************************/
            bool IsEventPlaying(const std::string& eventName);

        private:
            AudioSystem();
            ~AudioSystem();

            // Deleted copy and move constructors and assignment operators.
            AudioSystem(const AudioSystem&) = delete;
            AudioSystem& operator=(const AudioSystem&) = delete;
            AudioSystem(AudioSystem&&) = delete;
            AudioSystem& operator=(AudioSystem&&) = delete;

            /*!***********************************************************************************
             \brief A unique pointer to the FmodStudioManager, ensuring proper resource management.
            *************************************************************************************/
            static UniquePtr<FmodStudioManager> m_fmod;
        };

    } // namespace Audio

} // namespace PE
