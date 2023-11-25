/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     GameStateController.cpp 
 \date     23-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu
 \par      code %:     50%
 \par      changes:    Added functions to 

 \co-author            Krystal Yamin
 \par      email:      krystal.y\@digipen.edu
 \par      code %:     50%
 \par      changes:    24-11-2023
											 Added functions to update gameplay HUD.

 \brief  This file contains the script that interfaces with the GameStateManager.

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
#include "Graphics/Text.h"
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
		else
		{ 
			// Show the gameplay UI
		  switch(GameStateManager::GetInstance().GetGameState())
			{
			case GameStates::MOVEMENT:
			{
					TogglePlanningHUD(id, true); 
					ToggleExecutionHUD(id, false);
					EndPhaseButton(id, "End Movement");

					// Update the energy level
					break;
			}
			case GameStates::ATTACK:
			{
					TogglePlanningHUD(id, true);
					ToggleExecutionHUD(id, false);
					EndPhaseButton(id, "End Turn");
					break;
			}
			case GameStates::EXECUTE:
			{
					TogglePlanningHUD(id, false);
					ToggleExecutionHUD(id, true);
					UpdateExecuteHUD(id, deltaTime);
					break;
			}
			default: {break; }
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

	void GameStateController::EndPhaseButton(EntityID id, bool endMovement)
	{
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].endMovementText).isActive = endMovement;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].endTurnText).isActive = !endMovement;
	}

	void GameStateController::UpdateExecuteHUD(EntityID id, float const deltaTime)
	{
			static float timePassed{};
			timePassed += deltaTime;

			// Fade the color in and out over time
			TextComponent& r_text{ EntityManager::GetInstance().Get<TextComponent>(id) };
			r_text.SetAlpha(std::sin(timePassed) + 1.f * 0.5f);
	}

	void GameStateController::UpdateTurnHUD(EntityID id, int const turnCount, bool isMovement)
	{
			EntityManager::GetInstance().Get<TextComponent>(m_ScriptData[id].turnNumberText).SetText(std::string{"Turn " + std::to_string(turnCount)});

			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].planMovementText).isActive = isMovement;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].planAttackText).isActive = !isMovement;
	}

	void GameStateController::TogglePlanningHUD(EntityID id, bool enable)
	{
			// Toggle the overlay
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].mapOverlay).isActive = enable;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].pawOverlay).isActive = enable;

			// Toggle the energy HUD
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].energyHeader).isActive = enable;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].currentEnergyText).isActive = enable;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].slashText).isActive = enable;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].maxEnergyText).isActive = enable;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].energyBackground).isActive = enable;

			// Toggle the turn HUD
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].turnNumberText).isActive = enable;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].planAttackText).isActive = false; // Ensure only movement text is active if planning hud is toggled
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].planMovementText).isActive = enable;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].turnBackground).isActive = enable;

			// Toggle the end phase buttons
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].endTurnButton).isActive = enable;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].endMovementText).isActive = enable;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].endTurnText).isActive = false; // Ensure only movement text is active if planning hud is toggled
	}

	void GameStateController::ToggleExecutionHUD(EntityID id, bool enable)
	{
			// Toggle the overlay
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].executingStatement).isActive = enable;
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].foliageOverlay).isActive = enable;
	}

	void GameStateController::FadePlanningHUD(EntityID id, float alpha)
	{
			// Toggle the overlay			
			if (EntityManager::GetInstance().Has(m_ScriptData[id].mapOverlay, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
			{ EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[id].mapOverlay).SetAlpha(alpha); }
			if (EntityManager::GetInstance().Has(m_ScriptData[id].pawOverlay, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
			{ EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[id].pawOverlay).SetAlpha(alpha); }

			// Toggle the energy HUD
			if (EntityManager::GetInstance().Has(m_ScriptData[id].energyHeader, EntityManager::GetInstance().GetComponentID<TextComponent>()))
				EntityManager::GetInstance().Get<TextComponent>(m_ScriptData[id].energyHeader).SetAlpha(alpha);
			if (EntityManager::GetInstance().Has(m_ScriptData[id].currentEnergyText, EntityManager::GetInstance().GetComponentID<TextComponent>()))
				EntityManager::GetInstance().Get<TextComponent>(m_ScriptData[id].currentEnergyText).SetAlpha(alpha);
			if (EntityManager::GetInstance().Has(m_ScriptData[id].slashText, EntityManager::GetInstance().GetComponentID<TextComponent>()))
				EntityManager::GetInstance().Get<TextComponent>(m_ScriptData[id].slashText).SetAlpha(alpha);
			if (EntityManager::GetInstance().Has(m_ScriptData[id].maxEnergyText, EntityManager::GetInstance().GetComponentID<TextComponent>()))
				EntityManager::GetInstance().Get<TextComponent>(m_ScriptData[id].maxEnergyText).SetAlpha(alpha);
			if (EntityManager::GetInstance().Has(m_ScriptData[id].energyBackground, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[id].energyBackground).SetAlpha(alpha);

			// Toggle the turn HUD
			if (EntityManager::GetInstance().Has(m_ScriptData[id].turnNumberText, EntityManager::GetInstance().GetComponentID<TextComponent>()))
				EntityManager::GetInstance().Get<TextComponent>(m_ScriptData[id].turnNumberText).SetAlpha(alpha);
			if (EntityManager::GetInstance().Has(m_ScriptData[id].planAttackText, EntityManager::GetInstance().GetComponentID<TextComponent>()))
				EntityManager::GetInstance().Get<TextComponent>(m_ScriptData[id].planAttackText).SetAlpha(alpha);
			if (EntityManager::GetInstance().Has(m_ScriptData[id].planMovementText, EntityManager::GetInstance().GetComponentID<TextComponent>()))
				EntityManager::GetInstance().Get<TextComponent>(m_ScriptData[id].planMovementText).SetAlpha(alpha);
			if (EntityManager::GetInstance().Has(m_ScriptData[id].turnBackground, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[id].turnBackground).SetAlpha(alpha);

			// Toggle the end phase buttons
			if (EntityManager::GetInstance().Has(m_ScriptData[id].endTurnButton, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[id].endTurnButton).SetAlpha(alpha);
			if (EntityManager::GetInstance().Has(m_ScriptData[id].endMovementText, EntityManager::GetInstance().GetComponentID<TextComponent>()))
				EntityManager::GetInstance().Get<TextComponent>(m_ScriptData[id].endMovementText).SetAlpha(alpha);
			if (EntityManager::GetInstance().Has(m_ScriptData[id].endTurnText, EntityManager::GetInstance().GetComponentID<TextComponent>()))
				EntityManager::GetInstance().Get<TextComponent>(m_ScriptData[id].endTurnText).SetAlpha(alpha);
	}

	void GameStateController::FadeExecutionHUD(EntityID id, float alpha)
	{
			// Adjust the alpha of the execution HUD
			if (EntityManager::GetInstance().Has(m_ScriptData[id].executingStatement, EntityManager::GetInstance().GetComponentID<TextComponent>()))
			{ EntityManager::GetInstance().Get<TextComponent>(m_ScriptData[id].executingStatement).SetAlpha(alpha); }

			if (EntityManager::GetInstance().Has(m_ScriptData[id].foliageOverlay, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
			{ EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[id].foliageOverlay).SetAlpha(alpha); }
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
				if (GameStateManager::GetInstance().GetGameState() == GameStates::PAUSE)
				{
					GameStateManager::GetInstance().ResumeState();
				}
				else if (GameStateManager::GetInstance().GetGameState() != GameStates::INACTIVE)
				{
					GameStateManager::GetInstance().SetPauseState();
				}
			}
		}
	}
}