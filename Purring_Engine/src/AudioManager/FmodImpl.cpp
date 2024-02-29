#include "prpch.h"
#include <vector>
#include <fmod_errors.h>

#include "AudioManager/FmodImpl.h"
#include "Logging/Logger.h"

namespace PE
{
    namespace Audio
    {

        extern Logger g_AudioLogger;

        FmodImpl::FmodImpl()
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

        FmodImpl::~FmodImpl()
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

        void FmodImpl::OnUpdate(float dt)
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

    } // namespace Audio

} // namespace PE
