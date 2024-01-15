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
			m_currentState = GameStates_v2_0::SPLASHSCREEN;
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

		if (m_currentState == GameStates_v2_0::PAUSE)
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].PauseMenuCanvas).isActive = true;
			return;
		}
		else
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].PauseMenuCanvas).isActive = false;
		}

		if (m_currentState == GameStates_v2_0::SPLASHSCREEN)
		{
			m_ScriptData[id].SplashTimer -= deltaTime;
			if (m_ScriptData[id].SplashTimer <= 0)
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].SplashScreen).isActive = false;
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

		m_currentState = GameStates_v2_0::INACTIVE;
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
		if (m_currentState != GameStates_v2_0::INACTIVE && m_currentState != GameStates_v2_0::WIN && m_currentState != GameStates_v2_0::LOSE)
		{
			m_currentState = GameStates_v2_0::PAUSE;
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
				if (m_currentState == GameStates_v2_0::WIN || m_currentState == GameStates_v2_0::LOSE)
				{
					return;
				}

				if (m_currentState == GameStates_v2_0::PAUSE)
				{
					ResumeStateV2();
				}
				else if (m_currentState != GameStates_v2_0::INACTIVE)
				{
					SetPauseState();
				}
			}

			if (KTE.keycode == GLFW_KEY_F1)
			{
				m_currentState = GameStates_v2_0::WIN;
			}

			if (KTE.keycode == GLFW_KEY_F2)
			{
				m_currentState = GameStates_v2_0::LOSE;
			}
		}
	}

	void GameStateController_v2_0::SetPauseState()
	{
		if (m_currentState != GameStates_v2_0::PAUSE)
		{
			m_prevState = m_currentState;
			m_currentState = GameStates_v2_0::PAUSE;

			PauseManager::GetInstance().SetPaused(true);
		}
	}
	void GameStateController_v2_0::SetGameState(GameStates_v2_0 gameState)
	{
		if (m_currentState != gameState)
			m_prevState = m_currentState;
		m_currentState = gameState;

	}
	void GameStateController_v2_0::ResumeStateV2(EntityID)
	{
		if (m_currentState == GameStates_v2_0::PAUSE)
		{
			m_currentState = m_prevState;
			m_prevState = GameStates_v2_0::PAUSE;

			PauseManager::GetInstance().SetPaused(false);
		}
	}
}
