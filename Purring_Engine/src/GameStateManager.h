/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GameStateManager.h
 \date     19-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Definition for the class GameStateManager that Handle the Gamestate of the game

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Singleton.h"
#include "Graphics/CameraManager.h"
#include "Graphics/GLHeaders.h"
#include "GUISystem.h"
#include "Data/SerializationManager.h"

namespace PE {
	enum class GameStates {MOVEMENT, ATTACK, EXECUTE, PAUSE, WIN, LOSE, SPLASHSCREEN, INACTIVE };

	class GameStateManager : public Singleton<GameStateManager>
	{
		// ----- Singleton ----- // 
		friend class Singleton<GameStateManager>;

		// ----- Public Constructors ----- // 
	public:
		/*!***********************************************************************************
		 \brief Constructor for the GameStateManager
		*************************************************************************************/
		GameStateManager();
		/*!***********************************************************************************
		 \brief Destructor for the Input System
		*************************************************************************************/
		~GameStateManager();

		// ----- Public Getters/Setters ----- // 
	public:
		/*!***********************************************************************************
		 \brief			Set the gamestate manually
		 \param [In]	The GameState to set to
		*************************************************************************************/
		void SetGameState(GameStates gameState);
		/*!***********************************************************************************
		 \brief			Set gamestate to pause and save previous state
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void SetPauseState(EntityID = -1);
		/*!***********************************************************************************
		 \brief			Set gamestate to Win and save previous state
		*************************************************************************************/
		void SetWinState();
		/*!***********************************************************************************
		 \brief			Set gamestate to Lose and save previous state
		*************************************************************************************/
		void SetLoseState();		
		/*!***********************************************************************************
		 \brief			Set current gamestate to pause and previous gamestate to movement
		*************************************************************************************/
		void ResetDefaultState();
		/*!***********************************************************************************
		 \brief			Sets the current turn number
		 \param [in]	int Number to set the turn number to
		*************************************************************************************/
		void SetTurnNumber(int number);
		/*!***********************************************************************************
		 \brief			Get the previous game state
		 \return		The previous game state
		*************************************************************************************/
		GameStates GetPreviousGameState();
		/*!***********************************************************************************
		 \brief			Get the current game state
		 \return		The current game state
		*************************************************************************************/
		GameStates GetGameState();		
		/*!***********************************************************************************
		 \brief			Get the current turn number
		 \return		The current turn number
		*************************************************************************************/
		int GetTurnNumber();
		/*!***********************************************************************************
		 \brief			True if the how to play menu is active, false otherwise
		 \return		True if the how to play menu is active, false otherwise
		*************************************************************************************/
		inline bool GetHowToPlayActive() { return howToPlay; }

		// ----- Public Functions ----- // 
	public:
		/*!***********************************************************************************
		 \brief			Increment the current gamestate between the 3 game loop states (Movement, Attack and Execute)
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void IncrementGameState(EntityID =-1);
		/*!***********************************************************************************
		 \brief			Decrement the current gamestate between the 3 game loop states (Movement, Attack and Execute)
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void DecrementGameState(EntityID = -1);
		/*!***********************************************************************************
		 \brief			resume from pause state to previous state
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void ResumeState(EntityID = -1);
		/*!***********************************************************************************
		 \brief			Register the functions that wants to be called by buttons
		*************************************************************************************/
		void RegisterButtonFunctions();

		/*!***********************************************************************************
		 \brief			open how to play window
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void HowToPlay(EntityID = -1);

		/*!***********************************************************************************
		 \brief			return to pause menu from hwo to play 
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void ReturnToPauseMenuFromHowToPlay(EntityID = -1);

		/*!***********************************************************************************
		 \brief			return to pause menu from are you sure
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void ReturnToPauseMenuFromExit(EntityID = -1);

		/*!***********************************************************************************
		 \brief			Reloads the game scene and resets the values of the game state manager
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void RestartGame(EntityID);

		/*!***********************************************************************************
		 \brief			Helper function to return to the win/lose screen from the quit 
								confirmation menu.
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void ReturnToWinLoseScreenFromExit(EntityID);

		/*!***********************************************************************************
		 \brief			enter confirmation prompt to exit
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void AreYouSureExit(EntityID = -1);

		/*!***********************************************************************************
		 \brief			enter confirmation prompt to exit from the win/lose screen
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void WLAreYouSureExit(EntityID = -1);

		/*!***********************************************************************************
		 \brief			exit the game
		 \param [In]	EntityID needed to save as a button function
		*************************************************************************************/
		void ExitGame(EntityID = -1);

		/*!***********************************************************************************
		 \brief			Toggle the win screen
		 \param [In]	True to enable the screen, false to disable.
		*************************************************************************************/
		void ToggleWin(bool);

		/*!***********************************************************************************
		 \brief			Toggle the lose screen
		 \param [In]	True to enable the screen, false to disable.
		*************************************************************************************/
		void ToggleLose(bool);

		/*!***********************************************************************************
		 \brief			Plays the sound effect for button presses
		*************************************************************************************/
		void ButtonPressSound();

		/*!***********************************************************************************
		 \brief			Plays the sound effect for phase transitions
		*************************************************************************************/
		void PhaseTransitionSound();

		/*!***********************************************************************************
		 \brief			Plays the sound effect for menu transitions
		*************************************************************************************/
		void MenuTransitionSound();

		/*!***********************************************************************************
		 \brief			Set the entire pause menu to inactive
		*************************************************************************************/
		void InactiveMenu();

		/*!***********************************************************************************
		 \brief			Set the entire pause menu except for the black background to inactive
		*************************************************************************************/
		void InactiveMenuButtons();

		/*!***********************************************************************************
		 \brief			Delete the entire pause menu UI
		*************************************************************************************/
		void DeleteMenu();

		/*!***********************************************************************************
		 \brief			Set the entire pause menu except for the black background to active
		*************************************************************************************/
		void ActiveMenuButtons();

		// ----- Public Variables ----- // 
	public:
		bool godMode{}, noPause{}; // boolean for cheatcode

		// ----- Private Variables ----- // 
	private:
		// State that is currently active
		GameStates m_currentGameState {GameStates::INACTIVE};
		// for resume state to set back to
		GameStates m_prevGameState {GameStates::MOVEMENT};

		SerializationManager serializationManager;
		
		//pause menu
		EntityID pauseBGID,pawsedID,resumeButtonID,howToPlayButtonID,quitButtonID;

		//how to play page
		EntityID howToPlayID,returnButtonID;

		//are you sure page
		EntityID areYouSureID, yesButtonID, noButtonID, sadCatID;

		//lose page
		EntityID loseTextID, loseCatID;

		//win page
		EntityID winTextID, winCatID;

		//Reused
		EntityID endGameRestartButtonID, endGameExitButtonID, endGameBGID;

		bool howToPlay{ false }, pausedOnce{ false }, areYouSure{ false }, Won{ false }, Lost{ false };

		int m_turnNumber{}; // Number of turns that the player has gone through since the beginning of the game
	};
}