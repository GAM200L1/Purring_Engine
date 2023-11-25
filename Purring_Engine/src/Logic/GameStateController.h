/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     GameStateController.h
 \date     23-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan\@digipen.edu


 \brief  This file contains the script that interfaces with the GameStateManager.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "Data/SerializationManager.h"

namespace PE
{
	struct GameStateControllerData
	{
		bool GameStateManagerActive;
		EntityID SplashScreen;
		float SplashTimer{2}; // Time in seconds that the splashscreen is displayed for
		float maxFadeTime{ 2 }; // Time in seconds that it takes the HUD to fade in and out

		// Statement that fades in and out during the execution phase
		EntityID executingStatement;

		// Overlays
		EntityID mapOverlay, pawOverlay, foliageOverlay;

		// Energy levels
		EntityID energyHeader, currentEnergyText, slashText, maxEnergyText;
		EntityID energyBackground;

		// Turn statements
		EntityID turnNumberText, planAttackText, planMovementText;
		EntityID turnBackground;

		// Buttons
		EntityID endTurnButton, endMovementText, endTurnText;
	};

	class GameStateController : public Script
	{
	public:
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
		std::map<EntityID, GameStateControllerData>& GetScriptData();

		/*!***********************************************************************************
		 \brief Get the script data instance for a specific entity (RTTR)

		 \param[in] id - The unique EntityID to retrieve the script data for
		 \return rttr::instance - The script data instance for the specified entity
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);


		// ----- Update HUD UI ----- // 

		/*!***********************************************************************************
		 \brief En/disables the button to end the movement / attack phase and updates the 
						text displayed on the button.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] endMovement - Set to true to display "End Movement" on the button instead of "End Turn"
		*************************************************************************************/
		void EndPhaseButton(EntityID id, bool endMovement);
		
		/*!***********************************************************************************
		 \brief Fades the text in and out over time.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] deltaTime - Time in seconds since the last frame. Used to determine how 
						much to adjust the alpha by.
		*************************************************************************************/
		void UpdateExecuteHUD(EntityID id, float const deltaTime);
		
		/*!***********************************************************************************
		 \brief Fades the text in and out over time.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] turnCount - The turn number to set the turn count text to.
		 \param[in] isMovement - Whether the current state is the movement state.
		*************************************************************************************/
		void UpdateTurnHUD(EntityID id, int const turnCount, bool isMovement);

		/*!***********************************************************************************
		 \brief Toggle the HUD that appears during the planning phase.

		 \param[in] id - ID of the entity that the script instance.
		 \param[in] enable - Enable the planning phase HUD.
		*************************************************************************************/
		void TogglePlanningHUD(EntityID id, bool enable);

		/*!***********************************************************************************
		 \brief toggle the HUD that appears during the execution phase.

		 \param[in] id - ID of the entity that the script instance.
		 \param[in] enable - Enable the execution phase HUD.
		*************************************************************************************/
		void ToggleExecutionHUD(EntityID id, bool enable);

		/*!***********************************************************************************
		 \brief Toggle the HUD that appears during the planning phase.

		 \param[in] id - ID of the entity that the script instance.
		 \param[in] alpha - Alpha to set all the planning HUD elements to.
		*************************************************************************************/
		void FadePlanningHUD(EntityID id, float alpha);

		/*!***********************************************************************************
		 \brief toggle the HUD that appears during the execution phase.

		 \param[in] id - ID of the entity that the script instance.
		 \param[in] alpha - Alpha to set all the execution HUD elements to.
		*************************************************************************************/
		void FadeExecutionHUD(EntityID id, float alpha);


		// ----- Event Callbacks ----- //

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
		 \brief Destructor for the test script

		 \return void
		*************************************************************************************/
		~GameStateController();
	private:
		std::map<EntityID, GameStateControllerData> m_ScriptData;
		SerializationManager serializationManager;
	};


}