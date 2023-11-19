#pragma once
#include "Singleton.h"
#include "Graphics/CameraManager.h"
#include "Graphics/GLHeaders.h"
#include "GUISystem.h"
namespace PE {
	enum class GameStates {MOVEMENT, ATTACK, EXECUTE, PAUSE, WIN, LOSE };

	class GameStateManager : public Singleton<GameStateManager>
	{
		// ----- Singleton ----- // 
		friend class Singleton<GameStateManager>;

		//public constructors
	public:
		GameStateManager();
		~GameStateManager();

		//public mutators and accessors
	public:
		//general gamestate loops goes from movement to attack to execcute back to movement
		//pause win and lose are extra game state that will be manually
		void SetGameState(GameStates gameState);
		void SetPauseState(EntityID);
		void SetWinState();
		void SetLoseState();
		GameStates GetGameState();
		//public functions
	public:
		void IncrementGameState(EntityID);
		void DecrementGameState(EntityID);
		void ResumeState(EntityID);
		void RegisterButtonFunctions();
		//public variables
	public:
		Graphics::CameraManager* p_cameraManager;
		GLFWwindow* p_window;

	private:
		//start with pause i guess?, might want to change if youre testing stuff
		GameStates m_currentGameState {GameStates::PAUSE};
		//for resume state to set back to
		GameStates m_prevGameState {GameStates::MOVEMENT};
	};

}