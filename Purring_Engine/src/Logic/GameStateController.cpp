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
#include "CatScript.h"

# define M_PI           3.14159265358979323846 

namespace PE 
{
	void GameStateController::Init(EntityID id)
	{
		//m_ScriptData[id].SplashScreen = PE::EntityManager::GetInstance().NewEntity();
		if (m_ScriptData[id].GameStateManagerActive)
		{
				GameStateManager::GetInstance().SetGameState(GameStates::SPLASHSCREEN);

				TogglePlanningHUD(id, true);
				ToggleExecutionHUD(id, false);
				ToggleSplashscreen(id, true);

				UpdateTurnHUD(id, GameStateManager::GetInstance().GetTurnNumber(), true);
				UpdateEnergyHUD(id, CatScript::GetMaximumEnergyLevel() - 1, CatScript::GetMaximumEnergyLevel() - 1);
		}

		ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, GameStateController::OnKeyEvent, this)
		ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowLostFocus, GameStateController::OnWindowOutOfFocus, this)
	}
	void GameStateController::Update(EntityID id, float deltaTime)
	{
		if (GameStateManager::GetInstance().GetGameState() == GameStates::SPLASHSCREEN)
		{
			m_ScriptData[id].SplashTimer -= deltaTime;
			FadeSplashscreen(id, std::clamp(m_ScriptData[id].SplashTimer, 0.f, 1.f));

			if (m_ScriptData[id].SplashTimer <= 0)
			{
				ToggleSplashscreen(id, false);
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
					EndPhaseButton(id, true);
					UpdateTurnHUD(id, GameStateManager::GetInstance().GetTurnNumber(), true);

					// Update the energy level
					UpdateEnergyHUD(id, CatScript::GetCurrentEnergyLevel(), CatScript::GetMaximumEnergyLevel() - 1);
					break;
			}
			case GameStates::ATTACK:
			{
					TogglePlanningHUD(id, true);
					ToggleExecutionHUD(id, false);
					EndPhaseButton(id, false);
					UpdateTurnHUD(id, GameStateManager::GetInstance().GetTurnNumber(), false);

					break;
			}
			case GameStates::EXECUTE:
			{
					TogglePlanningHUD(id, false);
					ToggleExecutionHUD(id, true);
					UpdateExecuteHUD(id, deltaTime);
					break;
			}
			default: { break; }
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

		TogglePlanningHUD(id, true);
		ToggleExecutionHUD(id, false);
		ToggleSplashscreen(id, true);

		UpdateTurnHUD(id, GameStateManager::GetInstance().GetTurnNumber(), true);
		UpdateEnergyHUD(id, CatScript::GetMaximumEnergyLevel() - 1, CatScript::GetMaximumEnergyLevel() - 1);
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
			ToggleEntity(m_ScriptData[id].endMovementText, endMovement);
			ToggleEntity(m_ScriptData[id].endTurnText, !endMovement);
	}

	void GameStateController::UpdateExecuteHUD(EntityID id, float const deltaTime)
	{
			// Fade the color in and out over time
			static float timePassed{};
			timePassed += deltaTime;
			SetTextAlpha(m_ScriptData[id].executingStatement, std::sin(timePassed * m_ScriptData[id].executingFadeSpeed) + 1.f * 0.5f);
	}

	void GameStateController::UpdateTurnHUD(EntityID id, int const turnCount, bool isMovement)
	{
			SetText(m_ScriptData[id].turnNumberText, std::string{ "Turn " + std::to_string(turnCount) });

			ToggleEntity(m_ScriptData[id].planMovementText, isMovement);
			ToggleEntity(m_ScriptData[id].planAttackText, !isMovement);
	}

	void GameStateController::UpdateEnergyHUD(EntityID id, int const currentEnergy, int const maximumEnergy)
	{
			SetText(m_ScriptData[id].currentEnergyText, std::to_string(currentEnergy));
			SetText(m_ScriptData[id].maxEnergyText, std::to_string(maximumEnergy));
	}

	void GameStateController::TogglePlanningHUD(EntityID id, bool enable)
	{
			// Toggle the overlay
			ToggleEntity(m_ScriptData[id].mapOverlay, enable);
			ToggleEntity(m_ScriptData[id].pawOverlay, enable);

			// Toggle the energy HUD
			ToggleEntity(m_ScriptData[id].energyHeader, enable);
			ToggleEntity(m_ScriptData[id].currentEnergyText, enable);
			ToggleEntity(m_ScriptData[id].slashText, enable);
			ToggleEntity(m_ScriptData[id].maxEnergyText, enable);
			ToggleEntity(m_ScriptData[id].energyBackground, enable);

			// Toggle the turn HUD
			ToggleEntity(m_ScriptData[id].turnNumberText, enable);
			ToggleEntity(m_ScriptData[id].planAttackText, false); // Ensure only movement text is active if planning hud is toggled
			ToggleEntity(m_ScriptData[id].planMovementText, enable);
			ToggleEntity(m_ScriptData[id].turnBackground, enable);

			// Toggle the end phase buttons
			ToggleEntity(m_ScriptData[id].endTurnButton, enable);
			ToggleEntity(m_ScriptData[id].endMovementText, enable);
			ToggleEntity(m_ScriptData[id].endTurnText, false); // Ensure only movement text is active if planning hud is toggled
	}

	void GameStateController::ToggleExecutionHUD(EntityID id, bool enable)
	{
			// Toggle the overlay
			ToggleEntity(m_ScriptData[id].executingStatement, enable);
			ToggleEntity(m_ScriptData[id].foliageOverlay, enable);
	}
	
	void GameStateController::ToggleSplashscreen(EntityID id, bool enable)
	{
			// Toggle the splashscreen
			ToggleEntity(m_ScriptData[id].SplashScreen, enable);
	}

	bool GameStateController::ToggleEntity(EntityID id, bool enable)
	{
			if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<EntityDescriptor>()))
			{
					EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = enable;
					return true;
			}

			return false;
	}

	void GameStateController::FadePlanningHUD(EntityID const id, float alpha)
	{
			// Adjust the alpha of the overlay
			SetGUIRendererAlpha(m_ScriptData[id].mapOverlay, alpha);
			SetGUIRendererAlpha(m_ScriptData[id].pawOverlay, alpha);

			// Adjust the alpha of the energy HUD
			SetTextAlpha(m_ScriptData[id].energyHeader, alpha);
			SetTextAlpha(m_ScriptData[id].currentEnergyText, alpha);
			SetTextAlpha(m_ScriptData[id].slashText, alpha);
			SetTextAlpha(m_ScriptData[id].maxEnergyText, alpha);
			SetGUIRendererAlpha(m_ScriptData[id].energyBackground, alpha);

			// Adjust the alpha of the turn HUD
			SetTextAlpha(m_ScriptData[id].turnNumberText, alpha);
			SetTextAlpha(m_ScriptData[id].planAttackText, alpha);
			SetTextAlpha(m_ScriptData[id].planMovementText, alpha);
			SetGUIRendererAlpha(m_ScriptData[id].turnBackground, alpha);

			// Adjust the alpha of the end phase buttons
			SetGUIRendererAlpha(m_ScriptData[id].endTurnButton, alpha);
			SetTextAlpha(m_ScriptData[id].endMovementText, alpha);
			SetTextAlpha(m_ScriptData[id].endTurnText, alpha);
	}

	void GameStateController::FadeExecutionHUD(EntityID const id, float const alpha)
	{
			// Adjust the alpha of the execution HUD
			SetTextAlpha(m_ScriptData[id].executingStatement, alpha);
			SetGUIRendererAlpha(m_ScriptData[id].foliageOverlay, alpha);
	}

	void GameStateController::FadeSplashscreen(EntityID const id, float const alpha)
	{
			// Adjust the alpha of the execution HUD
			if (EntityManager::GetInstance().Has(m_ScriptData[id].SplashScreen, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
			{ EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[id].SplashScreen).SetAlpha(alpha); }
	}

	bool GameStateController::SetText(EntityID const id, std::string const& text)
	{
			if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<TextComponent>()))
			{
					EntityManager::GetInstance().Get<TextComponent>(id).SetText(text);
					return true;
			}

			return false;
	}

	bool GameStateController::SetTextAlpha(EntityID const id, float const alpha)
	{
			if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<TextComponent>()))
			{
					EntityManager::GetInstance().Get<TextComponent>(id).SetAlpha(alpha);
					return true;
			}

			return false;
	}

	bool GameStateController::SetGUIRendererAlpha(EntityID const id, float const alpha)
	{
			if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
			{
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id).SetAlpha(alpha);
					return true;
			}

			return false;
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