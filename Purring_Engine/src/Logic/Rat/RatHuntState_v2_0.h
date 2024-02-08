/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatHuntState_v2_0.h
 \date     24-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions for a temp rat state for testing purposes.

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

		RatHunt_v2_0(EntityID const _targetId) : targetId{ _targetId } { /* Empty by design */ }

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
		virtual std::string_view GetName() override { return "RatHuntState_v2_0"; }

		/*!***********************************************************************************
			\brief Sets the target and resets the number of hunting turns.

			\param targetId - ID of the entity to target.
		*************************************************************************************/
		void SetNewTarget(EntityID const targetId);

		// --- COLLISION DETECTION --- // 

		/*!***********************************************************************************
		\brief Returns true if there are cats in the detection range, false otherwise.

		\return bool - Returns true if there are cats in the detection range, false otherwise.
		*************************************************************************************/
		bool CheckCatsInDetectionRange();

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
		GameStates_v2_0 previousGameState{ GameStates_v2_0::PLANNING }; // The game state in the previous frame

		EntityID targetId{}; // ID of the cat to target

		int huntingTurnsLeft{}; // Number of turns left for the rat to spend hunting

		// Event listener IDs 
		int m_collisionEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};

		// Waypoints for level 1, starting from the left of the scene
		std::vector<vec2> waypointsLevel1{
				vec2{-518.f, 169.f},
				vec2{-529.f, 4.f},
				vec2{-364.f, 112.f},
				vec2{-334.f, -107.f},
				vec2{-260.f, 318.f},
				vec2{-177.f, 24.f},
				vec2{-104.f, 193.f},
				vec2{-17.f, -139.f},
				vec2{24.f, 9.f},
				vec2{150.f, 123.f},
				vec2{236.f, -102.f},
				vec2{358.f, 27.f},
				vec2{587.f, 99.f}
		};

		// Waypoints for level 2, starting from the left of the scene
		std::vector<vec2> waypointsLevel2{
				vec2{-903.f, 83.f},
				vec2{-681.f, 83.f},
				vec2{-577.f, -40.f},
				vec2{-391.f, -121.f},
				vec2{-182.f, -147.f},
				vec2{-174.f, -284.f},
				vec2{19.f, -289.f},
				vec2{5.f, -171.f},
				vec2{160.f, -148.f},
				vec2{88.f, -32.f},
				vec2{259.f, -27.f},
				vec2{213.f, 94.f},
				vec2{398.f, 52.f},
				vec2{554.f, 92.f},
				vec2{654.f, 196.f}
		};

	private:

			/*!***********************************************************************************
				\brief Returns true if the current game state is different from the game state 
								in the previous frame, false otherwise.
			*************************************************************************************/
			inline bool StateJustChanged() const
			{
					return previousGameState != gameStateController->currentState;
			}

			void PickTargetPosition();
	};
} // End of namespace PE
