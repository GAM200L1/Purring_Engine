/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GameStateManager.h
 \date     19-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Declaration for the class GameStateManager that Handle the Gamestate of the game

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "GameStateManager.h"

namespace PE 
{
	GameStateManager::GameStateManager()
	{
	}

	GameStateManager::~GameStateManager()
	{
	}

	void GameStateManager::SetGameState(GameStates gameState)
	{
		m_prevGameState = m_currentGameState;
		m_currentGameState = gameState;
	}

	void GameStateManager::SetPauseState(EntityID)
	{
		if (m_currentGameState != GameStates::INACTIVE)
		{
			if (m_currentGameState != GameStates::PAUSE)
				m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::PAUSE;

			//create pause menuy here



		}
		//SerializationManager serializationManager;
		//serializationManager.LoadAllEntitiesFromFile("..Assets/Prefabs/PauseMenu_Prefab.json");
		// store the entityIDs of the created pause menu entities to be remove in resumestate
	}

	void GameStateManager::SetWinState()
	{
		if (m_currentGameState != GameStates::WIN)
		m_prevGameState = m_currentGameState;
		m_currentGameState = GameStates::WIN;
	}

	void GameStateManager::SetLoseState()
	{
		if (m_currentGameState != GameStates::LOSE)
		m_prevGameState = m_currentGameState;
		m_currentGameState = GameStates::LOSE;
	}

	GameStates GameStateManager::GetGameState()
	{
		return m_currentGameState;
	}

	void GameStateManager::IncrementGameState(EntityID)
	{
		if (m_currentGameState == GameStates::MOVEMENT)
		{
			m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::ATTACK;
		}
		else if (m_currentGameState == GameStates::ATTACK)
		{
			m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::EXECUTE;
		}
		else if (m_currentGameState == GameStates::EXECUTE)
		{
			m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::MOVEMENT;
		}
	}

	void GameStateManager::DecrementGameState(EntityID)
	{
		if (m_currentGameState == GameStates::MOVEMENT)
		{
			m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::EXECUTE;
		}
		else if (m_currentGameState == GameStates::ATTACK)
		{
			m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::MOVEMENT;
		}
		else if (m_currentGameState == GameStates::EXECUTE)
		{
			m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::ATTACK;
		}
	}

	void GameStateManager::ResumeState(EntityID)
	{
		if (m_currentGameState == GameStates::PAUSE)
		{
			m_currentGameState = m_prevGameState;
			m_prevGameState = GameStates::PAUSE;
			
			//delete pause menu here

		}
	}

	void GameStateManager::ExitGame(EntityID)
	{
		glfwSetWindowShouldClose(p_window, GL_TRUE);
	}

	void GameStateManager::HowToPlay(EntityID)
	{
		//set all the 4 buttons inactive
		
		//create howtoplay menu here

	}

	void GameStateManager::ReturnToPauseMenuFromHowToPlay(EntityID)
	{
		//set all the 4 buttons active

		//delete how to play menu here
	}

	void GameStateManager::ReturnToPauseMenuFromExit(EntityID)
	{
		//set all 4 button active and pawsed

		//delete yes no and are you sure object
	}

	void GameStateManager::AreYouSureExit(EntityID)
	{
		//set pawsed and 4 buttons inactive

		//create yes no button
		//create are you sure object

	}


	void GameStateManager::RegisterButtonFunctions()
	{
		REGISTER_UI_FUNCTION(SetPauseState, PE::GameStateManager);
		REGISTER_UI_FUNCTION(ResumeState, PE::GameStateManager);
		REGISTER_UI_FUNCTION(IncrementGameState, PE::GameStateManager);
		REGISTER_UI_FUNCTION(DecrementGameState, PE::GameStateManager);
		REGISTER_UI_FUNCTION(ExitGame, PE::GameStateManager);
	}






}
