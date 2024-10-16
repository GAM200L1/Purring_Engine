/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FmodStudioManager.cpp
 \date     2-03-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    FMOD Studio FmodStudioManager setup.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
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

        //void FmodStudioManager::OnUpdate(float dt)
        //{
        //    std::vector<ChannelMap::iterator> stoppedChannels;
        //    for (auto it = channels.begin(), itEnd = channels.end(); it != itEnd; ++it)
        //    {
        //        bool isPlaying = false;
        //        it->second->isPlaying(&isPlaying);
        //        if (!isPlaying)
        //        {
        //            stoppedChannels.push_back(it);
        //        }
        //    }

        //    for (auto& it : stoppedChannels)
        //    {
        //        channels.erase(it);
        //    }
        //    studioSystem->update();
        //    g_AudioLogger.AddLog(false, "FMOD Studio system updated successfully.", __FUNCTION__);
        //}


        FMOD::Studio::Bank* FmodStudioManager::GetBank(const std::string& r_bankName) const
        {
            auto it = banks.find(r_bankName);
            return it != banks.end() ? it->second : nullptr;
        }

        void FmodStudioManager::LoadBank(const std::string& r_bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
        {
            if (GetBank(r_bankName))
            {
                return; // Bank already loaded
            }

            FMOD::Studio::Bank* p_bank;
            auto result = studioSystem->loadBankFile(r_bankName.c_str(), flags, &p_bank);
            if (result == FMOD_OK)
            {
                banks[r_bankName] = p_bank;
            }
            else
            {
                // Handle error
            }
        }

        void FmodStudioManager::LoadEvent(const std::string& r_eventName)
        {
            if (events.find(r_eventName) != events.end())
                return; // Event already loaded

            FMOD::Studio::EventDescription* p_eventDescription = nullptr;
            FMOD_RESULT result = studioSystem->getEvent(r_eventName.c_str(), &p_eventDescription);
            if (result == FMOD_OK && p_eventDescription)
            {
                FMOD::Studio::EventInstance* p_eventInstance = nullptr;
                p_eventDescription->createInstance(&p_eventInstance);
                if (p_eventInstance)
                {
                    events[r_eventName] = p_eventInstance;
                }
            }
        }

        void FmodStudioManager::LoadSound(const std::string& r_soundName, bool is3d, bool isLooping, bool isStream)
        {
            if (sounds.find(r_soundName) != sounds.end())
                return; // Sound already loaded

            FMOD_MODE mode = FMOD_DEFAULT;
            mode |= is3d ? FMOD_3D : FMOD_2D;
            mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
            mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

            FMOD::Sound* p_sound = nullptr;
            FMOD_RESULT result = coreSystem->createSound(r_soundName.c_str(), mode, nullptr, &p_sound);
            if (result == FMOD_OK && p_sound)
            {
                sounds[r_soundName] = p_sound;
            }
        }

        void FmodStudioManager::UnloadSound(const std::string& r_soundName)
        {
            auto it = sounds.find(r_soundName);
            if (it != sounds.end())
            {
                it->second->release();
                sounds.erase(it);
            }
        }

        void FmodStudioManager::SetListenerAttributes(int listener, const glm::vec3& r_pos, const glm::vec3& velocity)
        {
            FMOD_3D_ATTRIBUTES attributes = { {r_pos.x, r_pos.y, r_pos.z}, {velocity.x, velocity.y, velocity.z} };
            studioSystem->setListenerAttributes(listener, &attributes);
        }

        int FmodStudioManager::PlaySound(const std::string& r_soundName, const glm::vec3& r_pos, float volumedB)
        {
            auto it = sounds.find(r_soundName);
            if (it == sounds.end())
                return -1; // Sound not found

            FMOD::Channel* p_channel = nullptr;
            coreSystem->playSound(it->second, nullptr, true, &p_channel);
            if (p_channel)
            {
                FMOD_VECTOR position = { r_pos.x, r_pos.y, r_pos.z };
                p_channel->set3DAttributes(&position, nullptr);
                p_channel->setVolume(dBToVolume(volumedB));
                p_channel->setPaused(false);
                channels[nextChannelId] = p_channel;
                return nextChannelId++;
            }
            return -1;
        }

        void FmodStudioManager::PlayEvent(const std::string& r_eventName, bool isImmediate)
        {
            auto it = events.find(r_eventName);
            if (it == events.end())
                return; // Event not found

            it->second->start(); // Start the event

            if (isImmediate)
            {
                it->second->stop(FMOD_STUDIO_STOP_IMMEDIATE);
            }
        }

        void FmodStudioManager::StopEvent(const std::string& r_eventName, bool isImmediate)
        {
            auto it = events.find(r_eventName);
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

        void FmodStudioManager::SetChannel3dPosition(int channelId, const glm::vec3& r_position)
        {
            auto it = channels.find(channelId);
            if (it == channels.end())
                return; // Channel not found

            FMOD_VECTOR pos = { r_position.x, r_position.y, r_position.z };
            it->second->set3DAttributes(&pos, nullptr);
        }

        void FmodStudioManager::SetChannelVolume(int channelId, float r_dB)
        {
            auto it = channels.find(channelId);
            if (it == channels.end())
                return; // Channel not found

            float volume = dBToVolume(r_dB);
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

        bool FmodStudioManager::IsEventPlaying(const std::string& r_eventName)
        {
            auto it = events.find(r_eventName);
            if (it == events.end())
                return false; // Event not found

            FMOD_STUDIO_PLAYBACK_STATE state;
            it->second->getPlaybackState(&state);
            return state == FMOD_STUDIO_PLAYBACK_PLAYING;
        }

        // fade in fade out feature that jarran requested
        float FmodStudioManager::dBToVolume(float r_dB)
        {
            return std::pow(10.0f, r_dB / 20.0f);
        }

        float FmodStudioManager::VolumeTodB(float volume)
        {
            return 20.0f * std::log10(volume);
        }

    } // namespace Audio

} // namespace PE
