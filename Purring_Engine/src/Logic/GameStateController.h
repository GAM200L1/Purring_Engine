/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     GameStateController.h
 \date     23-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan\@digipen.edu
 \par      code %:     50%
 \par      changes:    Added functions to control the states + fade the splashscreen in.

 \co-author            Krystal Yamin
 \par      email:      krystal.y\@digipen.edu
 \par      code %:     50%
 \par      changes:    24-11-2023
											 Added functions and logic to update gameplay HUD.


 \brief  This file contains the script that interfaces with the GameStateManager
					and updates the in game HUD.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "Data/SerializationManager.h"
#include "GameStateManager.h"

namespace PE
{
	struct GameStateControllerData
	{
		bool GameStateManagerActive;
		EntityID SplashScreen;
		float SplashTimer{ 2.f }; // Time in seconds that the splashscreen is displayed for
		float maxFadeTime{ 0.5f }; // Time in seconds that it takes the HUD to fade in and out
		float executingFadeSpeed{ 3.f }; // Time in seconds that it takes the executing text to fade in and out
		float timeSinceEnteredState; //Time in seconds since the state has first been entered
		float timeSinceExitedState; //Time in seconds since the state has first been exited
		GameStates prevState; //Previous game state

		// Turn counter
		int turnCounter{};

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

		int keyEventHandlerId, outOfFocusEventHandlerId;
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
		 \brief Updates the movement planning HUD.

		 \param[in] id - ID of the entity this script is associated with.
		 \param[in] deltaTime - Time in seconds since the last frame
		*************************************************************************************/
		void MovementStateHUD(EntityID const id, float deltaTime);

		/*!***********************************************************************************
		 \brief Updates the movement planning HUD.

		 \param[in] id - ID of the entity this script is associated with.
		 \param[in] deltaTime - Time in seconds since the last frame
		*************************************************************************************/
		void AttackStateHUD(EntityID const id);

		/*!***********************************************************************************
		 \brief Updates the movement planning HUD.

		 \param[in] id - ID of the entity this script is associated with.
		 \param[in] deltaTime - Time in seconds since the last frame
		*************************************************************************************/
		void ExecutionStateHUD(EntityID const id, float deltaTime);

		/*!***********************************************************************************
		 \brief En/disables the button to end the movement / attack phase and updates the 
						text displayed on the button.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] endMovement - Set to true to display "End Movement" on the button instead of "End Turn"
		*************************************************************************************/
		void EndPhaseButton(EntityID id, bool endMovement);
		
		/*!***********************************************************************************
		 \brief Fades the "Executing..." text in and out over time.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] deltaTime - Time in seconds since the last frame. Used to determine how 
						much to adjust the alpha by.
		*************************************************************************************/
		void UpdateExecuteHUD(EntityID id, float const deltaTime);
		
		/*!***********************************************************************************
		 \brief Updates the turn count.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] turnCount - The turn number to set the turn count text to.
		 \param[in] isMovement - Whether the current state is the movement state.
		*************************************************************************************/
		void UpdateTurnHUD(EntityID id, int const turnCount, bool isMovement);
		
		/*!***********************************************************************************
		 \brief Updates the energy level UI.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] currentEnergy - Amount of energy the player has left.
		 \param[in] maximumEnergy - Maximum amount of energy the player has.
		*************************************************************************************/
		void UpdateEnergyHUD(EntityID id, int const currentEnergy, int const maximumEnergy);

		/*!***********************************************************************************
		 \brief Toggle the HUD that appears during the planning phase.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] enable - Enable the planning phase HUD.
		*************************************************************************************/
		void TogglePlanningHUD(EntityID id, bool enable);

		/*!***********************************************************************************
		 \brief Toggle the HUD that appears during the execution phase.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] enable - Enable the execution phase HUD.
		*************************************************************************************/
		void ToggleExecutionHUD(EntityID id, bool enable);

		/*!***********************************************************************************
		 \brief Toggle the splashscreen entity.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] enable - Enable the execution phase HUD.
		*************************************************************************************/
		void ToggleSplashscreen(EntityID id, bool enable);

		/*!***********************************************************************************
		 \brief Toggle all text objects in the HUD. 

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] enable - Set to true to enable all text in the HUD, false otherwise.
		*************************************************************************************/
		void ToggleAllText(EntityID id, bool enable);

		/*!***********************************************************************************
		 \brief Toggle the entity passed in.

		 \param[in] id - ID of the entity to toggle.
		 \param[in] enable - Set to true to enable the entity, false otherwise.

		 \return Returns true if the entity was toggled successfully, false if the entity's 
						descriptor component could not be found.
		*************************************************************************************/
		bool ToggleEntity(EntityID id, bool enable);

		/*!***********************************************************************************
		 \brief Adjust the alpha of the HUD that appears during the planning phase.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] alpha - Alpha to set all the planning HUD elements to.
		*************************************************************************************/
		void FadePlanningHUD(EntityID id, float alpha);

		/*!***********************************************************************************
		 \brief Adjust the alpha of the HUD that appears during the execution phase.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] alpha - Alpha to set all the execution HUD elements to.
		*************************************************************************************/
		void FadeExecutionHUD(EntityID id, float alpha);

		/*!***********************************************************************************
		 \brief Adjust the alpha of the splashscreen UI.

		 \param[in] id - ID of the entity that the script instance is attached to.
		 \param[in] alpha - Alpha to set splashscreen to.
		*************************************************************************************/
		void FadeSplashscreen(EntityID id, float alpha);

		/*!***********************************************************************************
		 \brief Updates the text of the text component on the entity passed in.

		 \param[in] id - ID of the entity to with a text component to update.
		 \param[in] text - Text to update the component with.

		 \return Returns true if the text was set successfully, false if the text component 
						could not be found.
		*************************************************************************************/
		bool SetText(EntityID const id, std::string const& text);

		/*!***********************************************************************************
		 \brief Updates the alpha of the text component on the entity passed in.

		 \param[in] id - ID of the entity to with a text component to update.
		 \param[in] alpha - Alpha to set text to.

		 \return Returns true if the text alpha was set successfully, false if the text component 
						could not be found.
		*************************************************************************************/
		bool SetTextAlpha(EntityID const id, float const alpha);

		/*!***********************************************************************************
		 \brief Updates the alpha of the GUI renderer component on the entity passed in.

		 \param[in] id - ID of the entity with a text component to update.
		 \param[in] alpha - Alpha to set text to.

		 \return Returns true if the alpha was set successfully, false if the GUI renderer 
						component could not be found.
		*************************************************************************************/
		bool SetGUIRendererAlpha(EntityID const id, float const alpha);

		/*!***********************************************************************************
		 \brief Switches the gamestate from movement and attack execution back to movement planning.
		*************************************************************************************/
		void ExecutionToMovement();


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
		std::map<EntityID, GameStateControllerData> m_ScriptData; // Data associated with each instance of the script
		bool m_finishExecution{ false }; // used to keep track of whether the execution phase is complete. Set to true when the cats and rats are done executing their attacks.
		EntityID bgm; // ID of the entity holding the background music
		EntityID godModeText; // god mode text
	};


}