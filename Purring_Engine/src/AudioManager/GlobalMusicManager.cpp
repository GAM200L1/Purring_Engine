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
    GlobalMusicManager::GlobalMusicManager() : m_isPaused(false), m_maxVolume(1.0f) {}
    GlobalMusicManager::~GlobalMusicManager()
    {
        StopBackgroundMusic();
    }

    void GlobalMusicManager::Update(float deltaTime)
    {
        for (auto& [key, audioComponent] : m_audioComponents)
        {
            audioComponent->UpdateIndividualFade(deltaTime);
        }

        if (m_isFading)
        {
            m_fadeProgress += deltaTime / m_fadeDuration;
            m_fadeProgress = std::min(m_fadeProgress, 1.0f); // Ensure progress does not exceed 1

            // Calculate volume based on fade progress and max volume
            float volume = (m_isFadingIn ? m_fadeProgress : (1.0f - m_fadeProgress)) * m_maxVolume;
            SetBackgroundMusicVolume(volume);

            // Stop fading if the progress has completed
            if (m_fadeProgress >= 1.0f)
            {
                m_isFading = false;
            }
        }

    }

    void GlobalMusicManager::PlayBGM(const std::string& r_prefabPath, bool loop, float fadeInDuration)
    {
        EntityID audioEntity = ResourceManager::GetInstance().LoadPrefabFromFile(r_prefabPath);
        if (EntityManager::GetInstance().Has<AudioComponent>(audioEntity))
        {
            auto& audioComponent = EntityManager::GetInstance().Get<AudioComponent>(audioEntity);
            audioComponent.SetLoop(loop);

            if (fadeInDuration > 0.0f)
            {
                audioComponent.StartIndividualFadeIn(fadeInDuration);
            }
            else
            {
                audioComponent.SetVolume(m_maxVolume);  // Set to max volume if no fade-in
            }

            audioComponent.PlayAudioSound(AudioComponent::AudioType::BGM);
            m_audioComponents[r_prefabPath] = std::make_shared<AudioComponent>(audioComponent);
        }
        EntityManager::GetInstance().RemoveEntity(audioEntity);
    }

    void GlobalMusicManager::PlaySFX(const std::string& r_prefabPath, bool loop)
    {
        EntityID audioEntity = ResourceManager::GetInstance().LoadPrefabFromFile(r_prefabPath);
        if (EntityManager::GetInstance().Has<AudioComponent>(audioEntity))
        {
            auto audioComponent = EntityManager::GetInstance().Get<AudioComponent>(audioEntity);
            audioComponent.SetLoop(loop);
            audioComponent.PlayAudioSound(AudioComponent::AudioType::SFX);
            m_audioComponents[r_prefabPath] = std::make_shared<AudioComponent>(audioComponent);
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
            audioComponent->SetVolume(originalVolume);      // Restore the original volume
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

    std::shared_ptr<AudioComponent> GlobalMusicManager::GetOrCreateAudioComponent(const std::string& r_trackKey)
    {
        auto it = m_audioComponents.find(r_trackKey);
        if (it == m_audioComponents.end())
        {
            auto audioComponent = std::make_shared<AudioComponent>();
            audioComponent->SetAudioKey(r_trackKey);
            it = m_audioComponents.emplace(r_trackKey, audioComponent).first;
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

    void GlobalMusicManager::StopAllAudio()
    {
        for (auto& [key, audioComponent] : m_audioComponents)
        {
            if (audioComponent)
            {
                audioComponent->StopSound();
            }
        }

        // Clear the map after stopping all sounds
        m_audioComponents.clear();
    }

}