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


        FMOD::Studio::Bank* FmodStudioManager::GetBank(const std::string& bankName) const {
            auto it = banks.find(bankName);
            return it != banks.end() ? it->second : nullptr;
        }

        void FmodStudioManager::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
            if (GetBank(bankName)) {
                return; // Bank already loaded
            }

            FMOD::Studio::Bank* bank;
            auto result = studioSystem->loadBankFile(bankName.c_str(), flags, &bank);
            if (result == FMOD_OK) {
                banks[bankName] = bank;
            }
            else {
                // Handle error
            }
        }

        void FmodStudioManager::LoadEvent(const std::string& eventName)
        {
            if (events.find(eventName) != events.end()) return; // Event already loaded

            FMOD::Studio::EventDescription* eventDescription = nullptr;
            FMOD_RESULT result = studioSystem->getEvent(eventName.c_str(), &eventDescription);
            if (result == FMOD_OK && eventDescription) {
                FMOD::Studio::EventInstance* eventInstance = nullptr;
                eventDescription->createInstance(&eventInstance);
                if (eventInstance) {
                    events[eventName] = eventInstance;
                }
            }
        }

    } // namespace Audio

} // namespace PE
