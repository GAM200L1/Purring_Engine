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
		m_elapsedTime += deltaTime;

		if (PauseManager::GetInstance().IsPaused())
		{
			EntityID cutsceneSounds = m_serializationManager.LoadFromFile("AudioObject/Intro Cutscene Music.prefab");
			if (EntityManager::GetInstance().Has<EntityDescriptor>(cutsceneSounds))
				EntityManager::GetInstance().Get<AudioComponent>(cutsceneSounds).PauseSound();
			EntityManager::GetInstance().RemoveEntity(cutsceneSounds);

			EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Menu Background Music.prefab");
			if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
				EntityManager::GetInstance().Get<AudioComponent>(bgm).PauseSound();
			EntityManager::GetInstance().RemoveEntity(bgm);

			m_startCutscene = true;
		}
		else
		{

			if (m_startCutscene)
			{
				EntityID cutsceneSounds = m_serializationManager.LoadFromFile("AudioObject/Intro Cutscene Music.prefab");
				if (EntityManager::GetInstance().Has<EntityDescriptor>(cutsceneSounds))
					EntityManager::GetInstance().Get<AudioComponent>(cutsceneSounds).PlayAudioSound();
				EntityManager::GetInstance().RemoveEntity(cutsceneSounds);

				EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Menu Background Music.prefab");
				if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
					EntityManager::GetInstance().Get<AudioComponent>(bgm).PlayAudioSound();
				EntityManager::GetInstance().RemoveEntity(bgm);
				m_startCutscene = false;
			}

			if (m_elapsedTime >= m_sceneTimer && !m_endCutscene)
			{
				EntityID cutsceneSounds = m_serializationManager.LoadFromFile("AudioObject/Intro Cutscene Music.prefab");
				if (EntityManager::GetInstance().Has<EntityDescriptor>(cutsceneSounds))
					EntityManager::GetInstance().Get<AudioComponent>(cutsceneSounds).StopSound();
				EntityManager::GetInstance().RemoveEntity(cutsceneSounds);

				EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Menu Background Music.prefab");
				if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
					EntityManager::GetInstance().Get<AudioComponent>(bgm).StopSound();
				EntityManager::GetInstance().RemoveEntity(bgm);

				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData[id].CutsceneObject))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].CutsceneObject).isActive = false;

				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData[id].FinalScene))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].FinalScene).isActive = true;

				if (EntityManager::GetInstance().Has<TextComponent>(m_scriptData[id].Text))
					EntityManager::GetInstance().Get<TextComponent>(m_scriptData[id].Text).SetText("Continue");

				m_endCutscene = true;
			}

			if (m_isTransitioning)
				TransitionPanelFade(id, deltaTime);
		}

	}
	void IntroCutsceneController::Destroy(EntityID id)
	{
		EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Intro Cutscene Music.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
			EntityManager::GetInstance().Get<AudioComponent>(bgm).StopSound();
		EntityManager::GetInstance().RemoveEntity(bgm);

		m_elapsedTime = 0;
		m_endCutscene = false;
		m_startCutscene = true;
		m_transitionElapsedTime = 0;
		m_isTransitioning = false;
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
		EntityID buttonpress = m_serializationManager.LoadFromFile("AudioObject/Button Click SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}

	void IntroCutsceneController::PlaySceneTransitionAudio()
	{
		EntityID sound = m_serializationManager.LoadFromFile("AudioObject/Scene Transition SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	IntroCutsceneController::IntroCutsceneController()
	{
		REGISTER_UI_FUNCTION(ContinueToLevel, PE::IntroCutsceneController);
	}

	void IntroCutsceneController::TransitionPanelFade(EntityID const id, float deltaTime)
	{
		m_transitionElapsedTime += deltaTime;

		float fadeInSpeed = std::clamp(m_transitionElapsedTime / m_transitionTimer, 0.0f, 1.0f);

		FadeAllObject(m_scriptData[id].TransitionScreen, fadeInSpeed);

		if (fadeInSpeed >= 1)
		{
			PlaySceneTransitionAudio();
			SceneManager::GetInstance().LoadScene("Level1Scene.json");
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

	void IntroCutsceneController::OnWindowOutOfFocus(const PE::Event<PE::WindowEvents>& r_event)
	{
		PauseManager::GetInstance().SetPaused(true);
	}

	void IntroCutsceneController::OnWindowFocus(const PE::Event<PE::WindowEvents>& r_event)
	{
		PauseManager::GetInstance().SetPaused(false);
	}
}