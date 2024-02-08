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
#include "Cat/CatController_v2_0.h"
#include "Cat/CatScript_v2_0.h"
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
		if (m_scriptData[id].GameStateManagerActive)
		{
			if (m_currentLevel == 0)
			{
				ActiveObject(m_scriptData[id].HUDCanvas);
				ActiveObject(m_scriptData[id].TurnCounterCanvas);

				prevState = GameStates_v2_0::INACTIVE;
				currentState = GameStates_v2_0::PLANNING;
				m_isTransitioning = true;
				m_isTransitioningIn = true;
				m_timeSinceTransitionStarted = 0;
				m_timeSinceTransitionEnded = m_transitionTimer;
			}
			else // if not first level
			{
				ActiveObject(m_scriptData[id].HUDCanvas);
				ActiveObject(m_scriptData[id].TurnCounterCanvas);

				m_isTransitioning = true;
				m_isTransitioningIn = true;
				m_timeSinceTransitionStarted = 0;
				m_timeSinceTransitionEnded = m_transitionTimer;
				prevState = GameStates_v2_0::INACTIVE;
				currentState = GameStates_v2_0::DEPLOYMENT;

				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].PhaseBanner))
				{
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].PhaseBanner).SetTextureKey(ResourceManager::GetInstance().LoadTexture("PhaseSplash_Deployment_1429x415.png"));
				}

			}
		}

		//subscribe to events
		m_scriptData[id].keyEventHandlerId = ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, GameStateController_v2_0::OnKeyEvent, this)
			m_scriptData[id].outOfFocusEventHandlerId = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowLostFocus, GameStateController_v2_0::OnWindowOutOfFocus, this)
			m_scriptData[id].mouseClickEventID = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, GameStateController_v2_0::OnMouseClick, this)

			//resetting current turn
			CurrentTurn = 0;


		//getting the texture key for the current background and adding sepia to it
		m_currentLevelBackground = EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].Background).GetTextureKey();
		std::stringstream newTextureName;
		newTextureName << m_currentLevelBackground.substr(m_currentLevelBackground.find_last_of('/') + 1, m_currentLevelBackground.find_last_of('_'));
		m_currentLevelBackground = newTextureName.str();
		newTextureName = std::stringstream();
		newTextureName << m_currentLevelBackground.substr(0, m_currentLevelBackground.find_last_of('_')) << "_Sepia" << m_currentLevelBackground.substr(m_currentLevelBackground.find_last_of('_'), m_currentLevelBackground.length());
		m_currentLevelSepiaBackground = newTextureName.str();

		//save the texture keys
		m_currentLevelBackground = ResourceManager::GetInstance().LoadTexture(m_currentLevelBackground);
		m_currentLevelSepiaBackground = ResourceManager::GetInstance().LoadTexture(m_currentLevelSepiaBackground);
		m_defaultPotraitTextureKey = ResourceManager::GetInstance().LoadTexture("UnitPortrait_Default_256px.png");
		m_planningPhaseBanner = ResourceManager::GetInstance().LoadTexture("PhaseSplash_Planning_1429x415.png");
		m_deploymentPhaseBanner = ResourceManager::GetInstance().LoadTexture("PhaseSplash_Deployment_1429x415.png");
		m_exexcutePhaseBanner = ResourceManager::GetInstance().LoadTexture("PhaseSplash_Execution_1429x415.png");



		//start the background music
		EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Background Music_Prefab.json");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
			EntityManager::GetInstance().Get<AudioComponent>(bgm).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(bgm);

		//start the background ambience
		EntityID bga = m_serializationManager.LoadFromFile("AudioObject/Background Ambience_Prefab.json");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bga))
			EntityManager::GetInstance().Get<AudioComponent>(bga).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(bga);

		ResetPhaseBanner(true);
	}
	void GameStateController_v2_0::Update(EntityID id, float deltaTime)
	{
		if (!m_isRat && m_isPotraitShowing)
		{
			CatScript_v2_0Data* cat = GETSCRIPTDATA(CatScript_v2_0, m_lastSelectedEntity);
			SetPortraitInformation("UnitPortrait_CatNameFrame_Meowsalot_239x82.png", cat->catCurrentEnergy, cat->catMaxMovementEnergy, 0);
		}


		if (m_isTransitioning) // if running the transitioning bgm, this is detached from states so it can happen anytime
		{
			TransitionPanelFade(id, deltaTime, m_isTransitioningIn);
		}

		//check pause from pause menu, this is so window can set pause as well
		if (PauseManager::GetInstance().IsPaused())
		{
			if (currentState != GameStates_v2_0::PAUSE)
			{
				SetPauseStateV2();
			}
		}

		//if paused, but there are some stuff we want to only run once
		if (currentState == GameStates_v2_0::PAUSE)
		{
			ActiveObject(m_scriptData[id].PauseBackGroundCanvas);
			if (m_pauseMenuOpenOnce)
			{
				for (auto id2 : SceneView<GUIButton>())
				{
					if (EntityManager::GetInstance().Has<GUIButton>(id2))
					{
						EntityManager::GetInstance().Get<GUIButton>(id2).disabled = true;
					}
				}
				ActiveObject(m_scriptData[id].PauseMenuCanvas);
				m_pauseMenuOpenOnce = false;
			}
		}

		//switch statement for the different states
		switch (currentState)
		{
		case GameStates_v2_0::DEPLOYMENT: // deploying cats, need to disable end turn button, but otherwise normal HUD stays
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].Background))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].Background).SetTextureKey(m_currentLevelSepiaBackground);

			DeactiveObject(m_scriptData[m_currentGameStateControllerID].PauseBackGroundCanvas);
			DeactiveAllMenu();
			ActiveObject(m_scriptData[id].HUDCanvas);
			ActiveObject(m_scriptData[id].TurnCounterCanvas);

			PhaseBannerTransition(id, deltaTime);
			UpdateTurnCounter("Deployment");

			for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].HUDCanvas).children)
			{
				if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "EndTurnButton")
				{
					if (EntityManager::GetInstance().Has<EntityDescriptor>(id2))
					{
						EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
					}
					continue;
				}

				if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "EndPlanningText")
				{
					if (EntityManager::GetInstance().Has<EntityDescriptor>(id2))
					{
						EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
					}
					continue;
				}

			}
			break;
		}
		case GameStates_v2_0::PLANNING: // normal planning state, fade in HUD and fade out foliage
		{
			DeactiveObject(m_scriptData[m_currentGameStateControllerID].PauseBackGroundCanvas);
			DeactiveAllMenu();
			PlanningStateHUD(id, deltaTime);

			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].Background))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].Background).SetTextureKey(m_currentLevelSepiaBackground);

			PhaseBannerTransition(id, deltaTime);
			UpdateTurnCounter("Plan Movement");
			prevState = currentState;
			break;
		}
		case GameStates_v2_0::EXECUTE: // execute state, fade out HUD and fade in foliage
		{

			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].Background))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].Background).SetTextureKey(m_currentLevelBackground);

			DeactiveObject(m_scriptData[m_currentGameStateControllerID].PauseBackGroundCanvas);
			EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(m_defaultPotraitTextureKey);
			DeactiveObject(m_scriptData[m_currentGameStateControllerID].CatPortrait);
			DeactiveObject(m_scriptData[m_currentGameStateControllerID].RatPortrait);
			DeactiveAllMenu();

			ExecutionStateHUD(id, deltaTime);
			PhaseBannerTransition(id, deltaTime);
			UpdateTurnCounter("Executing...");
			prevState = currentState;
			break;
		}
		case GameStates_v2_0::WIN: // win state, show win canvas
			if (m_winOnce)
			{
				ActiveObject(m_scriptData[id].PauseBackGroundCanvas);
				ActiveObject(m_scriptData[id].WinCanvas);
				m_winOnce = false;
			}
			break;
		case GameStates_v2_0::LOSE: //lose state, show lose canvas				
			if (m_loseOnce)
			{
				ActiveObject(m_scriptData[id].PauseBackGroundCanvas);
				ActiveObject(m_scriptData[id].LoseCanvas);
				m_loseOnce = false;
			}
			break;

		}
	}

	void GameStateController_v2_0::Destroy(EntityID id)
	{
		//unsub from all events
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			REMOVE_KEY_EVENT_LISTENER(m_scriptData[id].keyEventHandlerId);
			REMOVE_WINDOW_EVENT_LISTENER(m_scriptData[id].outOfFocusEventHandlerId);
			REMOVE_MOUSE_EVENT_LISTENER(m_scriptData[id].mouseClickEventID);
		}

		//reset the current game state
		currentState = GameStates_v2_0::INACTIVE;

		//not paused now
		PauseManager::GetInstance().SetPaused(false);

		//pause the background music so it does not double play on next scene
		PauseBGM();
	}

	void GameStateController_v2_0::OnAttach(EntityID id)
	{
		m_scriptData[id] = GameStateController_v2_0Data();

		//clicklist test is substitute for cats
		m_scriptData[id].clicklisttest.resize(5);
		std::fill(m_scriptData[id].clicklisttest.begin(), m_scriptData[id].clicklisttest.end(), static_cast<EntityID>(-1));

		//set the current ID
		m_currentGameStateControllerID = id;
	}

	void GameStateController_v2_0::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
			m_scriptData.erase(id);

	}
	std::map<EntityID, GameStateController_v2_0Data>& GameStateController_v2_0::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance GameStateController_v2_0::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
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

		if (KTE.keycode == GLFW_KEY_F4)
		{
			NextStage(1);
		}

		if (KTE.keycode == GLFW_KEY_F11)
		{
			NextState();
		}

		if (KTE.keycode == GLFW_KEY_F6)
		{
			m_currentLevel++;
		}

		if (KTE.keycode == GLFW_KEY_F5)
		{
			m_currentLevel--;
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
				CatController_v2_0* CatManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
				//for rats
				for (auto [RatID, RatType] : RatManager->GetRats(RatManager->mainInstance))
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
							m_isRat = true;
							m_isPotraitShowing = true;
							//debug
							std::cout << "Clicked on: " << EntityManager::GetInstance().Get<EntityDescriptor>(RatID).name << std::endl;
							//add a switch statement here
							//need specific texture
							m_lastSelectedEntity = RatID;
							//set rat portrait active
							ActiveObject(m_scriptData[m_currentGameStateControllerID].RatPortrait);
							DeactiveObject(m_scriptData[m_currentGameStateControllerID].CatPortrait);
							switch (RatType)
							{
							case 0: //GUTTER
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Rat_Gutter_256px.png"));
								SetPortraitInformation("UnitPortrait_RatNameFrame_GutterRat_239x82.png", 0, 3, 1);

								break;
							case 1: //BRAWLER
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Rat_Brawler_256px.png"));
								SetPortraitInformation("UnitPortrait_RatNameFrame_BrawlerRatRat_239x82.png", 0, 3, 1);
								break;
							case 2: //SNIPER
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Rat_Sniper_256px.png"));
								break;
								//case 3: //HERALD
									//break;
							}
							return;
						}
						else
						{
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(m_defaultPotraitTextureKey);
							//set other portrait stuff inactive
							for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentGameStateControllerID].RatPortrait).children)
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
							DeactiveObject(m_scriptData[m_currentGameStateControllerID].RatPortrait);
							m_isPotraitShowing = false;
						}
					}
				}

				//repeat for cats
				for (auto [CatID, CatType] : CatManager->GetCurrentCats(CatManager->mainInstance))
				{
					if (EntityManager::GetInstance().Has<Transform>(CatID) && EntityManager::GetInstance().Has<Collider>(CatID))
					{
						//get mouse position
						vec2 cursorPosition{};
						GetMouseCurrentPosition(cursorPosition);
						//Get collider of the rat
						CircleCollider const& col = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(CatID).colliderVariant);
						//debug
						std::cout << cursorPosition.x << " " << cursorPosition.y << std::endl;
						// Check if the rat/cat has been clicked
						if (PointCollision(col, cursorPosition))
						{
							m_isRat = false;
							m_isPotraitShowing = true;
							m_lastSelectedEntity = CatID;
							//debug
							std::cout << "Clicked on: " << EntityManager::GetInstance().Get<EntityDescriptor>(CatID).name << std::endl;
							//add a switch statement here
							//need specific texture
							//set cat portrait active
							ActiveObject(m_scriptData[m_currentGameStateControllerID].CatPortrait);
							DeactiveObject(m_scriptData[m_currentGameStateControllerID].RatPortrait);
							switch (CatType)
							{
							case EnumCatType::MAINCAT: //
								SetPortraitInformation("UnitPortrait_CatNameFrame_Meowsalot_239x82.png", CatManager->GetCurrentMovementEnergy(CatID), CatManager->GetMaxMovementEnergy(CatID), 0);
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Cat_Meowsalot_256px.png"));
								break;
							case EnumCatType::GREYCAT: //
								SetPortraitInformation("UnitPortrait_CatNameFrame_GreyCat_239x82.png", CatManager->GetCurrentMovementEnergy(CatID), CatManager->GetMaxMovementEnergy(CatID), 0);
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Cat_Grey_256px.png"));
								break;
							case EnumCatType::ORANGECAT: //
								SetPortraitInformation("UnitPortrait_CatNameFrame_OrangeCat_239x82.png", CatManager->GetCurrentMovementEnergy(CatID), CatManager->GetMaxMovementEnergy(CatID), 0);
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Cat_Orange_256px.png"));
								break;
							}
							return;
						}
						else
						{
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(m_defaultPotraitTextureKey);
							//set other portrait stuff inactive
							for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentGameStateControllerID].CatPortrait).children)
							{
								if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "CatPotraitName")
								{
									if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id2))
									{
										EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id2).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_NameFrame_Cat_239x82.png"));
									}
									break;
								}
							}
							DeactiveObject(m_scriptData[m_currentGameStateControllerID].CatPortrait);

							if (!m_isRat)
								m_isPotraitShowing = false;
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

			PauseBGM();
			PlayPageAudio();

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
			ResumeBGM();

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

			PlayClickAudio();
			PlayPageAudio();
		}
	}

	void GameStateController_v2_0::ActiveObject(EntityID id)
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

	void GameStateController_v2_0::DeactiveObject(EntityID id)
	{
		//deactive all the children objects first
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

	void GameStateController_v2_0::FadeAllObject(EntityID id, float const alpha)
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

	void GameStateController_v2_0::DeactiveAllMenu()
	{
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].PauseMenuCanvas);
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].HowToPlayCanvas);
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].AreYouSureCanvas);
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].LoseCanvas);
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].WinCanvas);
	}

	void GameStateController_v2_0::NextState(EntityID)
	{
		if (currentState == GameStates_v2_0::PLANNING)
		{
			SetGameState(GameStates_v2_0::EXECUTE);
			PlayClickAudio();
			PlayPhaseChangeAudio();
			ResetPhaseBanner(true);
			if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner))
			{
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner).SetTextureKey(m_exexcutePhaseBanner);
			}
		}
		else if (currentState == GameStates_v2_0::EXECUTE)
		{
			CurrentTurn++;
			SetGameState(GameStates_v2_0::PLANNING);
			PlayClickAudio();
			PlayPhaseChangeAudio();
			ResetPhaseBanner(true);
			if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner))
			{
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner).SetTextureKey(m_planningPhaseBanner);
			}
		}
	}

	void GameStateController_v2_0::WinGame()
	{
		PauseBGM();
		PlayWinAudio();
		SetGameState(GameStates_v2_0::WIN);
		m_winOnce = true;
	}

	void GameStateController_v2_0::LoseGame()
	{
		PauseBGM();
		PlayLoseAudio();
		SetGameState(GameStates_v2_0::LOSE);
		m_loseOnce = true;
	}

	void GameStateController_v2_0::CloseHTP(EntityID)
	{
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].HowToPlayCanvas);
		ActiveObject(m_scriptData[m_currentGameStateControllerID].PauseMenuCanvas);
		PlayClickAudio();
	}

	void GameStateController_v2_0::OpenHTP(EntityID)
	{
		ActiveObject(m_scriptData[m_currentGameStateControllerID].HowToPlayCanvas);
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].PauseMenuCanvas);
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].HowToPlayPageTwo);
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentGameStateControllerID].HowToPlayCanvas).children)
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "pg1")
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
			}
		}
		PlayClickAudio();
	}

	void GameStateController_v2_0::HTPPage2(EntityID)
	{
		ActiveObject(m_scriptData[m_currentGameStateControllerID].HowToPlayPageTwo);
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].HowToPlayPageOne);

		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentGameStateControllerID].HowToPlayCanvas).children)
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
		PlayClickAudio();
		PlayPageAudio();
	}

	void GameStateController_v2_0::HTPPage1(EntityID)
	{
		ActiveObject(m_scriptData[m_currentGameStateControllerID].HowToPlayPageOne);
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].HowToPlayPageTwo);

		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentGameStateControllerID].HowToPlayCanvas).children)
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
		PlayClickAudio();
		PlayPageAudio();
	}

	void GameStateController_v2_0::PlanningStateHUD(EntityID const id, float deltaTime)
	{
		ActiveObject(m_scriptData[id].HUDCanvas);
		ActiveObject(m_scriptData[id].TurnCounterCanvas);

		if (!m_isPotraitShowing)
			DeactiveObject(m_scriptData.at(id).Portrait);

		if (prevState == GameStates_v2_0::EXECUTE)
		{
			m_timeSinceEnteredState = 0;
			m_timeSinceExitedState = m_UIFadeTimer;
		}

		m_timeSinceEnteredState += deltaTime;
		m_timeSinceExitedState -= deltaTime;

		float fadeOutSpeed = std::clamp(m_timeSinceExitedState / m_UIFadeTimer, 0.0f, 1.0f);
		float fadeInSpeed = std::clamp(m_timeSinceEnteredState / m_UIFadeTimer, 0.0f, 1.0f);

		FadeAllObject(m_scriptData[id].HUDCanvas, fadeInSpeed);
		FadeAllObject(m_scriptData[id].ExecuteCanvas, fadeOutSpeed);

		if (fadeInSpeed >= 1)
		{
			DeactiveObject(m_scriptData[id].ExecuteCanvas);
		}
	}

	void GameStateController_v2_0::ExecutionStateHUD(EntityID const id, float deltaTime)
	{
		ActiveObject(m_scriptData[id].ExecuteCanvas);

		if (prevState == GameStates_v2_0::PLANNING)
		{
			m_timeSinceEnteredState = 0;
			m_timeSinceExitedState = m_UIFadeTimer;
		}

		m_timeSinceEnteredState += deltaTime;
		m_timeSinceExitedState -= deltaTime;

		float fadeOutSpeed = std::clamp(m_timeSinceExitedState / m_UIFadeTimer, 0.0f, 1.0f);
		float fadeInSpeed = std::clamp(m_timeSinceEnteredState / m_UIFadeTimer, 0.0f, 1.0f);

		FadeAllObject(m_scriptData[id].HUDCanvas, fadeOutSpeed);
		FadeAllObject(m_scriptData[id].ExecuteCanvas, fadeInSpeed);

		if (fadeInSpeed >= 1)
		{
			DeactiveObject(m_scriptData[id].HUDCanvas);
		}
	}

	void GameStateController_v2_0::TransitionPanelFade(EntityID const id, float deltaTime, bool in)
	{
		ActiveObject(m_scriptData[id].TransitionPanel);

		m_timeSinceTransitionStarted += deltaTime;
		m_timeSinceTransitionEnded -= deltaTime;

		float fadeOutSpeed = std::clamp(m_timeSinceTransitionEnded / m_transitionTimer, 0.0f, 1.0f);
		float fadeInSpeed = std::clamp(m_timeSinceTransitionStarted / m_transitionTimer, 0.0f, 1.0f);

		FadeAllObject(m_scriptData[id].TransitionPanel, in ? fadeOutSpeed : fadeInSpeed);

		if(fadeInSpeed >= 1 && !in)
		{
			m_isTransitioning = false;
			m_isTransitioningIn = true;	
			LogicSystem::restartingScene = true;
			LoadSceneFunction(m_leveltoLoad);

			m_timeSinceTransitionStarted = 0;
			m_timeSinceTransitionEnded = m_transitionTimer;
		}
		else if(fadeOutSpeed >= 1 && in)
		{
			m_isTransitioning = false;
			m_isTransitioningIn = false;
			DeactiveObject(m_scriptData[m_currentGameStateControllerID].TransitionPanel);

			m_timeSinceTransitionStarted = 0;
			m_timeSinceTransitionEnded = m_transitionTimer;
		}
	}

	void GameStateController_v2_0::PhaseBannerTransition(EntityID const id, float deltaTime)
	{
		if (m_isPhaseBannerTransition)
		{
			ActiveObject(EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].PhaseBanner).parent.value());

			float fadeInSpeed = std::clamp(m_phaseBannerEnter / m_phaseBannerTransitionTimer, 0.0f, 1.0f);
			FadeAllObject(m_scriptData[id].PhaseBanner, fadeInSpeed);

			if (fadeInSpeed >= 1)
			{
				m_phaseBannerStay += deltaTime;

				if (m_phaseBannerStay >= m_phaseBannerStayTimer)
				{
					float fadeOutSpeed = std::clamp(m_phaseBannerExit / m_transitionTimer, 0.0f, 1.0f);
					FadeAllObject(m_scriptData[id].PhaseBanner, fadeOutSpeed);
					if (fadeOutSpeed <= 0)
					{
						ResetPhaseBanner(false);
					}
					else
						m_phaseBannerExit -= deltaTime;
				}
			}
			else
				m_phaseBannerEnter += deltaTime;
		}
	}

	void GameStateController_v2_0::ResetPhaseBanner(bool SetPhaseBannerTransitionOn)
	{
		m_isPhaseBannerTransition = SetPhaseBannerTransitionOn;
		m_phaseBannerStay = 0;
		m_phaseBannerEnter = 0;
		m_phaseBannerExit = m_phaseBannerTransitionTimer;
		if(EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentGameStateControllerID].PhaseBanner).parent.has_value())
		DeactiveObject(EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentGameStateControllerID].PhaseBanner).parent.value());
		if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner))
		{
			EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner).SetAlpha(0);
		}
	}

	void GameStateController_v2_0::ReturnFromAYS(EntityID)
	{
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].AreYouSureCanvas);
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].AreYouSureRestartCanvas);
		switch (currentState)
		{
		case GameStates_v2_0::PAUSE:
			ActiveObject(m_scriptData[m_currentGameStateControllerID].PauseMenuCanvas);
			break;
		case GameStates_v2_0::WIN:
			ActiveObject(m_scriptData[m_currentGameStateControllerID].WinCanvas);
			break;
		case GameStates_v2_0::LOSE:
			ActiveObject(m_scriptData[m_currentGameStateControllerID].LoseCanvas);
			break;
		}
	}

	void GameStateController_v2_0::OpenAYS(EntityID)
	{
		DeactiveAllMenu();
		ActiveObject(m_scriptData[m_currentGameStateControllerID].AreYouSureCanvas);
		PlayNegativeFeedback();
	}

	void GameStateController_v2_0::OpenAYSR(EntityID)
	{
		DeactiveAllMenu();
		ActiveObject(m_scriptData[m_currentGameStateControllerID].AreYouSureRestartCanvas);
		PlayClickAudio();
	}

	void GameStateController_v2_0::RetryStage(EntityID)
	{
		m_isTransitioning = true;
		m_isTransitioningIn = false;
		m_timeSinceTransitionStarted = 0;
		m_timeSinceTransitionEnded = m_transitionTimer;
		PlayClickAudio();
		PlaySceneTransition();

		m_leveltoLoad = SceneManager::GetInstance().GetActiveScene();
	}

	void GameStateController_v2_0::NextStage(int nextStage)
	{
		m_isTransitioning = true;
		m_isTransitioningIn = false;
		m_timeSinceTransitionStarted = 0;
		m_timeSinceTransitionEnded = m_transitionTimer;

		PlaySceneTransition();
		m_currentLevel = nextStage;
		m_leveltoLoad = m_level2SceneName;

		
	}

	void GameStateController_v2_0::StartGameLoop()
	{
		if (currentState == GameStates_v2_0::DEPLOYMENT)
		{
			currentState = GameStates_v2_0::PLANNING;
			prevState = GameStates_v2_0::DEPLOYMENT;

			//play transition sound
			PlayPhaseChangeAudio();
			ResetPhaseBanner(true);
			if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner))
			{
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner).SetTextureKey(m_planningPhaseBanner);
			}
		}
	}

	void GameStateController_v2_0::RestartGame(EntityID)
	{
		m_currentLevel = 0;
		m_isTransitioning = true;
		m_isTransitioningIn = false;

		CurrentTurn = 0;
		m_leveltoLoad = m_level1SceneName;

		PlayClickAudio();
		PlaySceneTransition();
	}

	int GameStateController_v2_0::GetCurrentLevel()
	{
		return m_currentLevel;
	}

	bool GameStateController_v2_0::GetSelectedCat(EntityID& catID)
	{
		if (!m_isRat && m_isPotraitShowing)
		{
			catID = m_lastSelectedEntity;
			return true;
		}
		return false;
	}

	void GameStateController_v2_0::GetMouseCurrentPosition(vec2& Output)
	{
		InputSystem::GetCursorViewportPosition(WindowManager::GetInstance().p_currWindow, Output.x, Output.y);
		Output = GETCAMERAMANAGER()->GetWindowToWorldPosition(Output.x, Output.y);
	}

	void GameStateController_v2_0::SetPortraitInformation(const std::string_view TextureName, int Current, int Max, bool isRat)
	{
		if(isRat)
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentGameStateControllerID].RatPortrait).children)
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "RatPortraitName")
			{
				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id2))
				{
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id2).SetTextureKey(ResourceManager::GetInstance().LoadTexture(TextureName.data()));
				}
				break;
			}

			if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "RatHealthBar")
			{
				if (EntityManager::GetInstance().Has<GUISlider>(id2))
				{
					//get value from specific rat
					EntityManager::GetInstance().Get<GUISlider>(id2).m_maxValue = static_cast<float>(Max);
					EntityManager::GetInstance().Get<GUISlider>(id2).m_currentValue = static_cast<float>(Current);
				}
			}
		}
		else
		for (auto catid : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentGameStateControllerID].CatPortrait).children)
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(catid).name == "CatPortraitName")
			{
				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(catid))
				{
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(catid).SetTextureKey(ResourceManager::GetInstance().LoadTexture(TextureName.data()));
				}
				break;
			}

			if (EntityManager::GetInstance().Get<EntityDescriptor>(catid).name == "CatEnergyBar")
			{
				if (EntityManager::GetInstance().Has<GUISlider>(catid))
				{
					//get value from specific rat
					EntityManager::GetInstance().Get<GUISlider>(catid).m_maxValue = static_cast<float>(Max);
					EntityManager::GetInstance().Get<GUISlider>(catid).m_currentValue = static_cast<float>(Current);
				}
			}
		}
	}

	void GameStateController_v2_0::LoadSceneFunction(std::string levelname)
	{
		SceneManager::GetInstance().LoadScene(levelname);
	}

	void GameStateController_v2_0::PlayClickAudio()
	{
		EntityID buttonpress = m_serializationManager.LoadFromFile("AudioObject/Button Click SFX_Prefab.json");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}

	void GameStateController_v2_0::PlayNegativeFeedback()
	{
		EntityID buttonpress = m_serializationManager.LoadFromFile("AudioObject/Negative Feedback_Prefab.json");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}

	void GameStateController_v2_0::PlayPageAudio()
	{
		EntityID sound = m_serializationManager.LoadFromFile("AudioObject/Menu Transition SFX_Prefab.json");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateController_v2_0::PlayWinAudio()
	{
		EntityID sound = m_serializationManager.LoadFromFile("AudioObject/Game Win SFX_Prefab.json");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateController_v2_0::PlayLoseAudio()
	{
		EntityID sound = m_serializationManager.LoadFromFile("AudioObject/Game Lose SFX_Prefab.json");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateController_v2_0::PlaySceneTransition()
	{
		EntityID sound = m_serializationManager.LoadFromFile("AudioObject/Scene Transition SFX_Prefab.json");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateController_v2_0::PlayPhaseChangeAudio()
	{
		EntityID sound = m_serializationManager.LoadFromFile("AudioObject/Phase Transition SFX_Prefab.json");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateController_v2_0::PauseBGM()
	{
		EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Background Music_Prefab.json");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
			EntityManager::GetInstance().Get<AudioComponent>(bgm).PauseSound();
		EntityManager::GetInstance().RemoveEntity(bgm);

		EntityID bga = m_serializationManager.LoadFromFile("AudioObject/Background Ambience_Prefab.json");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bga))
			EntityManager::GetInstance().Get<AudioComponent>(bga).PauseSound();
		EntityManager::GetInstance().RemoveEntity(bga);
	}

	void GameStateController_v2_0::ResumeBGM()
	{
		EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Background Music_Prefab.json");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
			EntityManager::GetInstance().Get<AudioComponent>(bgm).ResumeSound();
		EntityManager::GetInstance().RemoveEntity(bgm);

		EntityID bga = m_serializationManager.LoadFromFile("AudioObject/Background Ambience_Prefab.json");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bga))
			EntityManager::GetInstance().Get<AudioComponent>(bga).ResumeSound();
		EntityManager::GetInstance().RemoveEntity(bga);
	}

	void GameStateController_v2_0::UpdateTurnCounter(std::string currentphase)
	{
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentGameStateControllerID].TurnCounterCanvas).children)
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
					EntityManager::GetInstance().Get<TextComponent>(id2).SetText(currentphase);
				}
				continue;
			}
		}
	}



}
