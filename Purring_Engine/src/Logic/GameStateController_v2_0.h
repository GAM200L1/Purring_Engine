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
		EntityID BackGroundCanvas;
		EntityID SplashScreen;
		float SplashTimer{ 2.f }; // Time in seconds that the splashscreen is displayed for
		EntityID PauseMenuCanvas;
		EntityID AreYouSureCanvas;
		EntityID LoseCanvas;
		EntityID WinCanvas;
		EntityID HowToPlayPageOne;
		EntityID HowToPlayPageTwo;


		int keyEventHandlerId, outOfFocusEventHandlerId;
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

		// ----- Helper Functions ----- //
	public:
		void SetPauseState();
		void SetGameState(GameStates_v2_0);
		void ResumeStateV2(EntityID=0);
		void ActiveObject(EntityID);
		void DeactiveObject(EntityID);
		void NextState(EntityID);

	public:
		GameStates_v2_0 currentState = GameStates_v2_0::INACTIVE;
		GameStates_v2_0 prevState = GameStates_v2_0::INACTIVE;
	private:
		std::map<EntityID, GameStateController_v2_0Data> m_ScriptData; // Data associated with each instance of the script
		bool m_pauseMenuOpenOnce{false};
	};
}




