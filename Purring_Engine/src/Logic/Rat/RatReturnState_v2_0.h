/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatReturnState_v2_0.h
 \date     24-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions for the rat's return state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "../StateManager.h"
#include "RatScript_v2_0.h"

namespace PE
{
	class RatReturn_v2_0 : public State
	{
		// ----- Destructor ----- //
	public:
		virtual ~RatReturn_v2_0() override { p_data = nullptr; }

		// ----- Public Functions ----- //
	public:
		/*!***********************************************************************************
			\brief Play the idle animation and start the timer

			\param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;

		/*!***********************************************************************************
			\brief Checks if its state should change

			\param[in,out] id - ID of instance of script
			\param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events
		*************************************************************************************/
		virtual void StateCleanUp();

		/*!***********************************************************************************
			\brief does nothing
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;

		/*!***********************************************************************************
			\brief Get the name of the state

			\return std::string_view name of state
		*************************************************************************************/
		virtual std::string_view GetName() override { return GETSCRIPTNAME(RatReturn_v2_0); }

	private:
	  RatScript_v2_0_Data* p_data; // pointer to script instance data
		GameStateController_v2_0* gameStateController{ nullptr }; // pointer to the game state controller
		GameStates_v2_0 previousGameState; // The game state in the previous frame
		bool m_planningRunOnce{ false }; // Set to true after target position has been set in the pause state, set to false one frame after the pause state has started.

		// Event listener IDs 
		int m_collisionEnterEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};
		int m_triggerEnterEventListener{}, m_triggerStayEventListener{}, m_triggerExitEventListener{};

	private:
		/*!***********************************************************************************
			\brief Returns true if the current game state is different from the game state
							in the previous frame, false otherwise.
		*************************************************************************************/
		inline bool StateJustChanged() const
		{
				return previousGameState != gameStateController->currentState;
		}

		/*!***********************************************************************************
		 \brief Checks if any cats entered or executed the rat's detection radius during
						the last execution phase and decides whether to swap to the attacking or
						hunting states respectively.
		*************************************************************************************/
		void CheckIfShouldChangeStates();


		// --- COLLISION DETECTION --- // 

		/*!***********************************************************************************
		 \brief Called when a collision enter event has occurred. If an event has
			occurred between this script's rat's collider and a cat or an obstacle,
			the parent rat is notified.

		 \param[in] r_event - Event data.
		*************************************************************************************/
		void OnCollisionEnterOrStay(const Event<CollisionEvents>& r_event);

		/*!***********************************************************************************
		 \brief Called when a collision exit event has occurred. If an event has
			occurred between this script's rat's collider and an obstacle, the parent rat
			is notified.

		 \param[in] r_event - Event data.
		*************************************************************************************/
		void OnCollisionExit(const Event<CollisionEvents>& r_event);

		/*!***********************************************************************************
		 \brief Called when a trigger enter or stay event has occured. If an event has
			occurred between this script's rat's detection collider and a cat, the parent rat
			is notified.

		 \param[in,out] r_TE - Trigger event data.
		*************************************************************************************/
		void OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE);

		/*!***********************************************************************************
		 \brief Called when a trigger exit event has occured. If an event has occurred
			between this script's rat's detection collider and a cat, the parent rat
			is notified.

		 \param[in,out] r_TE - Trigger event data.
		*************************************************************************************/
		void OnTriggerExit(const Event<CollisionEvents>& r_TE);
	};
} // End of namespace PE
