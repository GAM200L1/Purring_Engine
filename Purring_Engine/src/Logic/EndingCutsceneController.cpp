/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     EndingCutsceneController.cpp
 \date     3-14-2024

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu
 \par	   code %      80%

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan\@digipen.edu
 \par	   code %      20%

 \brief  This file contains the definition of IntroCutsceneController that controls
		 the cutscene for the ending of the game.

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "Data/SerializationManager.h"
#include "EndingCutsceneController.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "SceneManager/SceneManager.h"
#include "PauseManager.h"
#include "AudioManager/GlobalMusicManager.h"
#include "ResourceManager/ResourceManager.h"
#include "Logic/GameStateController_v2_0.h"
#include "Logic/LogicSystem.h"
#include "Logic/MainMenuController.h"
#include "GUISystem.h"
#include <limits>


# define M_PI           3.14159265358979323846 

namespace PE 
{
	void EndingCutsceneController::Init(EntityID id)
	{
		m_elapsedTime = 0;

		if (EntityManager::GetInstance().Has<AnimationComponent>(id))
			m_sceneTimer = EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimation()->GetAnimationDuration();

		m_endCutscene = false;
		m_startCutscene = true;

		m_scriptData[id].windowNotFocusEventID = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowLostFocus, EndingCutsceneController::OnWindowOutOfFocus, this)
		m_scriptData[id].windowFocusEventID = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowFocus, EndingCutsceneController::OnWindowFocus, this)
	}

	void EndingCutsceneController::Update(EntityID id, float deltaTime)
	{
		GlobalMusicManager::GetInstance().Update(deltaTime);

		if (PauseManager::GetInstance().IsPaused())
		{
			EntityID cutsceneSounds = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Outro Cutscene Music.prefab");
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
				EntityID cutsceneSounds = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Outro Cutscene Music.prefab");
				if (EntityManager::GetInstance().Has<EntityDescriptor>(cutsceneSounds))
					EntityManager::GetInstance().Get<AudioComponent>(cutsceneSounds).ResumeSound();
				EntityManager::GetInstance().RemoveEntity(cutsceneSounds);
			}

			//// Start a fade-out effect 3 seconds before the cutscene ends
			//if (m_elapsedTime >= (m_sceneTimer - 3.0f) && !m_isFadingOut)
			//{
			//	PE::GlobalMusicManager::GetInstance().StartFadeOut(3.0f);
			//	m_isFadingOut = true;
			//}

			if (m_elapsedTime >= m_sceneTimer && !m_endCutscene)
			{
				m_endCutscene = true;
			}

			if (m_isTransitioning)
				TransitionPanelFade(id, deltaTime);
		}

	}
	void EndingCutsceneController::Destroy(EntityID id)
	{
		EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Outro Cutscene Music.prefab");
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

	void EndingCutsceneController::OnAttach(EntityID id)
	{
		m_scriptData[id] = EndingCutsceneControllerData();

		//set the current ID
		 m_currentCutsceneObject = id;
	}

	void EndingCutsceneController::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			REMOVE_WINDOW_EVENT_LISTENER(m_scriptData[id].windowNotFocusEventID);
			REMOVE_WINDOW_EVENT_LISTENER(m_scriptData[id].windowFocusEventID);
		}

		m_scriptData.erase(id);
	}

	std::map<EntityID, EndingCutsceneControllerData>& EndingCutsceneController::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance EndingCutsceneController::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

	void EndingCutsceneController::ContinueToLevel(EntityID id)
	{
		m_isTransitioning = true;
		PlayClickAudio();
	}

	void EndingCutsceneController::PlayClickAudio()
	{
		EntityID buttonpress = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Button Click SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}

	void EndingCutsceneController::PlayWinAudio()
	{
		EntityID sound = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Game Win SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void EndingCutsceneController::PlaySceneTransitionAudio()
	{
		EntityID sound = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Scene Transition SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	EndingCutsceneController::EndingCutsceneController()
	{
		REGISTERANIMATIONFUNCTION(SetButtonText, PE::EndingCutsceneController);
		REGISTERANIMATIONFUNCTION(StartCutscene, PE::EndingCutsceneController);

		REGISTER_UI_FUNCTION(ECReturnToMainMenu, PE::EndingCutsceneController);
		REGISTER_UI_FUNCTION(ECAreYouSure, PE::EndingCutsceneController);
		REGISTER_UI_FUNCTION(ECReturnFromAreYouSure, PE::EndingCutsceneController);
		REGISTER_UI_FUNCTION(WinScreen, PE::EndingCutsceneController);
	}

	void EndingCutsceneController::TransitionPanelFade(EntityID const id, float deltaTime)
	{
		m_transitionElapsedTime += deltaTime;

		float fadeInSpeed = std::clamp(m_transitionElapsedTime / m_transitionTimer, 0.0f, 1.0f);

		FadeAllObject(m_scriptData[id].TransitionScreen, fadeInSpeed);

		if (fadeInSpeed >= 1)
		{
			PlaySceneTransitionAudio();
			SceneManager::GetInstance().LoadScene("MainMenu.scene");
		}
	}

	void EndingCutsceneController::FadeAllObject(EntityID id, float const alpha)
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

	void EndingCutsceneController::SetButtonText(EntityID id)
	{
		//if (EntityManager::GetInstance().Has<TextComponent>(m_scriptData[id].Text))
		//	EntityManager::GetInstance().Get<TextComponent>(m_scriptData[id].Text).SetText("Next");

		if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData[id].ContinueButton))
			EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].ContinueButton).isActive = true;

		if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData[id].SkipButton))
			EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].SkipButton).isActive = false;
	}

	void EndingCutsceneController::StartCutscene(EntityID id)
	{
		PE::GlobalMusicManager::GetInstance().PlayBGM("AudioObject/Outro Cutscene Music.prefab", false, 5.0f);

		m_startCutscene = false;
	}

	void EndingCutsceneController::OnWindowOutOfFocus(const PE::Event<PE::WindowEvents>& r_event)
	{
		PauseManager::GetInstance().SetPaused(true);
	}

	void EndingCutsceneController::ActiveObject(EntityID id)
	{
		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;

		//set active the current object
		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = true;

		//set active the childrens if there are any
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
		{
			if (EntityManager::GetInstance().Has<GUIButton>(id2))
			{
				EntityManager::GetInstance().Get<GUIButton>(id2).disabled = false;
			}

			//set active the children of the childrens, up to only 2nd child level
			for (auto id3 : EntityManager::GetInstance().Get<EntityDescriptor>(id2).children)
			{
				if (EntityManager::GetInstance().Has<GUIButton>(id3))
				{
					EntityManager::GetInstance().Get<GUIButton>(id3).disabled = false;
				}

				if (!EntityManager::GetInstance().Has<EntityDescriptor>(id3))
					break;

				EntityManager::GetInstance().Get<EntityDescriptor>(id3).isActive = true;
			}
			if (!EntityManager::GetInstance().Has<EntityDescriptor>(id2))
				break;

			EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = true;
		}
	}

	void EndingCutsceneController::DeactiveObject(EntityID id)
	{
		//deactive all the children objects first
		if (EntityManager::GetInstance().Has<EntityDescriptor>(id))
			for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
			{
				if (!EntityManager::GetInstance().Has<EntityDescriptor>(id2))
					break;

				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
			}

		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;

		//deactive current object
		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = false;
	}

	
	void EndingCutsceneController::OnWindowFocus(const PE::Event<PE::WindowEvents>& r_event)
	{
		PauseManager::GetInstance().SetPaused(false);
	}

	void EndingCutsceneController::WinScreen(EntityID)
	{
		GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->currentState = GameStates_v2_0::WIN;

		EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Outro Cutscene Music.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
			EntityManager::GetInstance().Get<AudioComponent>(bgm).StopSound();
		EntityManager::GetInstance().RemoveEntity(bgm);

		PlayWinAudio();

		ActiveObject(m_scriptData[m_currentCutsceneObject].WinCanvas);
		ActiveObject(m_scriptData[m_currentCutsceneObject].BackgroundCanvas);

		if (EntityManager::GetInstance().Has<TextComponent>(m_scriptData[m_currentCutsceneObject].DeathCounter))
			EntityManager::GetInstance().Get<TextComponent>(m_scriptData[m_currentCutsceneObject].DeathCounter).SetText(std::to_string(GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetDeathCount()));
		
		if (EntityManager::GetInstance().Has<TextComponent>(m_scriptData[m_currentCutsceneObject].RescueCounter))
			EntityManager::GetInstance().Get<TextComponent>(m_scriptData[m_currentCutsceneObject].RescueCounter).SetText(std::to_string(GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetCatRescued()));
		
		if (EntityManager::GetInstance().Has<TextComponent>(m_scriptData[m_currentCutsceneObject].TurnCounter))
			EntityManager::GetInstance().Get<TextComponent>(m_scriptData[m_currentCutsceneObject].TurnCounter).SetText(std::to_string(GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetTurnCount()));
		
		if (EntityManager::GetInstance().Has<TextComponent>(m_scriptData[m_currentCutsceneObject].KillCounter))
			EntityManager::GetInstance().Get<TextComponent>(m_scriptData[m_currentCutsceneObject].KillCounter).SetText(std::to_string(GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetRatsKilled()));

	}

	void EndingCutsceneController::ECReturnToMainMenu(EntityID)
	{
		m_isTransitioning = true;
		GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->ResetStats();


		GETSCRIPTINSTANCEPOINTER(MainMenuController)->NotFirstStart();
		PlayClickAudio();
	}

	void EndingCutsceneController::ECAreYouSure(EntityID)
	{
		DeactiveObject(m_scriptData[m_currentCutsceneObject].WinCanvas);
		ActiveObject(m_scriptData[m_currentCutsceneObject].AreYouSureCanvas);
	}

	void EndingCutsceneController::ECReturnFromAreYouSure(EntityID)
	{
		DeactiveObject(m_scriptData[m_currentCutsceneObject].AreYouSureCanvas);
		ActiveObject(m_scriptData[m_currentCutsceneObject].WinCanvas);
	}
}