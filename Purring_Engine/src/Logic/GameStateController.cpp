/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     GameStateController.cpp 
 \date     03-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu


 \brief  This file contains the definitions of testScript

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/

#include "prpch.h"
#include "GameStateController.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "GameStateManager.h"
#include "Graphics/Renderer.h"
# define M_PI           3.14159265358979323846 

namespace PE 
{
	void GameStateController::Init(EntityID id)
	{
		//m_ScriptData[id].SplashScreen = PE::EntityManager::GetInstance().NewEntity();
		if (m_ScriptData[id].GameStateManagerActive)
		{
			GameStateManager::GetInstance().SetGameState(GameStates::SPLASHSCREEN);
		}

		ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, GameStateController::OnKeyEvent, this)
		ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowLostFocus, GameStateController::OnWindowOutOfFocus, this)
	}
	void GameStateController::Update(EntityID id, float deltaTime)
	{
		if (GameStateManager::GetInstance().GetGameState() == GameStates::SPLASHSCREEN)
		{
			m_ScriptData[id].SplashTimer -= deltaTime;

			if (m_ScriptData[id].SplashTimer <= 0)
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].SplashScreen).isActive = false;
				GameStateManager::GetInstance().SetGameState(GameStates::MOVEMENT);		
			}
		}
	}
	void GameStateController::Destroy(EntityID)
	{
	}

	GameStateController::~GameStateController()
	{
	}

	void GameStateController::OnAttach(EntityID id)
	{
		m_ScriptData[id] = GameStateControllerData();
	}

	void GameStateController::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	std::map<EntityID, GameStateControllerData>& GameStateController::GetScriptData()
	{
		return m_ScriptData;
	}

	rttr::instance GameStateController::GetScriptData(EntityID id)
	{
		return rttr::instance(m_ScriptData.at(id));
	}

	void GameStateController::OnWindowOutOfFocus(const PE::Event<PE::WindowEvents>& r_event)
	{
		if (GameStateManager::GetInstance().GetGameState() != GameStates::INACTIVE)
		{
			GameStateManager::GetInstance().SetPauseState();
		}
	}

	void GameStateController::OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event)
	{
		PE::KeyTriggeredEvent KTE;

		//dynamic cast
		if (r_event.GetType() == PE::KeyEvents::KeyTriggered)
		{
			KTE = dynamic_cast<const PE::KeyTriggeredEvent&>(r_event);

			if (KTE.keycode == GLFW_KEY_ESCAPE)
			{
				if (GameStateManager::GetInstance().GetGameState() != GameStates::INACTIVE)
				{
					GameStateManager::GetInstance().SetPauseState();
				}
			}
		}
	}
}