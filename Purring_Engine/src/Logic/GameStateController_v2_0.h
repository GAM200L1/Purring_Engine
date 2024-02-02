/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GameStateController_v2_0.h
 \date     15-01-2024

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Keep Tracks whether the game is paused or needs to be paused.
	Contains the function to unpause the game as well.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "Data/SerializationManager.h"
#include "PauseManager.h"

namespace PE
{
	enum class GameStates_v2_0
	{
		PLANNING, DEPLOYMENT, EXECUTE, PAUSE, WIN, LOSE, SPLASHSCREEN, INACTIVE
	};

	struct GameStateController_v2_0Data
	{
		bool GameStateManagerActive;
		EntityID PauseBackGroundCanvas;
		EntityID SplashScreen;
		float SplashTimer{ 2.f }; // Time in seconds that the splashscreen is displayed for
		EntityID PauseMenuCanvas;
		EntityID AreYouSureCanvas;
		EntityID AreYouSureRestartCanvas;
		EntityID LoseCanvas;
		EntityID WinCanvas;
		EntityID HowToPlayCanvas;
		EntityID HowToPlayPageOne;
		EntityID HowToPlayPageTwo;
		EntityID HUDCanvas;
		EntityID ExecuteCanvas;
		EntityID TurnCounterCanvas;

		EntityID CatPortrait,RatPortrait;
		EntityID Portrait;

		EntityID Background;
		EntityID TransitionPanel;

		int NumberInList{5};
		std::vector<EntityID> clicklisttest;

		int keyEventHandlerId, outOfFocusEventHandlerId, mouseClickEventID;
	};

	class GameStateController_v2_0 : public Script
	{
	public:
		GameStateController_v2_0();
		/*!***********************************************************************************
		 \brief Initialize the test script for an entity (RTTR)

		 \param[in] id - The unique EntityID to initialize the script for
		 \return void
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief Update the test script for an entity (RTTR)

		 \param[in] id - The unique EntityID to update the script for
		 \param[in] deltaTime - The time passed since the last update
		 \return void
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Destroy the test script for an entity (RTTR)

		 \param[in] id - The unique EntityID to destroy the script for
		 \return void
		*************************************************************************************/
		virtual void Destroy(EntityID id);

		/*!***********************************************************************************
		 \brief Attach the test script to an entity (RTTR)

		 \param[in] id - The unique EntityID to attach the script to
		 \return void
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		 \brief Detach the test script from an entity (RTTR)

		 \param[in] id - The unique EntityID to detach the script from
		 \return void
		*************************************************************************************/
		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Get the script data for all entities

		 \return std::map<EntityID, TestScriptData>& - A map of EntityID to TestScriptData
		*************************************************************************************/
		std::map<EntityID, GameStateController_v2_0Data>& GetScriptData();

		/*!***********************************************************************************
		 \brief Get the script data instance for a specific entity (RTTR)

		 \param[in] id - The unique EntityID to retrieve the script data for
		 \return rttr::instance - The script data instance for the specified entity
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);

		// ----- Event Callbacks ----- //
	public:
		/*!***********************************************************************************
		 \brief     Handle window out of focus event.
		 \param     Event containing window-specific details.
		*************************************************************************************/
		void OnWindowOutOfFocus(const PE::Event<PE::WindowEvents>& r_event);

		/*!***********************************************************************************
		 \brief     Handle Key Press event.
		 \param     Event containing window-specific details.
		*************************************************************************************/
		void OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event);

		/*!***********************************************************************************
		 \brief Checks if the mouse cursor is within the bounds of any GUI objects

		 \param[in,out] r_ME mouse click event details
		*************************************************************************************/
		void OnMouseClick(const Event<MouseEvents>& r_ME);

		// ----- Helper Functions ----- //
	public:
		//State Functions
		void SetPauseStateV2(EntityID = 0);
		void SetGameState(GameStates_v2_0);
		void ResumeStateV2(EntityID = 0);
		void NextState(EntityID = 0);
		void WinGame();
		void LoseGame();
		void RetryStage(EntityID);
		void NextStage(int nextStage);
		void StartGameLoop();
		void RestartGame(EntityID);


		//Object Helper Functions
		void ActiveObject(EntityID);
		void DeactiveObject(EntityID);
		void DeactiveAllMenu();
		void GetMouseCurrentPosition(vec2& Output);
		void LoadSceneFunction(std::string levelname);

		//HUD/UI functions
		void FadeAllObject(EntityID Canvas, float const alpha);
		void PlanningStateHUD(EntityID const id, float deltaTime);
		void ExecutionStateHUD(EntityID const id, float deltaTime);
		void ExecuteSplashScreen(EntityID const id, float deltaTime);
		void ExecuteTransition(EntityID const id, float deltaTime, bool in);
		void SetPortraitInformation(std::string_view TextureName, int Current, int Max);
		void UpdateTurnCounter(std::string currentphase);

		//Menu Functions
		void CloseHTP(EntityID);
		void OpenHTP(EntityID);
		void HTPPage2(EntityID);
		void HTPPage1(EntityID);
		void ReturnFromAYS(EntityID);
		void OpenAYS(EntityID);
		void OpenAYSR(EntityID);


		//Audio Functions
		void PlayClickAudio();
		void PlayPageAudio();
		void PlayPhaseChangeAudio();


	public:
		GameStates_v2_0 currentState = GameStates_v2_0::INACTIVE;
		GameStates_v2_0 prevState = GameStates_v2_0::INACTIVE;
		int CurrentTurn{1};
	private:
		//Script Variables
		std::map<EntityID, GameStateController_v2_0Data> m_ScriptData; // Data associated with each instance of the script
		EntityID m_currentGameStateControllerID;
		SerializationManager serializationManager;

		//Stage Variables
		int m_currentLevel{};
		std::string m_level1SceneName{ "Level1Scene.json" };
		std::string m_level2SceneName{ "Level2Scene.json" };
		std::string m_leveltoLoad{ "Level1Scene.json" };

		//Texture Keys
		std::string m_currentLevelBackground;
		std::string m_currentLevelSepiaBackground;
		std::string m_defaultPotraitTextureKey;

		//Pause and Win/Lose Variables
		bool m_pauseMenuOpenOnce{ false }, m_winOnce{}, m_loseOnce{};
		bool m_isTransitioning{ false };
		bool m_isTransitioningIn{ false };
		bool m_splashScreenShown{ false };
	
		//UI Variables
		float m_UIFadeTimer{.5f};
		float m_timeSinceEnteredState{ 1.f };
		float m_timeSinceExitedState{};
		float m_transitionTimer{ 0.5f };
		float m_timeSinceTransitionStarted{};
		float m_timeSinceTransitionEnded{};



	};
}




