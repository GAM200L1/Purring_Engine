/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GlobalMusicManager.cpp
 \date     2-03-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    Control global music.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
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

    void GlobalMusicManager::Update(float deltaTime)
    {
        if (m_isFading)
        {
            m_fadeProgress += deltaTime / m_fadeDuration;

            if (m_fadeProgress >= 1.0f)
            {
                m_fadeProgress = 1.0f;
                m_isFading = false;
            }

            float volume = m_isFadingIn ? m_fadeProgress : (1.0f - m_fadeProgress);

            SetBackgroundMusicVolume(volume);
        }

    }

    void GlobalMusicManager::PlayBGM(const std::string& prefabPath, bool loop)
    {
        EntityID audioEntity = m_serializationManager.LoadFromFile(prefabPath);
        if (EntityManager::GetInstance().Has<AudioComponent>(audioEntity))
        {
            auto audioComponent = EntityManager::GetInstance().Get<AudioComponent>(audioEntity);
            audioComponent.SetLoop(loop);
            audioComponent.PlayAudioSound(AudioComponent::AudioType::BGM);
            m_audioComponents[prefabPath] = std::make_shared<AudioComponent>(audioComponent);
        }
        EntityManager::GetInstance().RemoveEntity(audioEntity);
    }

    void GlobalMusicManager::PlaySFX(const std::string& prefabPath, bool loop)
    {
        EntityID audioEntity = m_serializationManager.LoadFromFile(prefabPath);
        if (EntityManager::GetInstance().Has<AudioComponent>(audioEntity))
        {
            auto audioComponent = EntityManager::GetInstance().Get<AudioComponent>(audioEntity);
            audioComponent.SetLoop(loop);
            audioComponent.PlayAudioSound(AudioComponent::AudioType::SFX);
            m_audioComponents[prefabPath] = std::make_shared<AudioComponent>(audioComponent);
        }
        EntityManager::GetInstance().RemoveEntity(audioEntity);
    }


    void GlobalMusicManager::PauseBackgroundMusic()
    {
        for (auto& [key, audioComponent] : m_audioComponents)
        {
            float currentVolume = audioComponent->GetVolume();
            m_originalVolumes[key] = currentVolume;  // Store the original volume before changing it
            audioComponent->SetVolume(currentVolume * 0.2f);  // Reduce volume to 20%
        }
    }

    void GlobalMusicManager::ResumeBackgroundMusic()
    {
        // Assuming m_originalVolumes is a std::map<std::string, float> storing original volumes
        for (auto& [key, audioComponent] : m_audioComponents)
        {
            float originalVolume = m_originalVolumes[key];  // Retrieve the original volume
            audioComponent->SetVolume(originalVolume);  // Restore the original volume
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
            m_currentState = { m_currentTrackKey, position };
        }
    }


    void GlobalMusicManager::ResumeFromState(const AudioState& state)
    {
        //PlayBackgroundMusic(state.trackKey, true);  //loop awlays

        auto it = m_audioComponents.find(state.trackKey);
        if (it != m_audioComponents.end()) {
            it->second->GetChannel()->setPosition(state.position, FMOD_TIMEUNIT_MS);
        }
    }

    void GlobalMusicManager::StartFadeIn(float duration)
    {
        m_isFading = true;
        m_isFadingIn = true;
        m_fadeProgress = 0.0f;
        m_fadeDuration = duration;
    }

    void GlobalMusicManager::StartFadeOut(float duration)
    {
        m_isFading = true;
        m_isFadingIn = false;
        m_fadeProgress = 0.0f;
        m_fadeDuration = duration;
    }



}