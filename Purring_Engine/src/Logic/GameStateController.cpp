/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     GameStateController.cpp 
 \date     23-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu
 \par      code %:     50%
 \par      changes:    Added functions to control the states + fade the splashscreen in.

 \co-author            Krystal Yamin
 \par      email:      krystal.y\@digipen.edu
 \par      code %:     50%
 \par      changes:    24-11-2023
											 Added functions and logic to update gameplay HUD.

 \brief  This file contains the script that interfaces with the GameStateManager
					and updates the in game HUD.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/

#include "prpch.h"
#include "GameStateController.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "LogicSystem.h"
#include "GameStateManager.h"
#include "Graphics/Renderer.h"
#include "Graphics/Text.h"
#include "CatScript.h"
#include "RatScript.h"

# define M_PI           3.14159265358979323846 

namespace PE 
{
	void GameStateController::Init(EntityID id)
	{
		//m_ScriptData[id].SplashScreen = PE::EntityManager::GetInstance().NewEntity();
		if (m_ScriptData[id].GameStateManagerActive)
		{
				GameStateManager::GetInstance().SetGameState(GameStates::SPLASHSCREEN);

				// Enable the planning HUD and splashscreen UI by default
				TogglePlanningHUD(id, true);
				ToggleExecutionHUD(id, false);
				ToggleSplashscreen(id, true);

				UpdateTurnHUD(id, GameStateManager::GetInstance().GetTurnNumber(), true);
				UpdateEnergyHUD(id, CatScript::GetMaximumEnergyLevel() - 1, CatScript::GetMaximumEnergyLevel() - 1);
		}

		m_ScriptData[id].keyEventHandlerId = ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, GameStateController::OnKeyEvent, this)
		m_ScriptData[id].outOfFocusEventHandlerId = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowLostFocus, GameStateController::OnWindowOutOfFocus, this)

		// Play BGM
		SerializationManager serializationManager;
		bgm = serializationManager.LoadFromFile("../Assets/Prefabs/AudioObject/Background Music_Prefab.json");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm)) 
		{
			EntityManager::GetInstance().Get<AudioComponent>(bgm).StopSound();
			EntityManager::GetInstance().Get<EntityDescriptor>(bgm).toSave = false;
		}

		m_ScriptData[id].prevState = GameStateManager::GetInstance().GetGameState();
	}
	void GameStateController::Update(EntityID id, float deltaTime)
	{
		if (GameStateManager::GetInstance().GetGameState() == GameStates::SPLASHSCREEN)
		{
			m_ScriptData[id].SplashTimer -= deltaTime;
			FadeSplashscreen(id, std::clamp(m_ScriptData[id].SplashTimer, 0.f, 1.f));

			// Fade the splashscreen in
			if (m_ScriptData[id].SplashTimer <= 0)
			{
				ToggleSplashscreen(id, false);
				GameStateManager::GetInstance().SetGameState(GameStates::MOVEMENT);	
				m_ScriptData[id].prevState = GameStates::SPLASHSCREEN;

				m_ScriptData[id].timeSinceEnteredState = 1.f;
				m_ScriptData[id].timeSinceExitedState = 0.f;

				if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
					EntityManager::GetInstance().Get<AudioComponent>(bgm).PlayAudioSound();
			}

			m_ScriptData[id].prevState = GameStateManager::GetInstance().GetGameState();
		} 
		else
		{ 
			// Show the gameplay UI
		  switch(GameStateManager::GetInstance().GetGameState())
			{
			case GameStates::MOVEMENT:
			{
					MovementStateHUD(id, deltaTime);
					m_ScriptData[id].prevState = GameStateManager::GetInstance().GetGameState();
					break;
			}
			case GameStates::ATTACK:
			{
					AttackStateHUD(id);
					m_ScriptData[id].prevState = GameStateManager::GetInstance().GetGameState();
					break;
			}
			case GameStates::EXECUTE:
			{
					ExecutionStateHUD(id, deltaTime);
					m_ScriptData[id].prevState = GameStateManager::GetInstance().GetGameState();
					// Update animations and player actions
					ExecutionToMovement();
					break;
			}
			case GameStates::WIN:
			case GameStates::LOSE:
			{
					TogglePlanningHUD(id, false);
					ToggleExecutionHUD(id, false);
					EndPhaseButton(id, false);
					ToggleEntity(m_ScriptData[id].endTurnText, false);
					m_ScriptData[id].prevState = GameStateManager::GetInstance().GetGameState();
					break;
			}				
			case GameStates::PAUSE:
			{
					if (GameStateManager::GetInstance().GetHowToPlayActive())
					{
							ToggleAllText(id, false);
					}
					else
					{
							switch (GameStateManager::GetInstance().GetPreviousGameState()) 
							{
							case GameStates::MOVEMENT:
							{
									MovementStateHUD(id, 0.f);
									break;
							}
							case GameStates::ATTACK:
							{
									AttackStateHUD(id);
									break;
							}
							case GameStates::EXECUTE:
							{
									ExecutionStateHUD(id, 0.f);
									ToggleEntity(m_ScriptData[id].executingStatement, false);
									break;
							}
							default: { break; }
							}
					}
					m_ScriptData[id].prevState = GameStateManager::GetInstance().GetGameState();
					break;
			}
			default: 
			{
					m_ScriptData[id].prevState = GameStateManager::GetInstance().GetGameState(); 
					break; 
			}
			}
		}
	}
	void GameStateController::Destroy(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
		{
			REMOVE_KEY_EVENT_LISTENER(m_ScriptData[id].keyEventHandlerId);
			REMOVE_WINDOW_EVENT_LISTENER(m_ScriptData[id].outOfFocusEventHandlerId);
		}
	}

	GameStateController::~GameStateController()
	{
	}

	void GameStateController::OnAttach(EntityID id)
	{
		m_ScriptData[id] = GameStateControllerData();

		// Enable the splashscreen and planning HUD, disable the execution phase HUD
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


	void GameStateController::MovementStateHUD(EntityID const id, float deltaTime)
	{
			// Enable all HUD elements and begin fading in/out
			if (m_ScriptData[id].prevState == GameStates::EXECUTE)
			{
					m_ScriptData[id].timeSinceEnteredState = 0;
					m_ScriptData[id].timeSinceExitedState = m_ScriptData[id].maxFadeTime;
			}

			TogglePlanningHUD(id, true);
			EndPhaseButton(id, true);
			UpdateTurnHUD(id, GameStateManager::GetInstance().GetTurnNumber(), true);

			// Fade the HUD elements in
			m_ScriptData[id].timeSinceExitedState -= deltaTime;
			m_ScriptData[id].timeSinceEnteredState += deltaTime;

			float fadeOutSpeed = std::clamp(m_ScriptData[id].timeSinceExitedState / m_ScriptData[id].maxFadeTime, 0.f, 1.f);
			float fadeInSpeed = std::clamp(m_ScriptData[id].timeSinceEnteredState / m_ScriptData[id].maxFadeTime, 0.f, 1.f);

			FadeExecutionHUD(id, fadeOutSpeed);
			FadePlanningHUD(id, fadeInSpeed);

			// Disable the execution HUD
			if (fadeInSpeed >= 1)
			{
					ToggleExecutionHUD(id, false);

					if (EntityManager::GetInstance().Has(m_ScriptData[id].endTurnButton, EntityManager::GetInstance().GetComponentID<GUI>()))
					{
							EntityManager::GetInstance().Get<GUI>(m_ScriptData[id].endTurnButton).disabled = false;
					}
			}

			// Update the energy level
			UpdateEnergyHUD(id, CatScript::GetCurrentEnergyLevel(), CatScript::GetMaximumEnergyLevel() - 1);
	}


	void GameStateController::AttackStateHUD(EntityID const id)
	{
			// Enable planning HUD, disable execution HUD
			TogglePlanningHUD(id, true);
			ToggleExecutionHUD(id, false);
			EndPhaseButton(id, false);
			UpdateTurnHUD(id, GameStateManager::GetInstance().GetTurnNumber(), false);
	}


	void GameStateController::ExecutionStateHUD(EntityID const id, float deltaTime)
	{
			// Start fading HUD elements in/out if changing from planning phase
			if (m_ScriptData[id].prevState == GameStates::ATTACK)
			{
					m_ScriptData[id].timeSinceEnteredState = 0;
					m_ScriptData[id].timeSinceExitedState = m_ScriptData[id].maxFadeTime;
			}

			ToggleExecutionHUD(id, true);

			// Fade in/out
			m_ScriptData[id].timeSinceExitedState -= deltaTime;
			m_ScriptData[id].timeSinceEnteredState += deltaTime;

			float fadeOutSpeed = std::clamp(m_ScriptData[id].timeSinceExitedState / m_ScriptData[id].maxFadeTime, 0.f, 1.f);
			float fadeInSpeed = std::clamp(m_ScriptData[id].timeSinceEnteredState / m_ScriptData[id].maxFadeTime, 0.f, 1.f);

			FadePlanningHUD(id, fadeOutSpeed);

			// Disable button while fading
			if (EntityManager::GetInstance().Has(m_ScriptData[id].endTurnButton, EntityManager::GetInstance().GetComponentID<GUI>()))
			{
					EntityManager::GetInstance().Get<GUI>(m_ScriptData[id].endTurnButton).disabled = true;
			}

			FadeExecutionHUD(id, fadeInSpeed);

			// Disable planning HUD when planning HUD is invisible
			if (fadeInSpeed >= 1)
			{
					TogglePlanningHUD(id, false);
					UpdateExecuteHUD(id, deltaTime);
			}
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
	
	void GameStateController::ToggleAllText(EntityID id, bool enable)
	{
			// Toggle the energy HUD
			ToggleEntity(m_ScriptData[id].energyHeader, enable);
			ToggleEntity(m_ScriptData[id].currentEnergyText, enable);
			ToggleEntity(m_ScriptData[id].slashText, enable);
			ToggleEntity(m_ScriptData[id].maxEnergyText, enable);

			// Toggle the turn HUD
			ToggleEntity(m_ScriptData[id].turnNumberText, enable);
			ToggleEntity(m_ScriptData[id].planAttackText, enable); 
			ToggleEntity(m_ScriptData[id].planMovementText, enable);

			// Toggle the end phase buttons
			ToggleEntity(m_ScriptData[id].endMovementText, enable);
			ToggleEntity(m_ScriptData[id].endTurnText, enable); 

			// Toggle the overlay
			ToggleEntity(m_ScriptData[id].executingStatement, enable);
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
		if (GameStateManager::GetInstance().GetGameState() != GameStates::INACTIVE && GameStateManager::GetInstance().GetGameState() != GameStates::WIN && GameStateManager::GetInstance().GetGameState() != GameStates::LOSE)
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


			if (KTE.keycode == GLFW_KEY_F1)
			{
				GameStateManager::GetInstance().SetWinState();
			}

			if (KTE.keycode == GLFW_KEY_F2)
			{
				GameStateManager::GetInstance().SetLoseState();
			}

			if (KTE.keycode == GLFW_KEY_F3)
			{
				GameStateManager::GetInstance().godMode = !GameStateManager::GetInstance().godMode;
				SerializationManager serializationManager;

				if (GameStateManager::GetInstance().godMode)
				{
					godModeText = serializationManager.LoadFromFile("../Assets/Prefabs/HUD/God Mode_Prefab.json");
					if (EntityManager::GetInstance().Has<EntityDescriptor>(godModeText))
						EntityManager::GetInstance().Get<EntityDescriptor>(godModeText).toSave = false;
				}
				else
				{
					if (EntityManager::GetInstance().Has<EntityDescriptor>(godModeText))
						EntityManager::GetInstance().RemoveEntity(godModeText);
				}

			}

			if (KTE.keycode == GLFW_KEY_F10)
			{
				GameStateManager::GetInstance().RestartGame(-1);
			}
		}
	}

	void GameStateController::ExecutionToMovement()
	{
		if (!m_finishExecution)
		{
			for (EntityID scriptID : SceneView<ScriptComponent>())
			{
				if (!EntityManager::GetInstance().Get<EntityDescriptor>(scriptID).isActive) { continue; }
				if (EntityManager::GetInstance().Get<ScriptComponent>(scriptID).m_scriptKeys.find("RatScript") != EntityManager::GetInstance().Get<ScriptComponent>(scriptID).m_scriptKeys.end())
				{
					RatScriptData* p_ratScript = GETSCRIPTDATA(RatScript, scriptID);
					if (!p_ratScript->finishedExecution)
					{
						m_finishExecution = false;
						break;
					}
					else
					{
						m_finishExecution = true;
					}
				}
				else if (EntityManager::GetInstance().Get<ScriptComponent>(scriptID).m_scriptKeys.find("CatScript") != EntityManager::GetInstance().Get<ScriptComponent>(scriptID).m_scriptKeys.end())
				{
					CatScriptData* p_catScript = GETSCRIPTDATA(CatScript, scriptID);
					if (!p_catScript->finishedExecution)
					{
						m_finishExecution = false;
						break;
					}
					else
					{
						m_finishExecution = true;
					}
				}
			}
		}
		else
		{
			GameStateManager::GetInstance().IncrementGameState();
			m_finishExecution = false;
		}
	}
}