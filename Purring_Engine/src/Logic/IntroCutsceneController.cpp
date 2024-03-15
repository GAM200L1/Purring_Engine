/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     IntroCutsceneController.cpp 
 \date     7-2-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu


 \brief  This file contains the declarations of IntroCutsceneController that controls
		 the cutscene for the introduction of the game.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/

#include "prpch.h"
#include "Data/SerializationManager.h"
#include "IntroCutsceneController.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "SceneManager/SceneManager.h"
#include "PauseManager.h"
#include "AudioManager/GlobalMusicManager.h"
#include "ResourceManager/ResourceManager.h"

#include <limits>


# define M_PI           3.14159265358979323846 

namespace PE 
{
	void IntroCutsceneController::Init(EntityID id)
	{
		m_elapsedTime = 0;
		m_endCutscene = false;
		m_startCutscene = true;

		m_scriptData[id].windowNotFocusEventID = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowLostFocus, IntroCutsceneController::OnWindowOutOfFocus, this)
		m_scriptData[id].windowFocusEventID = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowFocus, IntroCutsceneController::OnWindowFocus, this)
	}

	void IntroCutsceneController::Update(EntityID id, float deltaTime)
	{
		GlobalMusicManager::GetInstance().Update(deltaTime);

		if (PauseManager::GetInstance().IsPaused())
		{
			EntityID cutsceneSounds = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Intro Cutscene Music.prefab");
			if (EntityManager::GetInstance().Has<EntityDescriptor>(cutsceneSounds))
				EntityManager::GetInstance().Get<AudioComponent>(cutsceneSounds).PauseSound();
			EntityManager::GetInstance().RemoveEntity(cutsceneSounds);

			//m_startCutscene = true;
		}
		else
		{
			m_elapsedTime += deltaTime;

			if (!m_startCutscene)
			{
				EntityID cutsceneSounds = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Intro Cutscene Music.prefab");
				if (EntityManager::GetInstance().Has<EntityDescriptor>(cutsceneSounds))
					EntityManager::GetInstance().Get<AudioComponent>(cutsceneSounds).ResumeSound();
				EntityManager::GetInstance().RemoveEntity(cutsceneSounds);
			}

			// Start a fade-out effect 3 seconds before the cutscene ends
			if (m_elapsedTime >= (m_sceneTimer - 3.0f) && !m_isFadingOut)
			{
				PE::GlobalMusicManager::GetInstance().StartFadeOut(3.0f);
				m_isFadingOut = true;
			}

			if (m_elapsedTime >= m_sceneTimer && !m_endCutscene)
			{
				//if (EntityManager::GetInstance().Has<TextComponent>(m_scriptData[id].Text))
				//	EntityManager::GetInstance().Get<TextComponent>(m_scriptData[id].Text).SetText("Continue");

				m_endCutscene = true;
			}

			if (m_isTransitioning)
				TransitionPanelFade(id, deltaTime);
		}

	}
	void IntroCutsceneController::Destroy(EntityID id)
	{
		EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Intro Cutscene Music.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
			EntityManager::GetInstance().Get<AudioComponent>(bgm).StopSound();
		EntityManager::GetInstance().RemoveEntity(bgm);

		m_elapsedTime = 0;
		m_endCutscene = false;
		m_startCutscene = true;
		m_transitionElapsedTime = 0;
		m_isTransitioning = false;
		m_isFadingOut = false;
	}

	void IntroCutsceneController::OnAttach(EntityID id)
	{
		m_scriptData[id] = IntroCutsceneControllerData();
	}

	void IntroCutsceneController::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			REMOVE_WINDOW_EVENT_LISTENER(m_scriptData[id].windowNotFocusEventID);
			REMOVE_WINDOW_EVENT_LISTENER(m_scriptData[id].windowFocusEventID);
		}

		m_scriptData.erase(id);
	}

	std::map<EntityID, IntroCutsceneControllerData>& IntroCutsceneController::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance IntroCutsceneController::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

	void IntroCutsceneController::ContinueToLevel(EntityID id)
	{
		m_isTransitioning = true;
		PlayClickAudio();
	}

	void IntroCutsceneController::PlayClickAudio()
	{
		EntityID buttonpress = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Button Click SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}

	void IntroCutsceneController::PlaySceneTransitionAudio()
	{
		EntityID sound = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Scene Transition SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	IntroCutsceneController::IntroCutsceneController()
	{
		REGISTER_UI_FUNCTION(ContinueToLevel, PE::IntroCutsceneController);
		REGISTERANIMATIONFUNCTION(SetButtonText, PE::IntroCutsceneController);
		REGISTERANIMATIONFUNCTION(StartCutscene, PE::IntroCutsceneController);
	}

	void IntroCutsceneController::TransitionPanelFade(EntityID const id, float deltaTime)
	{
		m_transitionElapsedTime += deltaTime;

		float fadeInSpeed = std::clamp(m_transitionElapsedTime / m_transitionTimer, 0.0f, 1.0f);

		FadeAllObject(m_scriptData[id].TransitionScreen, fadeInSpeed);

		if (fadeInSpeed >= 1)
		{
			PlaySceneTransitionAudio();
			SceneManager::GetInstance().LoadScene("Level1Scene.scene");
		}
	}

	void IntroCutsceneController::FadeAllObject(EntityID id, float const alpha)
	{
		//fade all the children objects first
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
		{
			if (EntityManager::GetInstance().Has<TextComponent>(id2))
			{
				EntityManager::GetInstance().Get<TextComponent>(id2).SetAlpha(alpha);
			}
			else if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id2))
			{
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id2).SetAlpha(alpha);
			}
		}

		if (!EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id))
			return;

		//fade the current object
		EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id).SetAlpha(alpha);
	}

	void IntroCutsceneController::SetButtonText(EntityID id)
	{
		if (EntityManager::GetInstance().Has<TextComponent>(m_scriptData[id].Text))
			EntityManager::GetInstance().Get<TextComponent>(m_scriptData[id].Text).SetText("Continue");
	}

	void IntroCutsceneController::StartCutscene(EntityID id)
	{
		PE::GlobalMusicManager::GetInstance().PlayBGM("AudioObject/Intro Cutscene Music.prefab", false, 5.0f);

		m_startCutscene = false;
	}

	void IntroCutsceneController::OnWindowOutOfFocus(const PE::Event<PE::WindowEvents>& r_event)
	{
		PauseManager::GetInstance().SetPaused(true);
	}

	void IntroCutsceneController::OnWindowFocus(const PE::Event<PE::WindowEvents>& r_event)
	{
		PauseManager::GetInstance().SetPaused(false);
	}
}