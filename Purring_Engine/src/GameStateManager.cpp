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
#include "SceneManager/SceneManager.h"
#include "ResourceManager/ResourceManager.h"

namespace PE 
{
	GameStateManager::GameStateManager()
	{
			// Does nothing
	}

	GameStateManager::~GameStateManager()
	{
			// Does nothing
	}

	void GameStateManager::SetGameState(GameStates gameState)
	{
		if (m_currentGameState != gameState)
			m_prevGameState = m_currentGameState;
		m_currentGameState = gameState;
	}

	void GameStateManager::SetPauseState(EntityID)
	{
		if(!noPause)
		if (m_currentGameState != GameStates::INACTIVE && m_currentGameState != GameStates::PAUSE && m_currentGameState != GameStates::SPLASHSCREEN)
		{
			m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::PAUSE;

			MenuTransitionSound();

			EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Music1.prefab");
			if (EntityManager::GetInstance().Has<AudioComponent>(bgm))
				EntityManager::GetInstance().Get<AudioComponent>(bgm).PauseSound();
			EntityManager::GetInstance().RemoveEntity(bgm);

			//create pause menu here

			if (pausedOnce) {
				pausedOnce = false;
				DeleteMenu();
			}


			//pause menu

			pauseBGID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/pausebg.prefab");
			resumeButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/resumegamebutton.prefab");
			howToPlayButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/howtoplaybutton.prefab");
			quitButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/quitgamebutton.prefab");
			pawsedID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/pawsed.prefab");


			EntityManager::GetInstance().Get<EntityDescriptor>(resumeButtonID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(pauseBGID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(howToPlayButtonID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(quitButtonID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(pawsedID).toSave = false;

			//how to play//

			howToPlayID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/howtoplayobj.prefab");
			returnButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/returnbutton.prefab");


			EntityManager::GetInstance().Get<EntityDescriptor>(howToPlayID).isActive = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(returnButtonID).isActive = false;

			EntityManager::GetInstance().Get<EntityDescriptor>(howToPlayID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(returnButtonID).toSave = false;


			//are you sure

			areYouSureID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/areyousure.prefab");
			yesButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/Yes.prefab");
			noButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/No.prefab");
			sadCatID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/sadcat.prefab");


			EntityManager::GetInstance().Get<EntityDescriptor>(areYouSureID).isActive = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(yesButtonID).isActive = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(noButtonID).isActive = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(sadCatID).isActive = false;

			EntityManager::GetInstance().Get<EntityDescriptor>(areYouSureID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(yesButtonID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(noButtonID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(sadCatID).toSave = false;
		}
	}

	void GameStateManager::SetWinState()
	{
		EntityID bgm;
		switch (m_currentGameState)
		{
			case GameStates::MOVEMENT:
			case GameStates::ATTACK:
			case GameStates::EXECUTE:

				bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Music1.prefab");
				if (EntityManager::GetInstance().Has<AudioComponent>(bgm))
					EntityManager::GetInstance().Get<AudioComponent>(bgm).StopSound();
				EntityManager::GetInstance().RemoveEntity(bgm);

				MenuTransitionSound();

				//win menu
				endGameBGID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/pausebg.prefab");
				winCatID = ResourceManager::GetInstance().LoadPrefabFromFile("WinLoseMenu/HappyCat.prefab");
				winTextID = ResourceManager::GetInstance().LoadPrefabFromFile("WinLoseMenu/youwin.prefab");
				endGameRestartButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("WinLoseMenu/Restart.prefab");
				endGameExitButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("WinLoseMenu/WLQuit.prefab");

				EntityManager::GetInstance().Get<EntityDescriptor>(winCatID).toSave = false;
				EntityManager::GetInstance().Get<EntityDescriptor>(endGameBGID).toSave = false;
				EntityManager::GetInstance().Get<EntityDescriptor>(winTextID).toSave = false;
				EntityManager::GetInstance().Get<EntityDescriptor>(endGameRestartButtonID).toSave = false;
				

				//are you sure
				areYouSureID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/areyousure.prefab");
				yesButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/Yes.prefab");
				noButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("WinLoseMenu/WNo.prefab");
				sadCatID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/sadcat.prefab");


				EntityManager::GetInstance().Get<EntityDescriptor>(areYouSureID).isActive = false;
				EntityManager::GetInstance().Get<EntityDescriptor>(yesButtonID).isActive = false;
				EntityManager::GetInstance().Get<EntityDescriptor>(noButtonID).isActive = false;
				EntityManager::GetInstance().Get<EntityDescriptor>(sadCatID).isActive = false;

				EntityManager::GetInstance().Get<EntityDescriptor>(areYouSureID).toSave = false;
				EntityManager::GetInstance().Get<EntityDescriptor>(yesButtonID).toSave = false;
				EntityManager::GetInstance().Get<EntityDescriptor>(noButtonID).toSave = false;
				EntityManager::GetInstance().Get<EntityDescriptor>(sadCatID).toSave = false;

				m_prevGameState = m_currentGameState;
				m_currentGameState = GameStates::WIN;

				Won = true;
				break;
			default:
				break;
		}
	}

	void GameStateManager::SetLoseState()
	{
		EntityID bgm;
		switch (m_currentGameState)
		{
		case GameStates::MOVEMENT:
		case GameStates::ATTACK:
		case GameStates::EXECUTE:

			bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Music1.prefab");
			if (EntityManager::GetInstance().Has<AudioComponent>(bgm))
				EntityManager::GetInstance().Get<AudioComponent>(bgm).StopSound();
			EntityManager::GetInstance().RemoveEntity(bgm);

			MenuTransitionSound();

			//win menu
			endGameBGID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/pausebg.prefab");
			loseCatID = ResourceManager::GetInstance().LoadPrefabFromFile("WinLoseMenu/SadCat.prefab");
			loseTextID = ResourceManager::GetInstance().LoadPrefabFromFile("WinLoseMenu/youlose.prefab");
			endGameRestartButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("WinLoseMenu/Restart.prefab");
			endGameExitButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("WinLoseMenu/WLQuit.prefab");

			EntityManager::GetInstance().Get<EntityDescriptor>(loseCatID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(endGameBGID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(loseTextID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(endGameRestartButtonID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(endGameExitButtonID).toSave = false;


			//are you sure

			areYouSureID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/areyousure.prefab");
			yesButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/Yes.prefab");
			noButtonID = ResourceManager::GetInstance().LoadPrefabFromFile("WinLoseMenu/LNo.prefab");
			sadCatID = ResourceManager::GetInstance().LoadPrefabFromFile("PauseMenu/sadcat.prefab");


			EntityManager::GetInstance().Get<EntityDescriptor>(areYouSureID).isActive = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(yesButtonID).isActive = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(noButtonID).isActive = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(sadCatID).isActive = false;

			EntityManager::GetInstance().Get<EntityDescriptor>(areYouSureID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(yesButtonID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(noButtonID).toSave = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(sadCatID).toSave = false;

			m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::LOSE;
			Lost = true;
			break;
		default:
			break;
		}
	}

	void GameStateManager::SetTurnNumber(int number)
	{
			m_turnNumber = number;
	}

	void GameStateManager::ResetDefaultState()
	{
			SetTurnNumber(0);
			EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Music1.prefab");
			if (EntityManager::GetInstance().Has<AudioComponent>(bgm))
				EntityManager::GetInstance().Get<AudioComponent>(bgm).StopSound();
			EntityManager::GetInstance().RemoveEntity(bgm);

			m_currentGameState = GameStates::INACTIVE;
			m_prevGameState = GameStates::PAUSE;

			godMode = false;
			noPause = false;
	}

	GameStates GameStateManager::GetPreviousGameState()
	{
		return m_prevGameState;
	}	

	GameStates GameStateManager::GetGameState()
	{
		return m_currentGameState;
	}

	int GameStateManager::GetTurnNumber()
	{
			return m_turnNumber;
	}

	void GameStateManager::IncrementGameState(EntityID)
	{
		if (m_currentGameState == GameStates::MOVEMENT)
		{
			ButtonPressSound();
			PhaseTransitionSound();
			m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::ATTACK;
		}
		else if (m_currentGameState == GameStates::ATTACK)
		{
			ButtonPressSound();
			PhaseTransitionSound();
			m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::EXECUTE;
		}
		else if (m_currentGameState == GameStates::EXECUTE)
		{
			ButtonPressSound();
			PhaseTransitionSound();
			SetTurnNumber(m_turnNumber + 1);
			m_prevGameState = m_currentGameState;
			m_currentGameState = GameStates::MOVEMENT;
		}
	}

	void GameStateManager::DecrementGameState(EntityID)
	{
		if (m_currentGameState == GameStates::MOVEMENT)
		{
			SetTurnNumber(m_turnNumber - 1);
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
			
			ButtonPressSound();

			EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Background Music1.prefab");
			if (EntityManager::GetInstance().Has<AudioComponent>(bgm))
				EntityManager::GetInstance().Get<AudioComponent>(bgm).ResumeSound();
			EntityManager::GetInstance().RemoveEntity(bgm);

			pausedOnce = true;

			InactiveMenu();
		}
	}

	void GameStateManager::ExitGame(EntityID)
	{
		ButtonPressSound();
		glfwSetWindowShouldClose(WindowManager::GetInstance().GetWindow(), GL_TRUE);
	}

	void GameStateManager::ToggleWin(bool b)
	{
		EntityManager::GetInstance().Get<EntityDescriptor>(winCatID).isActive = b;
		EntityManager::GetInstance().Get<EntityDescriptor>(winTextID).isActive = b;
		EntityManager::GetInstance().Get<EntityDescriptor>(endGameRestartButtonID).isActive = b;
		EntityManager::GetInstance().Get<EntityDescriptor>(endGameExitButtonID).isActive = b;
	}

	void GameStateManager::ToggleLose(bool b)
	{
		EntityManager::GetInstance().Get<EntityDescriptor>(loseCatID).isActive = b;
		EntityManager::GetInstance().Get<EntityDescriptor>(loseTextID).isActive = b;
		EntityManager::GetInstance().Get<EntityDescriptor>(endGameRestartButtonID).isActive = b;
		EntityManager::GetInstance().Get<EntityDescriptor>(endGameExitButtonID).isActive = b;
	}

	void GameStateManager::ButtonPressSound()
	{
		EntityID buttonpress = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Button Click SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}

	void GameStateManager::PhaseTransitionSound()
	{
		EntityID sound = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Phase Transition SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateManager::MenuTransitionSound()
	{
		EntityID sound = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Menu Transition SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void GameStateManager::InactiveMenuButtons()
	{
		EntityManager::GetInstance().Get<EntityDescriptor>(resumeButtonID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(howToPlayButtonID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(quitButtonID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(pawsedID).isActive = false;

	}

	void GameStateManager::InactiveMenu()
	{
		EntityManager::GetInstance().Get<EntityDescriptor>(resumeButtonID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(pauseBGID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(howToPlayButtonID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(quitButtonID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(pawsedID).isActive = false;

		if (howToPlay)
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(howToPlayID).isActive = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(returnButtonID).isActive = false;
		}

		if (areYouSure)
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(areYouSureID).isActive = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(yesButtonID).isActive = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(noButtonID).isActive = false;
			EntityManager::GetInstance().Get<EntityDescriptor>(sadCatID).isActive = false;
		}
	}

	void GameStateManager::DeleteMenu()
	{

		EntityManager::GetInstance().RemoveEntity(pauseBGID);
		EntityManager::GetInstance().RemoveEntity(howToPlayButtonID);
		EntityManager::GetInstance().RemoveEntity(quitButtonID);
		EntityManager::GetInstance().RemoveEntity(resumeButtonID);
		EntityManager::GetInstance().RemoveEntity(pawsedID);

		if (howToPlay)
		{
			EntityManager::GetInstance().RemoveEntity(howToPlayID);
			EntityManager::GetInstance().RemoveEntity(returnButtonID);

			howToPlay = false;
		}

		if (areYouSure)
		{
			EntityManager::GetInstance().RemoveEntity(areYouSureID);
			EntityManager::GetInstance().RemoveEntity(yesButtonID);
			EntityManager::GetInstance().RemoveEntity(noButtonID);
			EntityManager::GetInstance().RemoveEntity(sadCatID);

			areYouSure = false;
		}
	}

	void GameStateManager::ActiveMenuButtons()
	{
		EntityManager::GetInstance().Get<EntityDescriptor>(resumeButtonID).isActive = true;
		EntityManager::GetInstance().Get<EntityDescriptor>(pauseBGID).isActive = true;
		EntityManager::GetInstance().Get<EntityDescriptor>(howToPlayButtonID).isActive = true;
		EntityManager::GetInstance().Get<EntityDescriptor>(quitButtonID).isActive = true;
		EntityManager::GetInstance().Get<EntityDescriptor>(pawsedID).isActive = true;
	}

	void GameStateManager::HowToPlay(EntityID)
	{
		ButtonPressSound();
		//set all the 4 buttons inactive
		InactiveMenuButtons();
		//create howtoplay menu here
		EntityManager::GetInstance().Get<EntityDescriptor>(howToPlayID).isActive = true;
		EntityManager::GetInstance().Get<EntityDescriptor>(returnButtonID).isActive = true;

		howToPlay = true;
	}

	void GameStateManager::ReturnToPauseMenuFromHowToPlay(EntityID)
	{
		ButtonPressSound();
		//set all the 4 buttons active
		ActiveMenuButtons();
		//set inactive how to play menu here
		EntityManager::GetInstance().Get<EntityDescriptor>(howToPlayID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(returnButtonID).isActive = false;
		howToPlay = false;
	}

	void GameStateManager::ReturnToPauseMenuFromExit(EntityID)
	{
		ButtonPressSound();
		//set all 4 button active and pawsed
		ActiveMenuButtons();
		//delete yes no and are you sure object
		EntityManager::GetInstance().Get<EntityDescriptor>(areYouSureID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(yesButtonID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(noButtonID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(sadCatID).isActive = false;
	}

	void GameStateManager::RestartGame(EntityID)
	{
		ButtonPressSound();
		ResetDefaultState();
		Won = false;
		Lost = false;

		SceneManager::GetInstance().LoadScene(SceneManager::GetInstance().GetStartScene());
	}

	void GameStateManager::ReturnToWinLoseScreenFromExit(EntityID)
	{
		ButtonPressSound();
		if (Won)
		{
			ToggleWin(true);
		}
		else if (Lost)
		{
			ToggleLose(true);
		}

		//delete yes no and are you sure object
		EntityManager::GetInstance().Get<EntityDescriptor>(areYouSureID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(yesButtonID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(noButtonID).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(sadCatID).isActive = false;
	}

	void GameStateManager::AreYouSureExit(EntityID)
	{
		ButtonPressSound();
		//set pawsed and 4 buttons inactive
		InactiveMenuButtons();
		//create yes no button
		//create are you sure object
		EntityManager::GetInstance().Get<EntityDescriptor>(areYouSureID).isActive = true;
		EntityManager::GetInstance().Get<EntityDescriptor>(yesButtonID).isActive = true;
		EntityManager::GetInstance().Get<EntityDescriptor>(noButtonID).isActive = true;
		EntityManager::GetInstance().Get<EntityDescriptor>(sadCatID).isActive = true;

		areYouSure = true;
	}

	void GameStateManager::WLAreYouSureExit(EntityID)
	{
		ButtonPressSound();
		if (Won)
		{
			ToggleWin(false);
		}
		else if (Lost)
		{
			ToggleLose(false);

		}

		EntityManager::GetInstance().Get<EntityDescriptor>(areYouSureID).isActive = true;
		EntityManager::GetInstance().Get<EntityDescriptor>(yesButtonID).isActive = true;
		EntityManager::GetInstance().Get<EntityDescriptor>(noButtonID).isActive = true;
		EntityManager::GetInstance().Get<EntityDescriptor>(sadCatID).isActive = true;

		areYouSure = true;
	}

	void GameStateManager::RegisterButtonFunctions()
	{
		REGISTER_UI_FUNCTION(SetPauseState, PE::GameStateManager);
		REGISTER_UI_FUNCTION(ResumeState, PE::GameStateManager);
		REGISTER_UI_FUNCTION(IncrementGameState, PE::GameStateManager);
		REGISTER_UI_FUNCTION(DecrementGameState, PE::GameStateManager);
		REGISTER_UI_FUNCTION(WLAreYouSureExit, PE::GameStateManager);
		REGISTER_UI_FUNCTION(ExitGame, PE::GameStateManager);
		REGISTER_UI_FUNCTION(RestartGame, PE::GameStateManager);
		REGISTER_UI_FUNCTION(HowToPlay, PE::GameStateManager);
		REGISTER_UI_FUNCTION(AreYouSureExit, PE::GameStateManager);
		REGISTER_UI_FUNCTION(ReturnToPauseMenuFromHowToPlay, PE::GameStateManager);
		REGISTER_UI_FUNCTION(ReturnToPauseMenuFromExit, PE::GameStateManager);
		REGISTER_UI_FUNCTION(ReturnToWinLoseScreenFromExit, PE::GameStateManager);
	}

}
