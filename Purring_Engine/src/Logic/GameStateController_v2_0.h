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
		PLANNING, DEPLOYMENT, EXECUTE, PAUSE, WIN, LOSE, INACTIVE
	};

	struct GameStateController_v2_0Data
	{
		bool GameStateManagerActive;
		EntityID PauseBackGroundCanvas;
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

		EntityID Journal;
		EntityID RatKingJournal;
		EntityID JournalButton;

		EntityID PhaseBanner;

		EntityID CatPortrait,RatPortrait;
		EntityID Portrait;

		EntityID RatKingPortrait;

		EntityID Background;
		EntityID TransitionPanel;

		EntityID SettingsMenu;

		int NumberInList{5};
		std::vector<EntityID> clicklisttest;

		int keyEventHandlerId, outOfFocusEventHandlerId, mouseClickEventID;
	};

	class GameStateController_v2_0 : public Script
	{
	public:
		/*!***********************************************************************************
		 \brief Default constructor registers ui functions.
		*************************************************************************************/
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
		 \brief			Handle window out of focus event.
		 \param[in]     Event containing window-specific details.
		*************************************************************************************/
		void OnWindowOutOfFocus(const PE::Event<PE::WindowEvents>& r_event);

		/*!***********************************************************************************
		 \brief			Handle Key Press event.
		 \param[in]     Event containing window-specific details.
		*************************************************************************************/
		void OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event);

		/*!***********************************************************************************
		 \brief			Checks if the mouse cursor is within the bounds of any GUI objects

		 \param[in]		r_ME mouse click event details
		*************************************************************************************/
		void OnMouseClick(const Event<MouseEvents>& r_ME);

		// ----- Game State Helper Functions ----- //
	public:
		/*!***********************************************************************************
		 \brief			Set the game state to pause

		 \param[in]		EntityID so that the function can be called by a button
		*************************************************************************************/
		void SetPauseStateV2(EntityID = 0);
		/*!***********************************************************************************
		 \brief			Set the game state

		 \param[in]		The game state to set
		*************************************************************************************/
		void SetGameState(GameStates_v2_0);
		/*!***********************************************************************************
		 \brief			Set the game state to previous state saved

		 \param[in]		EntityID so that the function can be called by a button
		*************************************************************************************/
		void ResumeStateV2(EntityID = 0);
		/*!***********************************************************************************
		 \brief			Swap the state between execution and planning

		 \param[in]		EntityID so that the function can be called by a button
		*************************************************************************************/
		void NextState(EntityID = 0);
		/*!***********************************************************************************
		 \brief			Set the game state to Win and activate the Win Canvas
		*************************************************************************************/
		void WinGame();
		/*!***********************************************************************************
		 \brief			Set the game state to Lose and activate the Lose Canvas
		*************************************************************************************/
		void LoseGame();
		/*!***********************************************************************************
		 \brief			Set the next scene to load to the current scene, and activate transition to restart the scene

		 \param[in]		EntityID so that the function can be called by a button
		*************************************************************************************/
		void RetryStage(EntityID);
		/*!***********************************************************************************
		 \brief			Go to the next stage, with is 2 for now

		 \param[in]		The Next Stage to load
		*************************************************************************************/
		void NextStage(int nextStage);

		/*!***********************************************************************************
		 \brief		Return to main menu
		*************************************************************************************/
		void ReturnToMainMenu(EntityID ID);

		/*!***********************************************************************************
		 \brief			Move from Deployment to Planning
		*************************************************************************************/
		void StartGameLoop();
		/*!***********************************************************************************
		 \brief			Restart the game from the first level

		 \param[in]		EntityID so that the function can be called by a button
		*************************************************************************************/
		void RestartGame(EntityID);

		/*!***********************************************************************************
		 \brief			get level

		 \return	current level
		*************************************************************************************/
		int GetCurrentLevel();

		/*!***********************************************************************************
		 \brief			set level

		 \param[in]		Level to sset
		*************************************************************************************/
		void SetCurrentLevel(int lvl);


		/*!***********************************************************************************
		 \brief		Is Cat Selected

		 \return	true or false if cat selected
		*************************************************************************************/
		bool GetSelectedCat(EntityID catID);

		// ----- Object Helper Functions ----- //
	private:
		/*!***********************************************************************************
		 \brief			Activate the given object and its childrens

		 \param[in]		The objects to set active
		*************************************************************************************/		
		void ActiveObject(EntityID);
		/*!***********************************************************************************
		 \brief			Deactivate the given object and its childrens

		 \param[in]		The objects to set deactivate
		*************************************************************************************/
		void DeactiveObject(EntityID);
		/*!***********************************************************************************
		 \brief			Deactivate all the Pause Menu canvases
		*************************************************************************************/
		void DeactiveAllMenu();
		/*!***********************************************************************************
		 \brief				Get the current mouse position to screen

		 \param[out]		the vec2 to store the mouse position
		*************************************************************************************/
		void GetMouseCurrentPosition(vec2& Output);
		/*!***********************************************************************************
		 \brief			Load a specific level with the given name

		 \param[in]		the level to load
		*************************************************************************************/
		void LoadSceneFunction(std::string levelname);

		// ----- HUD Helper Functions ----- //
	private:
		/*!***********************************************************************************
		 \brief			Set the alpha of all given objects

		 \param[in]		EntityID the canvas to set alpha
		 \param[in]		float the alpha to set
		*************************************************************************************/
		void FadeAllObject(EntityID Canvas, float const alpha);
		/*!***********************************************************************************
		 \brief			Fade the Planning State HUD

		 \param[in]		the current game state manager
		 \param[in]		the time passed since the last update
		*************************************************************************************/
		void PlanningStateHUD(EntityID const id, float deltaTime);
		/*!***********************************************************************************
		 \brief			Fade the Execution State HUD

		 \param[in]		the current game state manager
		 \param[in]		the time passed since the last update
		*************************************************************************************/
		void ExecutionStateHUD(EntityID const id, float deltaTime);
		/*!***********************************************************************************
		 \brief			Fade the Planning State HUD

		 \param[in]		the current game state manager
		 \param[in]		the time passed since the last update
		*************************************************************************************/
		void TransitionPanelFade(EntityID const id, float deltaTime, bool in);

		/*!***********************************************************************************
		 \brief			Fade the phase banner

		 \param[in]		the current game state manager
		 \param[in]		the time passed since the last update
		*************************************************************************************/
		void PhaseBannerTransition(EntityID const id, float deltaTime);

		/*!***********************************************************************************
		 \brief			Reset the phase banner

		 \param[in]		whether it is transitioning
		*************************************************************************************/
		void ResetPhaseBanner(bool SetPhaseBannerTransitionOn);

		/*!***********************************************************************************
		 \brief			Set the Information of the Portrait

		 \param[in]		name of the texture to change the potrait to
		 \param[in]		current value of the slider
		 \param[in]		max value of the slider
		*************************************************************************************/
		void SetPortraitInformation(std::string_view TextureName, int Current, int Max, bool isRat);
		/*!***********************************************************************************
		 \brief			Update the Turn Counter information

		 \param[in]		the current phase to set the text to
		*************************************************************************************/
		void UpdateTurnCounter(std::string currentphase);

		/*!***********************************************************************************
		 \brief			Check whether we should finish execution
		*************************************************************************************/
		void CheckFinishExecution();

		// ----- MENU Helper Functions ----- //
	private:
		/*!***********************************************************************************
		 \brief			Close How To Play Canvas

		 \param[in]		EntityID so that this function can be called by a buttopn
		*************************************************************************************/
		void CloseHTP(EntityID);
		/*!***********************************************************************************
		 \brief			Open How To Play Canvas

		 \param[in]		EntityID so that this function can be called by a buttopn
		*************************************************************************************/
		void OpenHTP(EntityID);
		/*!***********************************************************************************
		 \brief			Change to How To Play Page 2

		 \param[in]		EntityID so that this function can be called by a buttopn
		*************************************************************************************/
		void HTPPage2(EntityID);
		/*!***********************************************************************************
		 \brief			Change to How To Play Page 1

		 \param[in]		EntityID so that this function can be called by a buttopn
		*************************************************************************************/
		void HTPPage1(EntityID);
		/*!***********************************************************************************
		 \brief			Return from the How You Sure Screen

		 \param[in]		EntityID so that this function can be called by a buttopn
		*************************************************************************************/
		void ReturnFromAYS(EntityID);
		/*!***********************************************************************************
		 \brief			Open the Are You Sure Canvas that quits the game

		 \param[in]		EntityID so that this function can be called by a buttopn
		*************************************************************************************/
		void OpenAYS(EntityID);
		/*!***********************************************************************************
		 \brief			Open the Are You Sure Canvas that restarts the game

		 \param[in]		EntityID so that this function can be called by a button
		*************************************************************************************/
		void OpenAYSR(EntityID);
		/*!***********************************************************************************
		 \brief			Function to call when mouse hovers over portrait, enables showing
						of journal

		 \param[in]		EntityID so that this function can be called by a button
		*************************************************************************************/
		void JournalHoverEnter(EntityID);
		/*!***********************************************************************************
		 \brief			Function to call when mouse exits portrait, disables showing of journal

		 \param[in]		EntityID so that this function can be called by a button
		*************************************************************************************/
		void JournalHoverExit(EntityID);

		/*!***********************************************************************************
		 \brief			Open Settings in Main Menu

		 \param[in]		EntityID so that this function can be called by a button
		*************************************************************************************/
		void OpenSettings(EntityID);

		/*!***********************************************************************************
		 \brief			Close Settings in Main Menu

		 \param[in]		EntityID so that this function can be called by a button
		*************************************************************************************/
		void CloseSettings(EntityID);

		// ----- Audio Helper Functions ----- //
	private:
		/*!***********************************************************************************
		 \brief			Play a Button Click Audio
		*************************************************************************************/
		void PlayClickAudio();
		/*!***********************************************************************************
		 \brief			Play a Negative Button Click Audio
		*************************************************************************************/
		void PlayNegativeFeedback();
		/*!***********************************************************************************
		 \brief			Play a Page Flip Audio
		*************************************************************************************/
		void PlayPageAudio();
		/*!***********************************************************************************
		 \brief			Play a Page Flip Audio
		*************************************************************************************/
		void PlayWinAudio();
		/*!***********************************************************************************
		 \brief			Play a Page Flip Audio
		*************************************************************************************/
		void PlayLoseAudio();
		/*!***********************************************************************************
		 \brief			Play a Page Transition Audio
		*************************************************************************************/
		void PlaySceneTransition();
		/*!***********************************************************************************
		 \brief			Play a Phase Change Drum Sound
		 *************************************************************************************/
		void PlayPhaseChangeAudio();
		/*!***********************************************************************************
		 \brief			Pause the BGM
		*************************************************************************************/
		void PauseBGM();
		/*!***********************************************************************************
		 \brief			Resume the BGM
		*************************************************************************************/
		void ResumeBGM();
		/*!***********************************************************************************
		 \brief			Play BGM Ambience and BGM and its respective layers based on stage.
		*************************************************************************************/
		void PlayBackgroundMusicForStage();


	public:
		GameStates_v2_0 currentState = GameStates_v2_0::INACTIVE;
		GameStates_v2_0 prevState = GameStates_v2_0::INACTIVE;
		int currentTurn{1};

	private:
		//Script Variables
		std::map<EntityID, GameStateController_v2_0Data> m_scriptData; // Data associated with each instance of the script
		EntityID m_currentGameStateControllerID;
		SerializationManager m_serializationManager;

		//Stage Variables
		int m_currentLevel{};
		std::string m_level1SceneName{ "Level1Scene.scene" };
		std::string m_level2SceneName{ "Level2Scene.scene" };
		std::string m_level3SceneName{ "Level3Scene.scene" };
		std::string m_level4SceneName{ "Level4Scene.scene" };
		std::string m_mainMenuSceneName{ "MainMenu.scene" };
		std::string m_leveltoLoad{ "Level1Scene.scene" };
		bool m_nextTurnOnce = false;


		//Texture Keys
		std::string m_currentLevelBackground;
		std::string m_currentLevelSepiaBackground;
		std::string m_defaultPotraitTextureKey;
		std::string m_deploymentPhaseBanner, m_planningPhaseBanner, m_exexcutePhaseBanner;

		//Pause and Win/Lose Variables
		bool m_pauseMenuOpenOnce{ false }, m_winOnce{}, m_loseOnce{};
		bool m_isTransitioning{ false };
		bool m_isTransitioningIn{ false };
		bool m_splashScreenShown{ false };
	
		//UI Variables
		const float m_UIFadeTimer{.5f};
		float m_timeSinceEnteredState{ 1.f };
		float m_timeSinceExitedState{};
		const float m_transitionTimer{ .75f };
		float m_timeSinceTransitionStarted{};
		float m_timeSinceTransitionEnded{};

		//portrait
		bool m_isPotraitShowing{};
		bool m_isRat{};
		bool m_bossRatSelected{};
		EntityID m_lastSelectedEntity{};
		std::string nextPortraitTexture{};

		//journal object testing
		bool m_journalShowing{};

		//phase banner
		const float m_phaseBannerTransitionTimer{ .2f };
		const float m_phaseBannerStayTimer{ .75f };
		float m_phaseBannerEnter{};
		float m_phaseBannerExit{m_phaseBannerTransitionTimer};
		float m_phaseBannerStay{};
		bool m_isPhaseBannerTransition{true};

		float prevVolume;

		// Audio Controls
		bool bgmStarted = false;

	};
}




