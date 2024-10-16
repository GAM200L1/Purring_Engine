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
#include "Cat/FollowScript_v2_0.h"
#include "AudioManager/GlobalMusicManager.h"
#include "Boss/BossRatScript.h"
#include "Animation/Animation.h"
#include "Logic/MainMenuController.h"

#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif

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
		REGISTER_UI_FUNCTION(JournalHoverEnter, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(JournalHoverExit, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(OpenSettings, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(CloseSettings, PE::GameStateController_v2_0);
		REGISTER_UI_FUNCTION(ReturnToMainMenu, PE::GameStateController_v2_0);

		ResetStats();
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

				ResetPhaseBanner(true);
				m_phaseBannerExit = m_phaseBannerTransitionTimer + m_transitionTimer + 1;

				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner))
				{
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].PhaseBanner).SetTextureKey(ResourceManager::GetInstance().LoadTexture("PhaseSplash_Planning_933x302.png"));
				}
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

				ResetPhaseBanner(true);
				m_phaseBannerExit = m_phaseBannerTransitionTimer + m_transitionTimer + 1;

				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].PhaseBanner))
				{
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].PhaseBanner).SetTextureKey(ResourceManager::GetInstance().LoadTexture("PhaseSplash_Deployment_933x302.png"));
				}

			}
		}

		//subscribe to events
			m_scriptData[id].keyEventHandlerId = ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, GameStateController_v2_0::OnKeyEvent, this)
			m_scriptData[id].outOfFocusEventHandlerId = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowLostFocus, GameStateController_v2_0::OnWindowOutOfFocus, this)
			m_scriptData[id].mouseClickEventID = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, GameStateController_v2_0::OnMouseClick, this)
			m_scriptData[id].returnFocusEventHandlerId = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowFocus,GameStateController_v2_0::OnWindowReturnFocus,this)

			//resetting current turn
			currentTurn = 0;
			m_isPotraitShowing = false;
			m_journalStayTime = m_journalStayTimer;
			m_startJournalTimer = false;
			m_journalShowing = false;

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
		m_planningPhaseBanner = ResourceManager::GetInstance().LoadTexture("PhaseSplash_Planning_933x302.png");
		m_deploymentPhaseBanner = ResourceManager::GetInstance().LoadTexture("PhaseSplash_Deployment_933x302.png");
		m_exexcutePhaseBanner = ResourceManager::GetInstance().LoadTexture("PhaseSplash_Execution_933x302.png");

		m_windParticleTexture = ResourceManager::GetInstance().LoadTexture("LeafTwirl.png");
		m_sepiaWindParticleTexture = ResourceManager::GetInstance().LoadTexture("LeafTwirl_Sepia.png");

		m_snowParticleTextures = ResourceManager::GetInstance().LoadTexture("Particle_Snow_512px.png");
		m_sepiaSnowParticleTexture = ResourceManager::GetInstance().LoadTexture("Particle_Snow_Sepia_512px.png");

		PlayBackgroundMusicForStage();

		if (EntityManager::GetInstance().Has<AnimationComponent>(m_scriptData.at(m_currentGameStateControllerID).JournalIcon))
		{
			EntityManager::GetInstance().Get<AnimationComponent>(m_scriptData.at(m_currentGameStateControllerID).JournalIcon).StopAnimation();
		}
		m_nextTurnOnce = false;
	}
	
	void GameStateController_v2_0::Update(EntityID id, float deltaTime)
	{
		GlobalMusicManager::GetInstance().Update(deltaTime);

		if (godMode)
		{
			if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).GodModeText))
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).GodModeText).isActive = true;
			}
		}
		else
		{
			if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).GodModeText))
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).GodModeText).isActive = false;
			}

		}

		if (!m_isRat && m_isPotraitShowing)
		{
			CatScript_v2_0Data* cat = GETSCRIPTDATA(CatScript_v2_0, m_lastSelectedEntity);

			switch (cat->catType)
			{
			case EnumCatType::MAINCAT: //
				nextPortraitTexture = "UnitPortrait_CatNameFrame_Meowsalot_239x82.png";
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Cat_Meowsalot_256px.png"));
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Journal).SetTextureKey(ResourceManager::GetInstance().LoadTexture("CatJournal_Meowsalot_753x402.png"));
				break;
			case EnumCatType::GREYCAT: //
				nextPortraitTexture = "UnitPortrait_CatNameFrame_GreyCat_239x82.png";
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Cat_Grey_256px.png"));
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Journal).SetTextureKey(ResourceManager::GetInstance().LoadTexture("CatJournal_GreyCat_753x402.png"));
				break;
			case EnumCatType::ORANGECAT: //
				nextPortraitTexture = "UnitPortrait_CatNameFrame_OrangeCat_239x82.png";
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Cat_Orange_256px.png"));
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Journal).SetTextureKey(ResourceManager::GetInstance().LoadTexture("CatJournal_OrangeCat_753x402.png"));
				break;
			}

			SetPortraitInformation(nextPortraitTexture, cat->catCurrentEnergy, cat->catMaxMovementEnergy, 0);
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

		if (m_startJournalTimer)
		{
			if (m_journalStayTime >= 0)
			{
				m_journalStayTime -= deltaTime;
			}
			else
			{
				m_journalStayTime = m_journalStayTimer;
				m_startJournalTimer = false;
				m_journalShowing = false;
			}
		}
		else
		{
			m_journalStayTime = m_journalStayTimer;
		}

		//switch statement for the different states
		switch (currentState)
		{
		case GameStates_v2_0::DEPLOYMENT: // deploying cats, need to disable end turn button, but otherwise normal HUD stays
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].Background))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].Background).SetTextureKey(m_currentLevelSepiaBackground);

			if (EntityManager::GetInstance().Has<AnimationComponent>(m_scriptData[m_currentGameStateControllerID].WindParticles))
				EntityManager::GetInstance().Get<AnimationComponent>(m_scriptData[m_currentGameStateControllerID].WindParticles).GetAnimation()->SetSpriteSheetKey(m_sepiaWindParticleTexture);

			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].SnowParticles))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].SnowParticles).SetTextureKey(m_sepiaSnowParticleTexture);

			DeactiveObject(m_scriptData[m_currentGameStateControllerID].PauseBackGroundCanvas);
			DeactiveAllMenu();
			ActiveObject(m_scriptData[id].HUDCanvas);
			ActiveObject(m_scriptData[id].TurnCounterCanvas);
			DeactiveObject(m_scriptData.at(id).Portrait);
			DeactiveObject(m_scriptData.at(id).JournalIcon);
			FadeAllObject(m_scriptData[id].RatKingJournal, 0);
			FadeAllObject(m_scriptData[id].Journal, 0);

			PhaseBannerTransition(id, deltaTime);
			UpdateTurnCounter("Deploying");

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

				if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "UndoButton")
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

			if (EntityManager::GetInstance().Has<AnimationComponent>(m_scriptData[m_currentGameStateControllerID].WindParticles))
				EntityManager::GetInstance().Get<AnimationComponent>(m_scriptData[m_currentGameStateControllerID].WindParticles).GetAnimation()->SetSpriteSheetKey(m_sepiaWindParticleTexture);

			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].SnowParticles))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].SnowParticles).SetTextureKey(m_sepiaSnowParticleTexture);

			PhaseBannerTransition(id, deltaTime);
			UpdateTurnCounter("Planning");
			prevState = currentState;
			break;
		}
		case GameStates_v2_0::EXECUTE: // execute state, fade out HUD and fade in foliage
		{

			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].Background))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].Background).SetTextureKey(m_currentLevelBackground);

			if (EntityManager::GetInstance().Has<AnimationComponent>(m_scriptData[m_currentGameStateControllerID].WindParticles))
				EntityManager::GetInstance().Get<AnimationComponent>(m_scriptData[m_currentGameStateControllerID].WindParticles).GetAnimation()->SetSpriteSheetKey(m_windParticleTexture);

			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].SnowParticles))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[m_currentGameStateControllerID].SnowParticles).SetTextureKey(m_snowParticleTextures);

			DeactiveObject(m_scriptData[m_currentGameStateControllerID].PauseBackGroundCanvas);
			EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(m_defaultPotraitTextureKey);
			DeactiveObject(m_scriptData[m_currentGameStateControllerID].CatPortrait);
			DeactiveObject(m_scriptData[m_currentGameStateControllerID].RatPortrait);
			DeactiveObject(m_scriptData[m_currentGameStateControllerID].RatKingPortrait);
			DeactiveAllMenu();

			ExecutionStateHUD(id, deltaTime);
			PhaseBannerTransition(id, deltaTime);
			UpdateTurnCounter("Executing");
			CheckFinishExecution();
			prevState = currentState;
			break;
		}
		case GameStates_v2_0::WIN: // win state, show win canvas
			if (m_winOnce)
			{
				for (auto id2 : SceneView<GUIButton>())
				{
					if (EntityManager::GetInstance().Has<GUIButton>(id2))
					{
						EntityManager::GetInstance().Get<GUIButton>(id2).disabled = true;
					}
				}
				ActiveObject(m_scriptData[id].PauseBackGroundCanvas);
				ActiveObject(m_scriptData[id].WinCanvas);
				m_winOnce = false;
			}
			break;
		case GameStates_v2_0::LOSE: //lose state, show lose canvas				
			if (m_loseOnce)
			{
				for (auto id2 : SceneView<GUIButton>())
				{
					if (EntityManager::GetInstance().Has<GUIButton>(id2))
					{
						EntityManager::GetInstance().Get<GUIButton>(id2).disabled = true;
					}
				}
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
			REMOVE_WINDOW_EVENT_LISTENER(m_scriptData[id].returnFocusEventHandlerId);
			REMOVE_MOUSE_EVENT_LISTENER(m_scriptData[id].mouseClickEventID);
		}

		//reset the current game state
		currentState = GameStates_v2_0::INACTIVE;

		//not paused now
		PauseManager::GetInstance().SetPaused(false);

		//pause the background music so it does not double play on next scene
		//PauseBGM();
		GlobalMusicManager::GetInstance().StopAllAudio();

		// reset bgm flag
		m_bgmStarted = false;
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
			GETANIMATIONMANAGER()->PauseAllAnimations();
			PauseBGM();
			SetPauseStateV2();
			PauseManager::GetInstance().SetPaused(true);
		}
	}

	void GameStateController_v2_0::OnWindowReturnFocus(const PE::Event<PE::WindowEvents>& r_event)
	{
		if (currentState != GameStates_v2_0::INACTIVE && currentState != GameStates_v2_0::WIN && currentState != GameStates_v2_0::LOSE)
		{
			ResumeBGM();
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

			return;
		}

		if (KTE.keycode == GLFW_KEY_F1)
		{
			//WinGame();
			return;
		}

		if (KTE.keycode == GLFW_KEY_F2)
		{
			LoseGame();
			return;
		}

		if (KTE.keycode == GLFW_KEY_F3)
		{
			godMode = godMode ? false : true;
			return;
		}

		int minNumber = (m_currentLevel < 2) ? 1 : 0;

		if (KTE.keycode == GLFW_KEY_F4)
		{
			if (currentState == GameStates_v2_0::DEPLOYMENT)
				return;


			GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->UpdateCurrentCats(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->mainInstance);
			if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetCurrentCats(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->mainInstance).size() > minNumber)
			NextStage(0);
		}		
		
		if (KTE.keycode == GLFW_KEY_F5)
		{
			if (currentState == GameStates_v2_0::DEPLOYMENT)
				return;

			GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->UpdateCurrentCats(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->mainInstance);
			if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetCurrentCats(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->mainInstance).size() > minNumber)
			NextStage(1);
		}

		if (KTE.keycode == GLFW_KEY_F6)
		{
			if (currentState == GameStates_v2_0::DEPLOYMENT)
				return;

			GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->UpdateCurrentCats(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->mainInstance);
			if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetCurrentCats(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->mainInstance).size() > minNumber)
			NextStage(2);
		}

		if (KTE.keycode == GLFW_KEY_F7)
		{
			if (currentState == GameStates_v2_0::DEPLOYMENT)
				return;

			GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->UpdateCurrentCats(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->mainInstance);
			if(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetCurrentCats(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->mainInstance).size() > minNumber)
			NextStage(3);
		}

		if (KTE.keycode == GLFW_KEY_F8)
		{
			if (m_currentLevel == 3)
			{
				*GETSCRIPTDATA(BossRatScript,GETSCRIPTINSTANCEPOINTER(BossRatScript)->currentBoss).currenthealth = 2;
			}
		}

		if (KTE.keycode == GLFW_KEY_F9)
		{
			NextState();
		}

	}

	void GameStateController_v2_0::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE;
		MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
#endif
		if (MBPE.button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (currentState == GameStates_v2_0::PLANNING)
			{
				RatController_v2_0* RatManager = GETSCRIPTINSTANCEPOINTER(RatController_v2_0);
				CatController_v2_0* CatManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
				BossRatScript* BossRat = GETSCRIPTINSTANCEPOINTER(BossRatScript);
				EntityID BossID = BossRat->currentBoss;

				m_journalStayTime = m_journalStayTimer;
				m_startJournalTimer = false;
				m_journalShowing = false;

				if (EntityManager::GetInstance().Has<AnimationComponent>(m_scriptData.at(m_currentGameStateControllerID).JournalIcon))
				{
					EntityManager::GetInstance().Get<AnimationComponent>(m_scriptData.at(m_currentGameStateControllerID).JournalIcon).StopAnimation();
				}

				//get mouse position
				vec2 cursorPosition{};
				GetMouseCurrentPosition(cursorPosition);

				//for boss
				if (BossID != 0 && m_currentLevel == 3)
				{
					if (EntityManager::GetInstance().Has<Transform>(BossID) && EntityManager::GetInstance().Has<Collider>(BossID))
					{
						//activate ratbossportrait
						//Get collider of the rat
						CircleCollider const& col = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(BossID).colliderVariant);
						if (PointCollision(col, cursorPosition))
						{
							m_isPotraitShowing = true;
							m_bossRatSelected = true;
							m_isRat = true;

							PlayRatBossSelectionSound();

							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Rat_Rat King_256px.png"));
							ActiveObject(m_scriptData[m_currentGameStateControllerID].RatKingPortrait);
							DeactiveObject(m_scriptData[m_currentGameStateControllerID].CatPortrait);
							DeactiveObject(m_scriptData[m_currentGameStateControllerID].RatPortrait);
							return;
						}
						else
						{
							m_isPotraitShowing = false;
							m_bossRatSelected = false;
							m_isRat = false;
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(m_defaultPotraitTextureKey);
							DeactiveObject(m_scriptData[m_currentGameStateControllerID].RatKingPortrait);
						}
					}
				}

				//for rats
				for (auto [RatID, RatType] : RatManager->GetRats(RatManager->mainInstance))
				{
					if (EntityManager::GetInstance().Has<Transform>(RatID) && EntityManager::GetInstance().Has<Collider>(RatID))
					{
						////get mouse position
						//vec2 cursorPosition{};
						//GetMouseCurrentPosition(cursorPosition);

						//Get collider of the rat
						CircleCollider const& col = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(RatID).colliderVariant);

						//debug
						//std::cout << cursorPosition.x << " " << cursorPosition.y << std::endl;

						// Check if the rat/cat has been clicked
						if (PointCollision(col, cursorPosition))
						{
							m_isRat = true;
							m_isPotraitShowing = true;
							//debug
							//std::cout << "Clicked on: " << EntityManager::GetInstance().Get<EntityDescriptor>(RatID).name << "EntityID: "<< RatID << std::endl;
							//add a switch statement here
							//need specific texture
							m_lastSelectedEntity = RatID;
							//set rat portrait active
							ActiveObject(m_scriptData[m_currentGameStateControllerID].RatPortrait);
							DeactiveObject(m_scriptData[m_currentGameStateControllerID].CatPortrait);

							//RatScript_v2_0* RatScript = GETSCRIPTINSTANCEPOINTER(RatScript_v2_0);
							//RatScript* _RatScript = GETSCRIPTINSTANCEPOINTER(RatScript);

							int ratSoundType = 0; // Default sound type

							switch (RatType)
							{
							case EnumRatType::GUTTER_V1: //GUTTER V1 (M3)
							case EnumRatType::GUTTER: //GUTTER
								ratSoundType = 1;
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Rat_Gutter_256px.png"));
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Journal).SetTextureKey(ResourceManager::GetInstance().LoadTexture("RatJournal_GutterRat_753x402.png"));
								SetPortraitInformation("UnitPortrait_RatNameFrame_GutterRat_239x82.png", RatManager->GetRatHealth(RatID), RatManager->GetRatMaxHealth(RatID), true);
								break;
							case EnumRatType::BRAWLER: //BRAWLER
								ratSoundType = 2;
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Rat_Brawler_256px.png"));
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Journal).SetTextureKey(ResourceManager::GetInstance().LoadTexture("RatJournal_BrawlerRat_753x402.png"));
								
								SetPortraitInformation("UnitPortrait_RatNameFrame_BrawlerRatRat_239x82.png", RatManager->GetRatHealth(RatID), RatManager->GetRatMaxHealth(RatID), true);
								break;
							case EnumRatType::SNIPER: //SNIPER
								ratSoundType = 3;
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Rat_Sniper_256px.png"));
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Journal).SetTextureKey(ResourceManager::GetInstance().LoadTexture("RatJournal_SniperRat_753x402.png"));
								SetPortraitInformation("UnitPortrait_RatNameFrame_SniperRat_239x82.png", RatManager->GetRatHealth(RatID), RatManager->GetRatMaxHealth(RatID), true);

								break;
								//case 3: //HERALD
									//break;
							}

							PlayRatSelectionSound(ratSoundType);

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
							m_journalShowing = false;
						}
					}
				}

				//repeat for cats
				for (auto [CatID, CatType] : CatManager->GetCurrentCats(CatManager->mainInstance))
				{
					if (EntityManager::GetInstance().Has<Transform>(CatID) && EntityManager::GetInstance().Has<Collider>(CatID))
					{
						////get mouse position
						//vec2 cursorPosition{};
						//GetMouseCurrentPosition(cursorPosition);
						//Get collider of the rat
						CircleCollider const& col = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(CatID).colliderVariant);
						//debug
						//std::cout << cursorPosition.x << " " << cursorPosition.y << std::endl;
						// Check if the rat/cat has been clicked
						if (PointCollision(col, cursorPosition))
						{
							m_isRat = false;
							m_isPotraitShowing = true;
							m_lastSelectedEntity = CatID;
							
							//ShowSelectionIndicator(CatID);

							//debug
							//std::cout << "Clicked on: " << EntityManager::GetInstance().Get<EntityDescriptor>(CatID).name << std::endl;
							//add a switch statement here
							//need specific texture
							//set cat portrait active
							ActiveObject(m_scriptData[m_currentGameStateControllerID].CatPortrait);
							DeactiveObject(m_scriptData[m_currentGameStateControllerID].RatPortrait);

							int catSoundType = 0;  // Default sound type for cats
							switch (CatType)
							{
							case EnumCatType::MAINCAT: //
								catSoundType = 1;
								nextPortraitTexture = "UnitPortrait_CatNameFrame_Meowsalot_239x82.png";
								SetPortraitInformation("UnitPortrait_CatNameFrame_Meowsalot_239x82.png", CatManager->GetCurrentMovementEnergy(CatID), CatManager->GetMaxMovementEnergy(CatID), 0);
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Cat_Meowsalot_256px.png"));
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Journal).SetTextureKey(ResourceManager::GetInstance().LoadTexture("CatJournal_Meowsalot_753x402.png"));
								break;
							case EnumCatType::GREYCAT: //
								catSoundType = 2;
								nextPortraitTexture = "UnitPortrait_CatNameFrame_GreyCat_239x82.png";
								SetPortraitInformation("UnitPortrait_CatNameFrame_GreyCat_239x82.png", CatManager->GetCurrentMovementEnergy(CatID), CatManager->GetMaxMovementEnergy(CatID), 0);
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Cat_Grey_256px.png"));
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Journal).SetTextureKey(ResourceManager::GetInstance().LoadTexture("CatJournal_GreyCat_753x402.png"));
								break;
							case EnumCatType::ORANGECAT: //
								catSoundType = 3;
								nextPortraitTexture = "UnitPortrait_CatNameFrame_OrangeCat_239x82.png";
								SetPortraitInformation("UnitPortrait_CatNameFrame_OrangeCat_239x82.png", CatManager->GetCurrentMovementEnergy(CatID), CatManager->GetMaxMovementEnergy(CatID), 0);
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Portrait).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_Cat_Orange_256px.png"));
								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].Journal).SetTextureKey(ResourceManager::GetInstance().LoadTexture("CatJournal_OrangeCat_753x402.png"));
								break;
							}

							PlayCatSelectionSound(catSoundType);

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
							{
								m_isPotraitShowing = false;
								m_journalShowing = false;
							}

						}
					}
				}
			}

		}
	}

	void GameStateController_v2_0::SetPauseStateV2(EntityID)
	{
		if (currentState != GameStates_v2_0::PAUSE && currentState != GameStates_v2_0::WIN && currentState != GameStates_v2_0::LOSE)
		{
			prevState = currentState;
			currentState = GameStates_v2_0::PAUSE;

			
			GlobalMusicManager::GetInstance().PauseBackgroundMusic();  // Adjust volume for pausing

			//PauseBGM();
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
			GlobalMusicManager::GetInstance().ResumeBackgroundMusic();  // Restore volume after resuming

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

			//ResumeBGM();
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
		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;
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
		DeactiveObject(m_scriptData[m_currentGameStateControllerID].SettingsMenu);
	}

	void GameStateController_v2_0::NextState(EntityID)
	{
		if (m_currentLevel == 3)
		{
			BossRatScriptData* p_brsd = GETSCRIPTDATA(BossRatScript, GETSCRIPTINSTANCEPOINTER(BossRatScript)->currentBoss);
			if (p_brsd->currenthealth <= 0)
				return;
		}

		if (currentState == GameStates_v2_0::PLANNING && !m_nextTurnOnce)
		{
			currentTurn++;
			SetGameState(GameStates_v2_0::EXECUTE);
			PlayClickAudio();
			PlayPhaseChangeAudio();
			ResetPhaseBanner(true);
			m_nextTurnOnce = true;
			if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner))
			{
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner).SetTextureKey(m_exexcutePhaseBanner);
			}
		}
		else if (currentState == GameStates_v2_0::EXECUTE && !m_nextTurnOnce)
		{
			SetGameState(GameStates_v2_0::PLANNING);
			m_isPotraitShowing = false;
			m_journalShowing = false;
			PlayClickAudio();
			ResetPhaseBanner(true);
			m_nextTurnOnce = true;
			if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner))
			{
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[m_currentGameStateControllerID].PhaseBanner).SetTextureKey(m_planningPhaseBanner);
			}
		}
	}

	void GameStateController_v2_0::WinGame()
	{
		//PauseBGM();
		//GlobalMusicManager::GetInstance().StartFadeOut(0.25f);

		GETANIMATIONMANAGER()->PauseAllAnimations();
		PlayWinAudio();
		PauseBGM();
		SetGameState(GameStates_v2_0::WIN);
		m_winOnce = true;
	}

	void GameStateController_v2_0::LoseGame()
	{
		//PauseBGM();
		//GlobalMusicManager::GetInstance().StartFadeOut(0.25f);

		GETANIMATIONMANAGER()->PauseAllAnimations();
		PlayLoseAudio();
		PauseBGM();
		IncrementDeathCount();

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
		{
			DeactiveObject(m_scriptData.at(id).Portrait);
			DeactiveObject(m_scriptData.at(id).JournalIcon);
		}

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

		if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].JournalButton))
			EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].JournalButton).SetAlpha(0);

		if (!m_journalShowing)
		{
			if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].Journal))
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].Journal).SetAlpha(0);

			if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal))
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal).SetAlpha(0);
		}
		else
		{
			if (m_bossRatSelected)
			{
				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal))
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal).SetAlpha(fadeInSpeed);

				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].Journal))
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].Journal).SetAlpha(0);
			}
			else
			{
				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].Journal))
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].Journal).SetAlpha(fadeInSpeed);

				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal))
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal).SetAlpha(0);
			}
		}


		if (fadeInSpeed >= 1)
		{
			DeactiveObject(m_scriptData[id].ExecuteCanvas);
			m_nextTurnOnce = false;
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

		if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].JournalButton))
			EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].JournalButton).SetAlpha(0);

		if (!m_journalShowing)
		{
			if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].Journal))
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].Journal).SetAlpha(0);

			if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal))
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal).SetAlpha(0);
		}
		else
		{
			if (m_bossRatSelected)
			{
				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal))
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal).SetAlpha(fadeOutSpeed);

				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].Journal))
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].Journal).SetAlpha(0);
			}
			else
			{
				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].Journal))
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].Journal).SetAlpha(fadeOutSpeed);

				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal))
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_scriptData[id].RatKingJournal).SetAlpha(0);
			}
		}


		if (fadeInSpeed >= 1)
		{
			DeactiveObject(m_scriptData[id].HUDCanvas);
			m_nextTurnOnce = false;
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
			if(EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].PhaseBanner).parent.has_value())
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

	void GameStateController_v2_0::JournalHoverEnter(EntityID)
	{
		if (m_isPotraitShowing)
		{
			m_journalShowing = true;

			if (EntityManager::GetInstance().Has<AnimationComponent>(m_scriptData.at(m_currentGameStateControllerID).JournalIcon))
			{
				EntityManager::GetInstance().Get<AnimationComponent>(m_scriptData.at(m_currentGameStateControllerID).JournalIcon).ResetAnimation();
				EntityManager::GetInstance().Get<AnimationComponent>(m_scriptData.at(m_currentGameStateControllerID).JournalIcon).PlayAnimation();
			}

		}
	}

	void GameStateController_v2_0::JournalHoverExit(EntityID)
	{
		m_startJournalTimer = true;
	}

	void GameStateController_v2_0::OpenSettings(EntityID)
	{
		PlayClickAudio();
		DeactiveAllMenu();
		ActiveObject(m_scriptData[m_currentGameStateControllerID].SettingsMenu);
	}

	void GameStateController_v2_0::CloseSettings(EntityID)
	{
		PlayClickAudio();
		DeactiveAllMenu();
		ActiveObject(m_scriptData[m_currentGameStateControllerID].PauseMenuCanvas);
	}

	void GameStateController_v2_0::RetryStage(EntityID)
	{
		m_isTransitioning = true;
		m_isTransitioningIn = false;
		m_timeSinceTransitionStarted = 0;
		m_timeSinceTransitionEnded = m_transitionTimer;

		IncrementTurnCount(currentTurn);

		PlayClickAudio();
		PlaySceneTransition();
		GETANIMATIONMANAGER()->PlayAllAnimations();

		m_bgmStarted = false;

		m_leveltoLoad = SceneManager::GetInstance().GetActiveScene();
	}

	void GameStateController_v2_0::NextStage(int nextStage)
	{
		//GlobalMusicManager::GetInstance().StartFadeOut(0.75f);

		PlaySceneTransition();

		IncrementTurnCount(currentTurn);

		switch (nextStage)
		{
		case 0: // 1st level
		{
			m_isTransitioning = true;
			m_isTransitioningIn = false;
			m_timeSinceTransitionStarted = 0;
			m_timeSinceTransitionEnded = m_transitionTimer;
			m_currentLevel = nextStage;
			m_leveltoLoad = m_level1SceneName;
			break;
		}
		case 1: // 2nd level
		{
			CatController_v2_0* p_catManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
			p_catManager->UpdateCurrentCats(p_catManager->mainInstance);
			p_catManager->UpdateDeployableCats(p_catManager->mainInstance);
			
			m_isTransitioning = true;
			m_isTransitioningIn = false;
			m_timeSinceTransitionStarted = 0;
			m_timeSinceTransitionEnded = m_transitionTimer;

			m_currentLevel = nextStage;
			m_leveltoLoad = m_level2SceneName;
			break;
		}
		case 2: // 3rd level
		{
			CatController_v2_0* p_catManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
			p_catManager->UpdateCurrentCats(p_catManager->mainInstance);
			p_catManager->UpdateDeployableCats(p_catManager->mainInstance);

			m_isTransitioning = true;
			m_isTransitioningIn = false;
			m_timeSinceTransitionStarted = 0;
			m_timeSinceTransitionEnded = m_transitionTimer;

			m_currentLevel = nextStage;
			m_leveltoLoad = m_level3SceneName;
			break;
		}
		case 3: // boss level
		{
			CatController_v2_0* p_catManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
			p_catManager->UpdateCurrentCats(p_catManager->mainInstance);
			p_catManager->UpdateDeployableCats(p_catManager->mainInstance);

			m_isTransitioning = true;
			m_isTransitioningIn = false;
			m_timeSinceTransitionStarted = 0;
			m_timeSinceTransitionEnded = m_transitionTimer;

			m_currentLevel = nextStage;
			m_leveltoLoad = m_level4SceneName;
			break;
		}
		default:
			WinGame();
			m_leveltoLoad = m_mainMenuSceneName;
			break;
		}
	
	}

	void GameStateController_v2_0::GoToOutroCutscene()
	{
		gameEnded = true;
		m_isTransitioning = true;
		m_isTransitioningIn = false;
		m_timeSinceTransitionStarted = 0;
		m_timeSinceTransitionEnded = m_transitionTimer;
		IncrementTurnCount(currentTurn);

		//GlobalMusicManager::GetInstance().StartFadeOut(0.75f);
		m_currentLevel = 0;
		m_leveltoLoad = m_outroCutsceneScene;
	}

	void GameStateController_v2_0::ReturnToMainMenu(EntityID ID)
	{
		m_isTransitioning = true;
		m_isTransitioningIn = false;
		m_timeSinceTransitionStarted = 0;
		m_timeSinceTransitionEnded = m_transitionTimer;

		m_currentLevel = 0;
		m_leveltoLoad = m_mainMenuSceneName;

		ResetStats();

		GETSCRIPTINSTANCEPOINTER(MainMenuController)->NotFirstStart();
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

		currentTurn = 0;
		m_leveltoLoad = m_level1SceneName;

		GETANIMATIONMANAGER()->PlayAllAnimations();
		PlayClickAudio();
		PlaySceneTransition();
	}

	int GameStateController_v2_0::GetCurrentLevel()
	{
		return m_currentLevel;
	}

	void GameStateController_v2_0::SetCurrentLevel(int lvl)
	{
		m_currentLevel = lvl;
	}

	bool GameStateController_v2_0::GetSelectedCat(EntityID catID)
	{
		if (!m_isRat && m_isPotraitShowing)
		{
			if ((GETSCRIPTDATA(CatScript_v2_0, catID))->planningAttack)
				m_lastSelectedEntity = catID;

			if (m_lastSelectedEntity == catID)
				return true;
		}
		return false;
	}

	void GameStateController_v2_0::ResetStats()
	{
		m_catsRescued = 0;
		m_ratsVanquished = 0;
		m_deathCount = 0;
		m_totalTurns = 0;
	}

	void GameStateController_v2_0::IncrementCatRescued()
	{
		++m_catsRescued;
	}

	void GameStateController_v2_0::IncrementRatsKilled()
	{
		++m_ratsVanquished;
	}

	void GameStateController_v2_0::IncrementTurnCount(int count)
	{
		m_totalTurns += count;
	}

	void GameStateController_v2_0::IncrementDeathCount()
	{
		++m_deathCount;
	}

	int GameStateController_v2_0::GetCatRescued()
	{
		return m_catsRescued;
	}

	int GameStateController_v2_0::GetRatsKilled()
	{
		return m_ratsVanquished;
	}

	int GameStateController_v2_0::GetTurnCount()
	{
		return m_totalTurns;
	}

	int GameStateController_v2_0::GetDeathCount()
	{
		return m_deathCount;
	}

	void GameStateController_v2_0::GetMouseCurrentPosition(vec2& Output)
	{
		InputSystem::GetCursorViewportPosition(WindowManager::GetInstance().p_currWindow, Output.x, Output.y);
		Output = GETCAMERAMANAGER()->GetWindowToWorldPosition(Output.x, Output.y);
	}

	void GameStateController_v2_0::SetPortraitInformation(const std::string_view TextureName, int Current, int Max, bool isRat)
	{
		float maxFloat{ static_cast<float>(Max) };
		float healthPercentage = static_cast<float>(Current) / (CompareFloats(0.f, maxFloat) ? 1.f : maxFloat);
		vec4 fillColor;

		if (healthPercentage <= 0.34f)
		{
			fillColor = m_fillColorAlmostEmpty;
		}
		else if (healthPercentage <= 0.67f)
		{
			fillColor = m_fillColorHalf;
		}
		else
		{
			fillColor = m_fillColorFull;
		}

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

					EntityID fillId = EntityManager::GetInstance().Get<GUISlider>(id2).m_knobID.value();

					if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(fillId))
					{
						EntityManager::GetInstance().Get<Graphics::GUIRenderer>(fillId).SetColor(fillColor.x, fillColor.y, fillColor.z, fillColor.w);
					}
				}
			}

			if (Max == 2 && EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "HealthFrame")
			{
				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id2))
				{
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id2).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_HealthBar_TwoSeg_180x46.png"));
				}
			}
			else if (Max == 3 && EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "HealthFrame")
			{
				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id2))
				{
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id2).SetTextureKey(ResourceManager::GetInstance().LoadTexture("UnitPortrait_EnergyBar_ThreeSeg_180x46.png"));
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

			if (GETSCRIPTINSTANCEPOINTER(CatScript_v2_0)->GetScriptData().at(m_lastSelectedEntity).isCaged)
			{
				if (EntityManager::GetInstance().Get<EntityDescriptor>(catid).name == "EnergyFrame")
				{
					DeactiveObject(catid);
				}

				if (EntityManager::GetInstance().Get<EntityDescriptor>(catid).name == "CatEnergyBar")
				{
					DeactiveObject(catid);
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
		EntityID buttonpress = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Button Click SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}

	void GameStateController_v2_0::PlayNegativeFeedback()
	{
		EntityID buttonpress = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Negative Feedback.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}

	void GameStateController_v2_0::PlayPageAudio()
	{
		EntityID sound = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Menu Transition SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateController_v2_0::PlayWinAudio()
	{
		EntityID sound = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Game Win SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateController_v2_0::PlayLoseAudio()
	{
		EntityID sound = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Game Lose SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateController_v2_0::PlaySceneTransition()
	{
		EntityID sound = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Scene Transition SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateController_v2_0::PlayPhaseChangeAudio()
	{
		EntityID sound = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Phase Transition SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateController_v2_0::PauseBGM()
	{
		EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Music1.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
			EntityManager::GetInstance().Get<AudioComponent>(bgm).PauseSound();
		EntityManager::GetInstance().RemoveEntity(bgm);

		EntityID bgm2 = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Music2.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm2))
			EntityManager::GetInstance().Get<AudioComponent>(bgm2).PauseSound();
		EntityManager::GetInstance().RemoveEntity(bgm2);

		EntityID bgm3 = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Music3.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm3))
			EntityManager::GetInstance().Get<AudioComponent>(bgm3).PauseSound();
		EntityManager::GetInstance().RemoveEntity(bgm3);

		EntityID bga = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Ambience.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bga))
			EntityManager::GetInstance().Get<AudioComponent>(bga).PauseSound();
		EntityManager::GetInstance().RemoveEntity(bga);
	}

	void GameStateController_v2_0::ResumeBGM()
	{
		EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Music1.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
			EntityManager::GetInstance().Get<AudioComponent>(bgm).ResumeSound();
		EntityManager::GetInstance().RemoveEntity(bgm);

		EntityID bgm2 = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Music2.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm2))
			EntityManager::GetInstance().Get<AudioComponent>(bgm2).PauseSound();
		EntityManager::GetInstance().RemoveEntity(bgm2);

		EntityID bgm3 = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Music3.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm3))
			EntityManager::GetInstance().Get<AudioComponent>(bgm3).PauseSound();
		EntityManager::GetInstance().RemoveEntity(bgm3);

		EntityID bga = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Ambience.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bga))
			EntityManager::GetInstance().Get<AudioComponent>(bga).ResumeSound();
		EntityManager::GetInstance().RemoveEntity(bga);
	}

	void GameStateController_v2_0::PlayBackgroundMusicForStage()
	{
		if (m_bgmStarted)  // Check if the background music has already been started to avoid restarting it unnecessarily
		{
			return;
		}

		// Defining the audio tracks for each stage
		std::vector<std::vector<std::string>> stageAudio =
		{
			{"AudioObject/Background Ambience.prefab", "AudioObject/Background Music1.prefab"}, // Stage 1
			{"AudioObject/Background Ambience.prefab", "AudioObject/Background Music1.prefab", "AudioObject/Background Music2.prefab"}, // Stage 2
			{"AudioObject/Background Ambience.prefab", "AudioObject/Background Music1.prefab", "AudioObject/Background Music2.prefab", "AudioObject/Background Music3.prefab"}, // Stage 3
			{"AudioObject/Background FinalBoss Music.prefab"}	// Boss fight audio
		};

		if (m_currentLevel >= 0 && m_currentLevel < stageAudio.size())
		{
			// Play each audio track based on the concurrent level
			for (const std::string& track : stageAudio[m_currentLevel])
			{
				GlobalMusicManager::GetInstance().PlayBGM(track, true, 2.5f);
			}
			m_bgmStarted = true; // Flag the background music as started
		}
		else
		{
			//std::cout << "[PlayBackgroundMusicForStage] Invalid level index: " << m_currentLevel << std::endl;

		}
	}

	void GameStateController_v2_0::PlayRatBossSelectionSound()
	{
		int randomSelection = std::rand() % 3 + 1;  // Random number between 1 and 3
		std::string soundPath = "AudioObject/BossAudioPrefab/Rat King Selection SFX" + std::to_string(randomSelection) + ".prefab";
		PE::GlobalMusicManager::GetInstance().PlaySFX(soundPath, false);
	}

	void GameStateController_v2_0::PlayRatSelectionSound(int ratType)
	{
		int randomSelection = std::rand() % 5 + 1;  // Random number between 1 and 5

		std::string ratTypeName;
		switch (ratType)
		{
		case 1:  // '1' for Gutter rats
			ratTypeName = "Gutter";
			break;
		case 2:  // '2' for Brawler rats
			ratTypeName = "Brawler";
			break;
		case 3:  // '3' for Sniper rats
			ratTypeName = "Sniper";
			break;
		default:
			ratTypeName = "Gutter";  // Default to Gutter rat sound if unknown type
			break;
		}

		std::string soundPath = "AudioObject/Rat " + ratTypeName + " Selection SFX" + std::to_string(randomSelection) + ".prefab";
		PE::GlobalMusicManager::GetInstance().PlaySFX(soundPath, false);
	}

	void GameStateController_v2_0::PlayCatSelectionSound(int catType)
	{
		int randomSelection = std::rand() % 5 + 1;  // Random number between 1 and 5

		std::string catTypeName;
		switch (catType)
		{
		case 1:  // '1' for Main cat
			catTypeName = "Meowsalot";
			break;
		case 2:  // '2' for Grey cat
			catTypeName = "Grey";
			break;
		case 3:  // '3' for Orange cat
			catTypeName = "Orange";
			break;
		default:
			catTypeName = "Meowsalot";  // Default to Main cat sound if unknown type
			break;
		}

		std::string soundPath = "AudioObject/Cat " + catTypeName + " Selection SFX" + std::to_string(randomSelection) + ".prefab";
		PE::GlobalMusicManager::GetInstance().PlaySFX(soundPath, false);

	}

	void GameStateController_v2_0::UpdateTurnCounter(std::string currentphase)
	{
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentGameStateControllerID].TurnCounterCanvas).children)
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "TurnNumberText")
			{
				if (EntityManager::GetInstance().Has<TextComponent>(id2))
				{
					EntityManager::GetInstance().Get<TextComponent>(id2).SetText("Turn " + std::to_string(currentTurn));
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

	void GameStateController_v2_0::CheckFinishExecution()
	{
		if (gameEnded)
		{
			return;
		}

		bool Finished = true;

		RatController_v2_0* RatManager = GETSCRIPTINSTANCEPOINTER(RatController_v2_0);
		CatController_v2_0* CatManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);

		for (auto [RatID, RatType] : RatManager->GetRats(RatManager->mainInstance))
		{
			if (RatType == EnumRatType::GUTTER_V1) 
			{
				// Ensures backwards compatability with the V1 rat
				Finished = Finished && GETSCRIPTINSTANCEPOINTER(RatScript)->GetScriptData()[RatID].finishedExecution;
			}
			else 
			{
				// Checks the V2 rat
				Finished = Finished && GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->GetScriptData()[RatID].finishedExecution;
			}
			//std::cout << "GameStateController_v2_0::CheckFinishExecution() RatID " << RatID << " finished exec " << Finished << " \n";
		}

		for (auto [CatID, CatType] : CatManager->GetCurrentCats(CatManager->mainInstance))
		{
			if(!GETSCRIPTINSTANCEPOINTER(CatScript_v2_0)->GetScriptData()[CatID].isCaged)
			Finished = Finished && GETSCRIPTINSTANCEPOINTER(CatScript_v2_0)->GetScriptData()[CatID].finishedExecution;
			//std::cout << "GameStateController_v2_0::CheckFinishExecution() CatID " << CatID << " finished exec " << Finished << " \n";
		}

		Finished = Finished && GETSCRIPTINSTANCEPOINTER(BossRatScript)->m_scriptData[GETSCRIPTINSTANCEPOINTER(BossRatScript)->currentBoss].finishExecution;


		if (Finished)
			NextState();
	}

}
