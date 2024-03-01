#include "prpch.h"
#include <vector>
#include <fmod_errors.h>

#include "AudioManager/FMODStudio/FmodStudioManager.h"
#include "Logging/Logger.h"

namespace PE
{
    namespace Audio
    {

        extern Logger g_AudioLogger;

        FmodStudioManager::FmodStudioManager()
        {
            FMOD_RESULT result = FMOD::Studio::System::create(&studioSystem);
            if (result != FMOD_OK)
            {
                g_AudioLogger.AddLog(true, "FMOD System could not be created: " + std::string(FMOD_ErrorString(result)), __FUNCTION__);
                exit(-1);
            }
            studioSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
            studioSystem->getCoreSystem(&coreSystem);
        }

        FmodStudioManager::~FmodStudioManager()
        {
            if (studioSystem)
            {
                studioSystem->unloadAll();
                studioSystem->release();
            }
            if (coreSystem)
            {
                coreSystem->release();
            }
        }

        void FmodStudioManager::OnUpdate(float dt)
        {
            std::vector<ChannelMap::iterator> stoppedChannels;
            for (auto it = channels.begin(), itEnd = channels.end(); it != itEnd; ++it)
            {
                bool isPlaying = false;
                it->second->isPlaying(&isPlaying);
                if (!isPlaying)
                {
                    stoppedChannels.push_back(it);
                }
            }

            for (auto& it : stoppedChannels)
            {
                channels.erase(it);
            }
            studioSystem->update();
            g_AudioLogger.AddLog(false, "FMOD Studio system updated successfully.", __FUNCTION__);
        }


        FMOD::Studio::Bank* FmodStudioManager::GetBank(const std::string& bankName) const
        {
            auto it = banks.find(bankName);
            return it != banks.end() ? it->second : nullptr;
        }

        void FmodStudioManager::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
        {
            if (GetBank(bankName))
            {
                return; // Bank already loaded
            }

            FMOD::Studio::Bank* bank;
            auto result = studioSystem->loadBankFile(bankName.c_str(), flags, &bank);
            if (result == FMOD_OK)
            {
                banks[bankName] = bank;
            }
            else
            {
                // Handle error
            }
        }

        void FmodStudioManager::LoadEvent(const std::string& eventName)
        {
            if (events.find(eventName) != events.end())
                return; // Event already loaded

            FMOD::Studio::EventDescription* eventDescription = nullptr;
            FMOD_RESULT result = studioSystem->getEvent(eventName.c_str(), &eventDescription);
            if (result == FMOD_OK && eventDescription)
            {
                FMOD::Studio::EventInstance* eventInstance = nullptr;
                eventDescription->createInstance(&eventInstance);
                if (eventInstance)
                {
                    events[eventName] = eventInstance;
                }
            }
        }

        void FmodStudioManager::LoadSound(const std::string& soundName, bool is3d, bool isLooping, bool isStream)
        {
            if (sounds.find(soundName) != sounds.end())
                return; // Sound already loaded

            FMOD_MODE mode = FMOD_DEFAULT;
            mode |= is3d ? FMOD_3D : FMOD_2D;
            mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
            mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

            FMOD::Sound* sound = nullptr;
            FMOD_RESULT result = coreSystem->createSound(soundName.c_str(), mode, nullptr, &sound);
            if (result == FMOD_OK && sound)
            {
                sounds[soundName] = sound;
            }
        }

        void FmodStudioManager::UnloadSound(const std::string& soundName)
        {
            auto it = sounds.find(soundName);
            if (it != sounds.end())
            {
                it->second->release();
                sounds.erase(it);
            }
        }

        void FmodStudioManager::SetListenerAttributes(int listener, const glm::vec3& pos, const glm::vec3& velocity)
        {
            FMOD_3D_ATTRIBUTES attributes = { {pos.x, pos.y, pos.z}, {velocity.x, velocity.y, velocity.z} };
            studioSystem->setListenerAttributes(listener, &attributes);
        }

        int FmodStudioManager::PlaySound(const std::string& soundName, const glm::vec3& pos, float volumedB)
        {
            auto it = sounds.find(soundName);
            if (it == sounds.end())
                return -1; // Sound not found

            FMOD::Channel* channel = nullptr;
            coreSystem->playSound(it->second, nullptr, true, &channel);
            if (channel)
            {
                FMOD_VECTOR position = { pos.x, pos.y, pos.z };
                channel->set3DAttributes(&position, nullptr);
                channel->setVolume(dBToVolume(volumedB));
                channel->setPaused(false);
                channels[nextChannelId] = channel;
                return nextChannelId++;
            }
            return -1;
        }

        void FmodStudioManager::PlayEvent(const std::string& eventName, bool isImmediate)
        {
            auto it = events.find(eventName);
            if (it == events.end())
                return; // Event not found

            it->second->start(); // Start the event

            if (isImmediate)
            {
                it->second->stop(FMOD_STUDIO_STOP_IMMEDIATE);
            }
        }

        void FmodStudioManager::StopEvent(const std::string& eventName, bool isImmediate)
        {
            auto it = events.find(eventName);
            if (it == events.end())
                return; // Event not found

            FMOD_STUDIO_STOP_MODE mode = isImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
            it->second->stop(mode);
        }

        void FmodStudioManager::StopAllChannels()
        {
            for (auto& channelPair : channels)
            {
                channelPair.second->stop();
            }
            channels.clear();
        }

        void FmodStudioManager::SetChannel3dPosition(int channelId, const glm::vec3& position)
        {
            auto it = channels.find(channelId);
            if (it == channels.end())
                return; // Channel not found

            FMOD_VECTOR pos = { position.x, position.y, position.z };
            it->second->set3DAttributes(&pos, nullptr);
        }

        void FmodStudioManager::SetChannelVolume(int channelId, float dB)
        {
            auto it = channels.find(channelId);
            if (it == channels.end())
                return; // Channel not found

            float volume = dBToVolume(dB);
            it->second->setVolume(volume);
        }

        bool FmodStudioManager::IsPlaying(int channelId)
        {
            auto it = channels.find(channelId);
            if (it == channels.end())
                return false; // Channel not found

            bool isPlaying = false;
            it->second->isPlaying(&isPlaying);
            return isPlaying;
        }

        bool FmodStudioManager::IsEventPlaying(const std::string& eventName)
        {
            auto it = events.find(eventName);
            if (it == events.end())
                return false; // Event not found

            FMOD_STUDIO_PLAYBACK_STATE state;
            it->second->getPlaybackState(&state);
            return state == FMOD_STUDIO_PLAYBACK_PLAYING;
        }

        // fade in fade out feature that jarran requested
        float FmodStudioManager::dBToVolume(float dB)
        {
            return std::pow(10.0f, dB / 20.0f);
        }

        float FmodStudioManager::VolumeTodB(float volume)
        {
            return 20.0f * std::log10(volume);
        }

    } // namespace Audio

} // namespace PE
