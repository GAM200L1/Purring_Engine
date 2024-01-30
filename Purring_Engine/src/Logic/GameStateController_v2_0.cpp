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
#include "SceneManager/scenemanager.h"
#include "System.h"
#include <Graphics/CameraManager.h>
#include <Physics/CollisionManager.h>
#include "ResourceManager/ResourceManager.h"
#include "Rat/RatController_v2_0.h"
namespace PE
{
	GameStateController_v2_0::GameStateController_v2_0()
	{
		REGISTER_UI_FUNCTION(ResumeStateV2, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(CloseHTP, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(OpenHTP, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(HTPPage1, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(HTPPage2, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(NextState, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(OpenAYS, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(ReturnFromAYS, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(RetryStage, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(SetPauseStateV2, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(RestartGame, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(OpenAYSR, PE::GameStateController_v2_0);
	}

	void GameStateController_v2_0::Init(EntityID id)
	{
		if (m_ScriptData[id].GameStateManagerActive)
		{
			if(m_currentLevel == 0)
			{ 
			currentState = GameStates_v2_0::SPLASHSCREEN;
			ActiveObject(m_ScriptData[id].SplashScreen);
			}
			else
			{
				currentState = GameStates_v2_0::PLANNING;
				ActiveObject(m_ScriptData[id].HUDCanvas);
			}
			//make sure all canvas inactive except for splashscreen 
		}

		m_ScriptData[id].keyEventHandlerId = ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, GameStateController_v2_0::OnKeyEvent, this)
		m_ScriptData[id].outOfFocusEventHandlerId = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowLostFocus, GameStateController_v2_0::OnWindowOutOfFocus, this)
		m_ScriptData[id].mouseClickEventID = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, GameStateController_v2_0::OnMouseClick, this)

	}
	void GameStateController_v2_0::Update(EntityID id, float deltaTime)
	{

		if (PauseManager::GetInstance().IsPaused())
		{
			SetPauseStateV2();
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
				for (auto id2 : SceneView<GUIButton>())
				{
					if (EntityManager::GetInstance().Has<GUIButton>(id2))
					{
						EntityManager::GetInstance().Get<GUIButton>(id2).disabled = true;
					}
				}
				ActiveObject(m_ScriptData[id].PauseMenuCanvas);
				m_pauseMenuOpenOnce = false;
			}
			return;
		}

		if (currentState == GameStates_v2_0::SPLASHSCREEN)
		{
			m_ScriptData[id].SplashTimer -= deltaTime;
			if (m_ScriptData[id].SplashTimer <= 0)
			{
				DeactiveObject(m_ScriptData[id].SplashScreen);
				DeactiveObject(m_ScriptData[m_currentGameStateControllerID].BackGroundCanvas);
				DeactiveAllMenu();
				ActiveObject(m_ScriptData[id].HUDCanvas);
				SetGameState(GameStates_v2_0::PLANNING);
			}
		}
		else
		{
			switch (currentState)
			{
			case GameStates_v2_0::PLANNING:
				DeactiveObject(m_ScriptData[m_currentGameStateControllerID].BackGroundCanvas);
				DeactiveAllMenu();
				PlanningStateHUD(id, deltaTime);
				for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].TurnCounterCanvas).children)
				{
					if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "TurnNumberText")
					{
						if (EntityManager::GetInstance().Has<TextComponent>(id2))
						{
							EntityManager::GetInstance().Get<TextComponent>(id2).SetText("Turn: " + std::to_string(CurrentTurn));
						}
						continue;
					}

					if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "CurrentTurnPhase")
					{
						if (EntityManager::GetInstance().Has<TextComponent>(id2))
						{
							EntityManager::GetInstance().Get<TextComponent>(id2).SetText("Plan Movement");
						}
						continue;
					}
				}
				prevState = currentState;
				break;
			case GameStates_v2_0::EXECUTE:
				DeactiveObject(m_ScriptData[m_currentGameStateControllerID].BackGroundCanvas);
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Default_256px.png"));
				DeactiveObject(m_ScriptData[m_currentGameStateControllerID].CatPortrait);
				DeactiveObject(m_ScriptData[m_currentGameStateControllerID].RatPortrait);
				DeactiveAllMenu();
				DeactiveObject(m_ScriptData[m_currentGameStateControllerID].CatPortrait);
				DeactiveObject(m_ScriptData[m_currentGameStateControllerID].RatPortrait);
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[m_currentGameStateControllerID].Portrait).SetTextureKey("../Assets/Textures/UnitPortrait_Default_256px.png");

				ExecutionStateHUD(id, deltaTime);
				for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].TurnCounterCanvas).children)
				{
					if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "CurrentTurnPhase")
					{
						if (EntityManager::GetInstance().Has<TextComponent>(id2))
						{
							EntityManager::GetInstance().Get<TextComponent>(id2).SetText("Executing...");
						}
						continue;
					}
				}
				prevState = currentState;
				break;
			case GameStates_v2_0::WIN:
				if (m_winOnce)
				{
					ActiveObject(m_ScriptData[id].BackGroundCanvas);
					ActiveObject(m_ScriptData[id].WinCanvas);
					m_winOnce = false;
				}
				break;
			case GameStates_v2_0::LOSE:				
				if (m_loseOnce)
				{
					ActiveObject(m_ScriptData[id].BackGroundCanvas);
					ActiveObject(m_ScriptData[id].LoseCanvas);
					m_loseOnce = false;
				}
				break;

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
		m_ScriptData[id].clicklisttest.resize(5);
		std::fill(m_ScriptData[id].clicklisttest.begin(), m_ScriptData[id].clicklisttest.end(), static_cast<EntityID>(-1));

		m_currentGameStateControllerID = id;
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
			SetPauseStateV2();
			PauseManager::GetInstance().SetPaused(true);
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
					SetPauseStateV2();
				}
			}

			if (KTE.keycode == GLFW_KEY_F1)
			{
				WinGame();
			}

			if (KTE.keycode == GLFW_KEY_F3)
			{
				LoseGame();
			}			
			if (KTE.keycode == GLFW_KEY_F11)
			{
				NextState();
			}
		}
	}

	void GameStateController_v2_0::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE;
		MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);

		if (MBPE.button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (currentState == GameStates_v2_0::PLANNING)
			{
				RatController_v2_0* RatManager = GETSCRIPTINSTANCEPOINTER(RatController_v2_0);

				for (auto[RatID,RatType] : RatManager->GetRats(RatManager->mainInstance))
				{
					if (EntityManager::GetInstance().Has<Transform>(RatID) && EntityManager::GetInstance().Has<Collider>(RatID))
					{
						//get mouse position
						vec2 cursorPosition{};
						GetMouseCurrentPosition(cursorPosition);

						//Get collider of the rat
						CircleCollider const& col = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(RatID).colliderVariant);

						//debug
						std::cout << cursorPosition.x << " " << cursorPosition.y << std::endl;

						// Check if the rat/cat has been clicked
						if (PointCollision(col, cursorPosition))
						{
							//debug
							std::cout << "Clicked on: " << EntityManager::GetInstance().Get<EntityDescriptor>(RatID).name << std::endl;
							//add a switch statement here
							//need specific texture
							
							//set rat portrait active
							ActiveObject(m_ScriptData[m_currentGameStateControllerID].RatPortrait);
							DeactiveObject(m_ScriptData[m_currentGameStateControllerID].CatPortrait);
							switch (RatType)
							{
							case 0: //GUTTER
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Rat_Gutter_256px.png"));
								SetPortraitName("UnitPortrait_RatNameFrame_GutterRat_239x82.png");
								break;
							case 1: //BRAWLER
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Rat_Brawler_256px.png"));
								SetPortraitName("UnitPortrait_RatNameFrame_BrawlerRatRat_239x82.png");
								break;
							case 2: //SNIPER
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Rat_Sniper_256px.png"));
								break;
							//case 3: //HERALD
								//break;
							}
							return;
						}
						else
						{
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Default_256px.png"));
							//set other portrait stuff inactive
							for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[m_currentGameStateControllerID].RatPortrait).children)
							{
								if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "RatPotraitName")
								{
									if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id2))
									{
										EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id2).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_NameFrame_Rat_239x82.png"));
									}
									break;
								}
							}
							DeactiveObject(m_ScriptData[m_currentGameStateControllerID].RatPortrait);
						}
					}
				}

				//for cats
				for (auto id : m_ScriptData[m_currentGameStateControllerID].clicklisttest)
				{
					if (EntityManager::GetInstance().Has<Transform>(id) && EntityManager::GetInstance().Has<Collider>(id))
					{
						vec2 cursorPosition{};
						InputSystem::GetCursorViewportPosition(WindowManager::GetInstance().p_currWindow, cursorPosition.x, cursorPosition.y);
						cursorPosition = GETCAMERAMANAGER()->GetWindowToWorldPosition(cursorPosition.x, cursorPosition.y);

						std::cout << cursorPosition.x << " " << cursorPosition.y << std::endl;

						// Check if the rat/cat has been clicked
						CircleCollider const& col = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);
						if (PointCollision(col, cursorPosition))
						{

							std::cout << "Clicked on: " << EntityManager::GetInstance().Get<EntityDescriptor>(id).name << std::endl;
							//add a switch statement here
							//need specific texture
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Cat_Orange_256px.png"));
							//set other prtrait stuff active and set right texture
							ActiveObject(m_ScriptData[m_currentGameStateControllerID].CatPortrait);
							for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[m_currentGameStateControllerID].CatPortrait).children)
							{
								if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "CatPortraitName")
								{
									if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id2))
									{
										EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id2).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_CatNameFrame_OrangeCat_239x82.png"));
									}
									break;
								}
							}
							return;
						}
						else
						{
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_ScriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Default_256px.png"));
							//set other portrait stuff inactive
							for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[m_currentGameStateControllerID].CatPortrait).children)
							{
								if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "CatPortraitName")
								{
									if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id2))
									{
										EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id2).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_NameFrame_Cat_239x82.png"));
									}
									break;
								}
							}
							DeactiveObject(m_ScriptData[m_currentGameStateControllerID].CatPortrait);
						}
					}
				}
			}
		}
	}

	void GameStateController_v2_0::SetPauseStateV2(EntityID)
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
			for (auto id : SceneView<GUIButton>())
			{
				if (EntityManager::GetInstance().Has<GUIButton>(id))
				{
					EntityManager::GetInstance().Get<GUIButton>(id).disabled = false;
				}
			}

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
			if (EntityManager::GetInstance().Has<GUIButton>(id2))
			{
				EntityManager::GetInstance().Get<GUIButton>(id2).disabled = false;
			}

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

	void GameStateController_v2_0::DeactiveObject(EntityID id)
	{
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
		{
			if (!EntityManager::GetInstance().Has<EntityDescriptor>(id2))
				break;

			EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
		}

		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;

		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = false;
	}

	void GameStateController_v2_0::FadeAllObject(EntityID id, float const alpha)
	{
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

		EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id).SetAlpha(alpha);
	}

	void GameStateController_v2_0::DeactiveAllMenu()
	{
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].PauseMenuCanvas);
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].HowToPlayCanvas);
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].AreYouSureCanvas);
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].LoseCanvas);
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].WinCanvas);
	}

	void GameStateController_v2_0::NextState(EntityID)
	{
		if (currentState == GameStates_v2_0::PLANNING)
		{
			SetGameState(GameStates_v2_0::EXECUTE);
		}
		else if (currentState == GameStates_v2_0::EXECUTE)
		{
			CurrentTurn++;
			SetGameState(GameStates_v2_0::PLANNING);
		}
	}

	void GameStateController_v2_0::WinGame()
	{
		SetGameState(GameStates_v2_0::WIN);
		m_winOnce = true;
	}

	void GameStateController_v2_0::LoseGame()
	{
		SetGameState(GameStates_v2_0::LOSE);
		m_loseOnce = true;
	}

	void GameStateController_v2_0::CloseHTP(EntityID)
	{
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].HowToPlayCanvas);
		ActiveObject(m_ScriptData[m_currentGameStateControllerID].PauseMenuCanvas);
	}

	void GameStateController_v2_0::OpenHTP(EntityID)
	{
		ActiveObject(m_ScriptData[m_currentGameStateControllerID].HowToPlayCanvas);
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].PauseMenuCanvas);
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].HowToPlayPageTwo);
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[m_currentGameStateControllerID].HowToPlayCanvas).children)
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "pg1")
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
			}
		}
	}

	void GameStateController_v2_0::HTPPage2(EntityID)
	{
		ActiveObject(m_ScriptData[m_currentGameStateControllerID].HowToPlayPageTwo);
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].HowToPlayPageOne);

		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[m_currentGameStateControllerID].HowToPlayCanvas).children)
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "pg1")
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = true;
			}
			else if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "pg2")
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
			}
		}
	}

	void GameStateController_v2_0::HTPPage1(EntityID)
	{
		ActiveObject(m_ScriptData[m_currentGameStateControllerID].HowToPlayPageOne);
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].HowToPlayPageTwo);

		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[m_currentGameStateControllerID].HowToPlayCanvas).children)
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "pg1")
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
			}
			else if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "pg2")
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = true;
			}
		}
	}

	void GameStateController_v2_0::PlanningStateHUD(EntityID const id, float deltaTime)
	{
		ActiveObject(m_ScriptData[id].HUDCanvas);
		ActiveObject(m_ScriptData[id].TurnCounterCanvas);

		if (prevState == GameStates_v2_0::EXECUTE)
		{
			m_timeSinceEnteredState = 0;
			m_timeSinceExitedState = m_UIFadeTimer;
		}

		m_timeSinceEnteredState += deltaTime;
		m_timeSinceExitedState -= deltaTime;

		float fadeOutSpeed = std::clamp(m_timeSinceExitedState / m_UIFadeTimer, 0.0f, 1.0f);
		float fadeInSpeed = std::clamp(m_timeSinceEnteredState / m_UIFadeTimer, 0.0f, 1.0f);

		FadeAllObject(m_ScriptData[id].HUDCanvas, fadeInSpeed);
		FadeAllObject(m_ScriptData[id].ExecuteCanvas, fadeOutSpeed);

		if (fadeInSpeed >= 1)
		{
			DeactiveObject(m_ScriptData[id].ExecuteCanvas);
		}
	}

	void GameStateController_v2_0::ExecutionStateHUD(EntityID const id, float deltaTime)
	{
		ActiveObject(m_ScriptData[id].ExecuteCanvas);

		if (prevState == GameStates_v2_0::PLANNING)
		{
			m_timeSinceEnteredState = 0;
			m_timeSinceExitedState = m_UIFadeTimer;
		}

		m_timeSinceEnteredState += deltaTime;
		m_timeSinceExitedState -= deltaTime;

		float fadeOutSpeed = std::clamp(m_timeSinceExitedState / m_UIFadeTimer, 0.0f, 1.0f);
		float fadeInSpeed = std::clamp(m_timeSinceEnteredState / m_UIFadeTimer, 0.0f, 1.0f);

		FadeAllObject(m_ScriptData[id].HUDCanvas, fadeOutSpeed);
		FadeAllObject(m_ScriptData[id].ExecuteCanvas, fadeInSpeed);

		if (fadeInSpeed >= 1)
		{
			DeactiveObject(m_ScriptData[id].HUDCanvas);
		}
	}

	void GameStateController_v2_0::ReturnFromAYS(EntityID)
	{
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].AreYouSureCanvas);
		DeactiveObject(m_ScriptData[m_currentGameStateControllerID].AreYouSureRestartCanvas);
		switch (currentState)
		{
		case GameStates_v2_0::PAUSE:
			ActiveObject(m_ScriptData[m_currentGameStateControllerID].PauseMenuCanvas);
			break;
		case GameStates_v2_0::WIN:
			ActiveObject(m_ScriptData[m_currentGameStateControllerID].WinCanvas);
			break;
		case GameStates_v2_0::LOSE:
			ActiveObject(m_ScriptData[m_currentGameStateControllerID].LoseCanvas);
			break;
		}
	}

	void GameStateController_v2_0::OpenAYS(EntityID)
	{
		DeactiveAllMenu();
		ActiveObject(m_ScriptData[m_currentGameStateControllerID].AreYouSureCanvas);
	}

	void GameStateController_v2_0::OpenAYSR(EntityID)
	{
		DeactiveAllMenu();
		ActiveObject(m_ScriptData[m_currentGameStateControllerID].AreYouSureRestartCanvas);
	}

	void GameStateController_v2_0::RetryStage(EntityID)
	{
		//other stuff that needs to resetted
		//SceneManager::GetInstance().LoadCurrentScene();

		//rmb to ask brandon for a restart scene function that dont need a filepath
	}

	void GameStateController_v2_0::RestartGame(EntityID)
	{
		//Set current stage to 0
		m_currentLevel = 0;
		//load that scene
		//i might want to store an array of scene names to load from actually
	}

	void GameStateController_v2_0::GetMouseCurrentPosition(vec2& Output)
	{
		InputSystem::GetCursorViewportPosition(WindowManager::GetInstance().p_currWindow, Output.x, Output.y);
		Output = GETCAMERAMANAGER()->GetWindowToWorldPosition(Output.x, Output.y);
	}

	void GameStateController_v2_0::SetPortraitName(const std::string_view TextureName)
	{
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[m_currentGameStateControllerID].RatPortrait).children)
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "RatPortraitName")
			{
				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id2))
				{
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id2).SetTextureKey(ResourceManager::GetInstance().LoadTexture(TextureName.data()));
				}
				break;
			}
		}
	}



}
