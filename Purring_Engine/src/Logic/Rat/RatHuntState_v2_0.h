/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatHuntState_v2_0.h
 \date     24-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions for the rat's hunting state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "../StateManager.h"
#include "RatScript_v2_0.h"

namespace PE
{
	class RatHunt_v2_0 : public State
	{
		// ----- Constructors ----- //
	public:
		/*!***********************************************************************************
			\brief Constructor for RatHunt_v2_0

			\param[in] _targetID - id of the target cat
		*************************************************************************************/
		RatHunt_v2_0(EntityID const _targetId) : targetId{ _targetId } { /* Empty by design */ }

		/*!***********************************************************************************
			\brief Destructor for RatHunt_v2_0
		*************************************************************************************/
		virtual ~RatHunt_v2_0() override { p_data = nullptr; }

		// ----- Public Functions ----- //
	public:
		/*!***********************************************************************************
			\brief Set up the state and subscribe to the collision events

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
			\brief does nothing
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events
		*************************************************************************************/
		virtual void StateCleanUp();	

		/*!***********************************************************************************
			\brief Get the name of the state

			\return std::string_view name of state
		*************************************************************************************/
		virtual std::string_view GetName() override { return GETSCRIPTNAME(RatHunt_v2_0); }

		/*!***********************************************************************************
			\brief Sets the target and resets the number of hunting turns.

			\param targetId - ID of the entity to target.
		*************************************************************************************/
		void SetHuntTarget(EntityID const targetId);

		/*!***********************************************************************************
		 \brief Checks if any cats entered or executed the rat's detection radius during
						the last execution phase and decides whether to swap to the attacking or
						hunting states respectively.
		*************************************************************************************/
		void CheckIfShouldChangeStates();


		// --- COLLISION DETECTION --- // 

		/*!***********************************************************************************
		 \brief Called when a collision enter or stay event has occurred. If an event has
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

	private:
		RatScript_v2_0_Data* p_data{ nullptr }; // pointer to script instance data
		GameStateController_v2_0* gameStateController{ nullptr }; // pointer to the game state controller
		GameStates_v2_0 m_previousGameState{ GameStates_v2_0::PLANNING }; // The game state in the previous frame
		bool m_planningRunOnce{ false }; // Set to true after target position has been set in the pause state, set to false one frame after the pause state has started.

		EntityID targetId{}; // ID of the cat to target

		int huntingTurnsLeft{}; // Number of turns left for the rat to spend hunting

		// Event listener IDs 
		int m_collisionEnterEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};
		int m_triggerEnterEventListener{},  m_triggerStayEventListener{}, m_triggerExitEventListener{};

	private:

			/*!***********************************************************************************
				\brief Returns true if the current game state is different from the game state 
								in the previous frame, false otherwise.
			*************************************************************************************/
			inline bool StateJustChanged() const
			{
					return m_previousGameState != gameStateController->currentState;
			}

			/*!***********************************************************************************
				\brief Pick the position that the rat should move toward (in a straight line).

				\return Returns a next viable target for the rat.
			*************************************************************************************/
			vec2 PickTargetPosition();

			/*!***********************************************************************************
				\brief Rotates a point about a center

				\param[in] center - center to rotate about
				\param[in] point - point to rotate about center
				\param[in] angle - angle to rotate
				\return Returns where the point ends up
			*************************************************************************************/
			vec2 RotatePoint(vec2 center, vec2 point, float angle);
	};
} // End of namespace PE
