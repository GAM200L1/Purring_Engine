/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GameStateController_v2_0.cpp
 \date     15-01-2024

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Keep Tracks whether the game is paused or needs to be paused.
	Contains the function to unpause the game as well.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "GameStateController_v2_0.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "LogicSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/Text.h"
#include "GUISystem.h"

namespace PE
{
	GameStateController_v2_0::GameStateController_v2_0()
	{
		REGISTER_UI_FUNCTION(ResumeStateV2, PE::GameStateController_v2_0);
	}

	void GameStateController_v2_0::Init(EntityID id)
	{
		if (m_ScriptData[id].GameStateManagerActive)
		{
			currentState = GameStates_v2_0::SPLASHSCREEN;
			//make sure all canvas inactive except for splashscreen 
		}

		m_ScriptData[id].keyEventHandlerId = ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, GameStateController_v2_0::OnKeyEvent, this)
		m_ScriptData[id].outOfFocusEventHandlerId = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowLostFocus, GameStateController_v2_0::OnWindowOutOfFocus, this)

	}
	void GameStateController_v2_0::Update(EntityID id, float deltaTime)
	{

		if (PauseManager::GetInstance().IsPaused())
		{
			SetPauseState();
		}
		else
		{
			ResumeStateV2();
		}

		if (currentState == GameStates_v2_0::PAUSE)
		{
			ActiveObject(m_ScriptData[id].BackGroundCanvas);
			if (m_pauseMenuOpenOnce)
			{
				ActiveObject(m_ScriptData[id].PauseMenuCanvas);
				m_pauseMenuOpenOnce = false;
			}
			return;
		}
		else
		{
			DeactiveObject(m_ScriptData[id].BackGroundCanvas);
			DeactiveObject(m_ScriptData[id].PauseMenuCanvas);
		}

		if (currentState == GameStates_v2_0::SPLASHSCREEN)
		{
			m_ScriptData[id].SplashTimer -= deltaTime;
			if (m_ScriptData[id].SplashTimer <= 0)
			{
				DeactiveObject(m_ScriptData[id].SplashScreen);
				SetGameState(GameStates_v2_0::PLANNING);
			}
		}
	}
	void GameStateController_v2_0::Destroy(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
		{
			REMOVE_KEY_EVENT_LISTENER(m_ScriptData[id].keyEventHandlerId);
			REMOVE_WINDOW_EVENT_LISTENER(m_ScriptData[id].outOfFocusEventHandlerId);
		}

		currentState = GameStates_v2_0::INACTIVE;
		PauseManager::GetInstance().SetPaused(false);
	}
	void GameStateController_v2_0::OnAttach(EntityID id)
	{
		m_ScriptData[id] = GameStateController_v2_0Data();
	}
	void GameStateController_v2_0::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}
	std::map<EntityID, GameStateController_v2_0Data>& GameStateController_v2_0::GetScriptData()
	{
		return m_ScriptData;
	}

	rttr::instance GameStateController_v2_0::GetScriptData(EntityID id)
	{
		return rttr::instance(m_ScriptData.at(id));
	}

	void GameStateController_v2_0::OnWindowOutOfFocus(const PE::Event<PE::WindowEvents>& r_event)
	{
		if (currentState != GameStates_v2_0::INACTIVE && currentState != GameStates_v2_0::WIN && currentState != GameStates_v2_0::LOSE)
		{
			currentState = GameStates_v2_0::PAUSE;
		}
	}
	void GameStateController_v2_0::OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event)
	{
		PE::KeyTriggeredEvent KTE;

		//dynamic cast
		if (r_event.GetType() == PE::KeyEvents::KeyTriggered)
		{
			KTE = dynamic_cast<const PE::KeyTriggeredEvent&>(r_event);

			if (KTE.keycode == GLFW_KEY_ESCAPE)
			{
				if (currentState == GameStates_v2_0::WIN || currentState == GameStates_v2_0::LOSE)
				{
					return;
				}

				if (currentState == GameStates_v2_0::PAUSE)
				{
					ResumeStateV2();
				}
				else if (currentState != GameStates_v2_0::INACTIVE)
				{
					SetPauseState();
				}
			}

			if (KTE.keycode == GLFW_KEY_F1)
			{
				currentState = GameStates_v2_0::WIN;
			}

			if (KTE.keycode == GLFW_KEY_F2)
			{
				currentState = GameStates_v2_0::LOSE;
			}
		}
	}

	void GameStateController_v2_0::SetPauseState()
	{
		if (currentState != GameStates_v2_0::PAUSE)
		{
			prevState = currentState;
			currentState = GameStates_v2_0::PAUSE;

			PauseManager::GetInstance().SetPaused(true);
			m_pauseMenuOpenOnce = true;
		}
	}
	void GameStateController_v2_0::SetGameState(GameStates_v2_0 gameState)
	{
		if (currentState != gameState)
			prevState = currentState;
		currentState = gameState;

	}
	void GameStateController_v2_0::ResumeStateV2(EntityID)
	{
		if (currentState == GameStates_v2_0::PAUSE)
		{
			currentState = prevState;
			prevState = GameStates_v2_0::PAUSE;

			PauseManager::GetInstance().SetPaused(false);
		}
	}

	void GameStateController_v2_0::ActiveObject(EntityID id)
	{
		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;

		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = true;

		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
		{
			if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
				break;

			EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = true;
		}
	}

	void GameStateController_v2_0::DeactiveObject(EntityID id)
	{
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
		{
			if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
				break;

			EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
		}

		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;

		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = false;
	}


}
