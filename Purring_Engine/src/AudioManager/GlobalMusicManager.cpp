#include "prpch.h"
#include "GlobalMusicManager.h"
#include "ResourceManager/ResourceManager.h"

namespace PE
{
    GlobalMusicManager::GlobalMusicManager() : m_isPaused(false) {}

    GlobalMusicManager::~GlobalMusicManager()
    {
        StopBackgroundMusic();
    }

    void GlobalMusicManager::PlayBackgroundMusic(const std::string& trackKey, bool loop)
    {
        StopBackgroundMusic();

        m_currentTrackKey = trackKey;
        auto audioComponent = GetOrCreateAudioComponent(trackKey);
        audioComponent->SetLoop(loop);
        audioComponent->PlayAudioSound();
    }

    void GlobalMusicManager::PauseBackgroundMusic()
    {
        auto it = m_audioComponents.find(m_currentTrackKey);
        if (it != m_audioComponents.end())
        {
            it->second->PauseSound();
            m_isPaused = true;
        }
    }

    void GlobalMusicManager::ResumeBackgroundMusic()
    {
        if (m_isPaused)
        {
            auto it = m_audioComponents.find(m_currentTrackKey);
            if (it != m_audioComponents.end())
            {
                it->second->ResumeSound();
                m_isPaused = false;
            }
        }
    }

    void GlobalMusicManager::StopBackgroundMusic()
    {
        auto it = m_audioComponents.find(m_currentTrackKey);
        if (it != m_audioComponents.end())
        {
            it->second->StopSound();
            m_audioComponents.erase(it);
        }
    }

    void GlobalMusicManager::SetBackgroundMusicVolume(float volume)
    {
        auto it = m_audioComponents.find(m_currentTrackKey);
        if (it != m_audioComponents.end())
        {
            it->second->SetVolume(volume);
        }
    }

    std::shared_ptr<AudioComponent> GlobalMusicManager::GetOrCreateAudioComponent(const std::string& trackKey)
    {
        auto it = m_audioComponents.find(trackKey);
        if (it == m_audioComponents.end())
        {
            auto audioComponent = std::make_shared<AudioComponent>();
            audioComponent->SetAudioKey(trackKey);
            it = m_audioComponents.emplace(trackKey, audioComponent).first;
        }
        return it->second;
    }

    void GlobalMusicManager::StoreCurrentState()
    {
        auto it = m_audioComponents.find(m_currentTrackKey);
        if (it != m_audioComponents.end() && it->second->IsPlaying())
        {
            unsigned int position;
            it->second->GetChannel()->getPosition(&position, FMOD_TIMEUNIT_MS);
            currentState = { m_currentTrackKey, position };
        }
    }


    void GlobalMusicManager::ResumeFromState(const AudioState& state)
    {
        PlayBackgroundMusic(state.trackKey, true);  //loop awlays

        auto it = m_audioComponents.find(state.trackKey);
        if (it != m_audioComponents.end()) {
            it->second->GetChannel()->setPosition(state.position, FMOD_TIMEUNIT_MS);
        }
    }


}
