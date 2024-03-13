/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatScript_v2_0.h
 \date     17-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains declarations for functions used for a rats mechanics

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "RatScript_v2_0_Data.h"

#include "../LogicSystem.h"
#include "../Script.h"
#include "../StateManager.h"
#include "../ECS/Entity.h"
#include "../ECS/EntityFactory.h"
#include "../Math/MathCustom.h"
#include "../Events/EventHandler.h"
#include "../GameStateController_v2_0.h"

namespace PE
{	
	class RatScript_v2_0 : public Script
	{

		// ----- Public Members ----- //
	public:
		std::map<EntityID, RatScript_v2_0_Data> m_scriptData;

		static const inline int detectionColliderLayer{ 5 };

		// ----- Constructors ----- //
	public:
		/*!***********************************************************************************
		 \brief Does nothing.
		*************************************************************************************/
		virtual ~RatScript_v2_0() {};


		// ----- Public Functions ----- //
	public:
		/*!***********************************************************************************
		\brief Called when the object is first spawned into the scene

		\param[in,out] id - ID of the script to update
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief Called every frame.

		 \param[in,out] id - ID of the instance to update
		 \param[in,out] deltaTime - delta time used to update the state
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Does nothing
		*************************************************************************************/
		virtual void Destroy(EntityID) {}

		/*!***********************************************************************************
		 \brief Called when the script is attached to an entity (NOTE: even within the editor!).
				DO NOT create objects here.

		 \param[in,out] id - ID of Instance of script to update
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		 \brief Called when the entity is destroyed and/or this script is removed from
				the entity. Delete the script data and statemanager here.

		 \param[in,out] id - ID of instance of script to clear
		*************************************************************************************/
		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Helper function to en/disables an entity.

		 \param[in] id EntityID of the entity to en/disable.
		 \param[in] setToActive Whether this entity should be set to active or inactive.
		*************************************************************************************/
		static void ToggleEntity(EntityID id, bool setToActive);

		/*!***********************************************************************************
		 \brief Adjusts the position of the transform to the value passed in.

		 \param[in] transformId ID of the entity to update the transform of.
		 \param[in] r_position Position to set the transform to.
		*************************************************************************************/
		static void PositionEntity(EntityID const transformId, vec2 const& r_position);
		
		/*!***********************************************************************************
		 \brief Adjusts the relative position of the transform to the value passed in.

		 \param[in] transformId ID of the entity to update the transform of.
		 \param[in] r_position Position to set the transform to.
		*************************************************************************************/
		static void PositionEntityRelative(EntityID const transformId, vec2 const& r_position);

		/*!***********************************************************************************
		 \brief Adjusts the scale of the transform to the value passed in.

		 \param[in] transformId ID of the entity to update the transform of.
		 \param[in] width Width to set the transform to.
		 \param[in] height Height to set the transform to.
		*************************************************************************************/
		static void ScaleEntity(EntityID const transformId, float const width, float const height);

		/*!***********************************************************************************
		 \brief Rotates the entity by the orientation amount passed in. 

		 \param[in] transformId ID of the entity to update the transform of.
		 \param[in] orientation Angle in radians about the z-axis starting from 
						the positive x axis.
		*************************************************************************************/
		static void RotateEntity(EntityID const transformId, float const orientation);

		/*!***********************************************************************************
		 \brief Updates the relative rotation of the entity by the orientation amount passed in. 

		 \param[in] transformId ID of the entity to update the transform of.
		 \param[in] orientation Angle in radians about the z-axis starting from 
						the positive x axis.
		*************************************************************************************/
		static void RotateEntityRelative(EntityID const transformId, float const orientation);


		// ----- Animation Helper Functions ----- //

		/*!***********************************************************************************
		 \brief Returns true if the current animation being played is the hurt animation, 
				false otherwise.

		 \return Returns true if the current animation being played is the hurt animation, 
				false otherwise.
		*************************************************************************************/
		bool GetIsPlayingHurtAnim(EntityID const id);

		/*!***********************************************************************************
		 \brief Returns true if the current animation is done playing, false otherwise.

		 \return Returns true if the current animation is done playing, false otherwise.
		*************************************************************************************/
		bool GetHasAnimEnded(EntityID const id);

		/*!***********************************************************************************
		 \brief Helper function for playing rat animation.

		 \param[in] id - EntityID of the rat which the animation should be playing on.
		 \param[in] animationState - Animation state that should be played.
		 \param[in] forceRestart - Set to true if the current animation should be restarted 
										if the animation state passed in matches the current state.
		*************************************************************************************/
		void PlayAnimation(EntityID const id, EnumRatAnimations const animationState, bool const forceRestart = false);

		/*!***********************************************************************************
		 \brief Helper function for checking if the rat's animation is done.

		 \param[in] id - EntityID of the rat which the animation should be playing on.
		 \return bool - Returns true if the animation is done, false otherwise.
		*************************************************************************************/
		float GetAnimationDuration(EntityID const id) const;


		// ----- Play Audio ----- //

		/*!***********************************************************************************
		 \brief Play one of a few random attack SFX.
		*************************************************************************************/
		void PlayAttackAudio(EntityID id);

		/*!***********************************************************************************
		 \brief Play one of a few random death SFX.
		*************************************************************************************/
		void PlayDeathAudio(EntityID id);

		/*!***********************************************************************************
		 \brief Play one of a few random detection SFX.
		*************************************************************************************/
		void PlayDetectionAudio(EntityID id);

		/*!***********************************************************************************
		 \brief Play one of a few random injury SFX.
		*************************************************************************************/
		void PlayInjuredAudio(EntityID id);

		/*!***********************************************************************************
		 \brief Spawn an audio object using the audio object at the filepath passed in and
				play audio with it.

		 \param[in] soundPrefab - Filepath of the audio object prefab to load.
		*************************************************************************************/
		static void PlayAudio(std::string const& soundPrefab);


		// ----- Rat stuff ----- //

		/*!***********************************************************************************
		 \brief Deals damage to the cat that the attack collider collided with.

		 \param[in] collidedCat - ID of the cat to deal damage to.
		 \param[in] rat - ID of the rat dealing damage to the cat.
		*************************************************************************************/
		void DealDamageToCat(EntityID collidedCat, EntityID rat);

		// ----- Getters/RTTR ----- //

		/*!***********************************************************************************
		 \brief Returns the position of the transform of the entity passed in.

		 \param[in] transformId ID of the entity to retrieve the position of.
		*************************************************************************************/
		static vec2 GetEntityPosition(EntityID const transformId);

		/*!***********************************************************************************
		 \brief Returns the scale of the transform of the entity passed in.

		 \param[in] transformId ID of the entity to retrieve the scale of.
		*************************************************************************************/
		static vec2 GetEntityScale(EntityID const transformId);

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \return std::map<EntityID, RatScript_v2_0_Data>& Map of script data.
		*************************************************************************************/
		std::map<EntityID, RatScript_v2_0_Data> &GetScriptData();

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \param[in,out] id - ID of the script to get the data from
		 \return rttr::instance Instance of the script to get the data from
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);


		// --- TELEGRAPH HELPER FUNCTIONS --- // 

		/*!***********************************************************************************
		 \brief Rotates the movement telegraph to face the target position of the rat 
				and enable it.

		 \param[in] id - EntityID of the rat.
		 \param[in] r_targetPosition - Position that the rat aims to move towards.
		*************************************************************************************/
		void EnableMovementTelegraphs(EntityID id, vec2 const& r_targetPosition);

		/*!***********************************************************************************
		 \brief Disables the movement telegraphs.

		 \param[in] id - EntityID of the rat.
		*************************************************************************************/
		void DisableMovementTelegraphs(EntityID id);

		/*!***********************************************************************************
		 \brief Enables the detection telegraph (a question mark).Called by an animation event.

		 \param[in] id - EntityID of the rat.
		*************************************************************************************/
		void EnableDetectionTelegraphs(EntityID id);

		/*!***********************************************************************************
		 \brief Disables the detection telegraph (a question mark). Called by an animation event.

		 \param[in] id - EntityID of the rat.
		*************************************************************************************/
		void DisableDetectionTelegraphs(EntityID id);

		/*!***********************************************************************************
		 \brief Disables all entities spawned by the rat.

		 \param[in] id - EntityID of the rat.
		*************************************************************************************/
		void DisableAllSpawnedEntities(EntityID id);


		// --- RAT STATE CHANGE DETECTION --- // 

		/*!***********************************************************************************
		 \brief Sets the flag for the state to be changed after the delay passed in.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] p_nextState - Pointer to the state to change to. Can be set to nullptr
										to cancel the previous state change request, if any.
		 \param[in] stateChangeDelay - Time in seconds before switching to the next state.
										Set to zero by default.
		*************************************************************************************/
		void TriggerStateChange(EntityID id, State* p_nextState, float const stateChangeDelay = 0.f);

		/*!***********************************************************************************
		 \brief Changes the state if the flag has been set and the delay is zero and below.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] deltaTime - Time in seconds that have passed since the last frame.

		 \return true - True if the state should change.
		 \return false - False if the state should NOT change.
		*************************************************************************************/
		bool CheckShouldStateChange(EntityID id, float const deltaTime);

		/*!***********************************************************************************
		 \brief Sets the flag for the state to be changed to the hunting state
						after the delay passed in.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] targetId - EntityID of the entity to target.
		 \param[in] stateChangeDelay - Time in seconds before switching to the next state.
										Set to zero by default.
		*************************************************************************************/
		void ChangeStateToHunt(EntityID const id, EntityID const targetId, float const stateChangeDelay = 0.f);

		/*!***********************************************************************************
		 \brief Sets the flag for the state to be changed to the returning state
						after the delay passed in.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] stateChangeDelay - Time in seconds before switching to the next state.
										Set to zero by default.
		*************************************************************************************/
		void ChangeStateToReturn(EntityID const id, float const stateChangeDelay = 0.f);

		/*!***********************************************************************************
		 \brief Sets the flag for the state to be changed to the idle state
						after the delay passed in.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] stateChangeDelay - Time in seconds before switching to the next state.
										Set to zero by default.
		*************************************************************************************/
		void ChangeStateToIdle(EntityID const id, float const stateChangeDelay = 0.f);

		/*!***********************************************************************************
		 \brief Sets the flag for the state to be changed to the movement state
						after the delay passed in.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] stateChangeDelay - Time in seconds before switching to the next state.
										Set to zero by default.
		*************************************************************************************/
		void ChangeStateToMovement(EntityID const id, float const stateChangeDelay = 0.f);

		/*!***********************************************************************************
		 \brief Sets the flag for the state to be changed to the attack state 
						after the delay passed in.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] stateChangeDelay - Time in seconds before switching to the next state.
										Set to zero by default.
		*************************************************************************************/
		void ChangeStateToAttack(EntityID const id, float const stateChangeDelay = 0.f);

		/*!***********************************************************************************
		 \brief Sets the flag for the state to be changed to the death state 
						after the delay passed in.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] stateChangeDelay - Time in seconds before switching to the next state.
										Set to zero by default.
		*************************************************************************************/
		void ChangeStateToDeath(EntityID const id, float const stateChangeDelay = 0.f);

		/*!***********************************************************************************
		\brief Returns true if the execution phase should time out.

		\param[in] id - EntityID of the rat.

		\return Returns true if the execution phase should time out, false otherwise.
		*************************************************************************************/
		bool GetExecutionPhaseTimeout(EntityID const id);

		/*!***********************************************************************************
		\brief Returns true if the rat is aggressive (i.e. in movement or attack state).

		\param[in] id - EntityID of the rat.

		\return Returns true if the rat is in an aggressive state, false otherwise.
		*************************************************************************************/
		bool GetIsAggressive(EntityID const id);


		// --- COLLISION DETECTION --- // 

		/*!***********************************************************************************
		 \brief Returns true if the entity passed in is an obstacle.

		 \param[in] id - EntityID of the entity to check.
		 \return bool - Returns true if the entity passed in is a obstacle, false otherwise.
		*************************************************************************************/
		static bool GetIsObstacle(EntityID const id);

		/*!***********************************************************************************
		 \brief Returns true if the entity passed in is a cat that isn't caged, false otherwise.

		 \param[in] id - EntityID of the entity to check.
		 \return bool - Returns true if the entity passed in is a cat, false otherwise.
		*************************************************************************************/
		static bool GetIsNonCagedCat(EntityID const id);

		/*!***********************************************************************************
		 \brief Clears the containers used to store the cats being collided with.

		 \param[in] id - EntityID of the rat whose detection radius was entered.
		*************************************************************************************/
		void ClearCollisionContainers(EntityID const id);

		/*!***********************************************************************************
		 \brief Clears dead cats from the set passed in.

		 \param[in,out] catSet - Container of cats to clear dead cats from.
		*************************************************************************************/
		static void ClearDeadCats(std::set<EntityID>& catSet);

		/*!***********************************************************************************
		 \brief Called when a cat is within the rat's detection radius.

		 \param[in] id - EntityID of the rat whose detection radius was entered.
		 \param[in] catID - ID of the cat that entered the radius.
		*************************************************************************************/
		void CatEntered(EntityID const id, EntityID const catID);

		/*!***********************************************************************************
		 \brief Called when a cat exits the rat's detection radius.

		 \param[in] id - EntityID of the rat whose detection radius was entered.
		 \param[in] catID - ID of the cat that exited the radius.
		*************************************************************************************/
		void CatExited(EntityID const id, EntityID const catID);

		/*!***********************************************************************************
		\brief Checks if the collision event involved the rat's detection radius and a cat,
				calls the CatEntered() function if true.

		\param[in] ratId - EntityID of the rat to check the detection radius of.
		\param entity1 - One of the entities involved in the trigger event.
		\param entity2 - One of the entities involved in the trigger event.

		\return Returns true if the collision involved the rat's detection radius and a cat,
				false otherwise.
		*************************************************************************************/
		bool CheckDetectionTriggerEntered(EntityID const ratId, EntityID const entity1, EntityID const entity2);

		/*!***********************************************************************************
		\brief Checks if the collision event involved the rat's detection radius and a cat,
				calls the CatExited() function if true.

		\param[in] ratId - EntityID of the rat to check the detection radius of.
		\param entity1 - One of the entities involved in the trigger event.
		\param entity2 - One of the entities involved in the trigger event.

		\return Returns true if the collision involved the rat's detection radius and a cat,
				false otherwise.
		*************************************************************************************/
		bool CheckDetectionTriggerExited(EntityID const ratId, EntityID const entity1, EntityID const entity2);

		/*!***********************************************************************************
		\brief Checks if the collision event involves the rat's collider and a cat,
				calls DealDamageToCat() function if true.

		\param[in] ratId - EntityID of the rat to check the detection radius of.
		\param entity1 - One of the entities involved in the collision event.
		\param entity2 - One of the entities involved in the collision event.

		\return Returns true if the collision involves the rat's collider and a cat,
				false otherwise.
		*************************************************************************************/
		bool CheckRatTouchingCat(EntityID const ratId, EntityID const entity1, EntityID const entity2);

		/*!***********************************************************************************
		\brief Checks if the collision event involves the rat's collider and a wall,
				times the execution phase out if true. Call during on collision enter events.

		\param[in] ratId - EntityID of the rat to check the detection radius of.
		\param entity1 - One of the entities involved in the collision event.
		\param entity2 - One of the entities involved in the collision event.

		\return Returns true if the collision involves the rat's collider and a cat,
				false otherwise.
		*************************************************************************************/
		bool CheckRatTouchingWall(EntityID const ratId, EntityID const entity1, EntityID const entity2);

		/*!***********************************************************************************
		\brief Checks if the collision event involves the rat's collider and a wall. 
				Call during on collision exit events.

		\param[in] ratId - EntityID of the rat to check the detection radius of.
		\param entity1 - One of the entities involved in the collision event.
		\param entity2 - One of the entities involved in the collision event.

		\return Returns true if the collision involves the rat's collider and a cat,
				false otherwise.
		*************************************************************************************/
		bool CheckRatStopTouchingWall(EntityID const ratId, EntityID const entity1, EntityID const entity2);


		// ------------ MOVEMENT HELPER FUNCTIONS ------------ //

		/*!***********************************************************************************
		 \brief Returns the ID of the entity from the container closest to the position passed in.

		 \param[in] position - Position to compare the entities against.
		 \param[in] potentialTargets - Container of the entities to compare distances to.
		 \return EntityID - Returns the ID of the entity closest to the position passed in.
		*************************************************************************************/
		static EntityID GetCloserTarget(vec2 position, std::set<EntityID> const& potentialTargets);

		/*!***********************************************************************************
		 \brief Returns the position from the container closest to the position passed in.

		 \param[in] position - Position to compare the entities against.
		 \param[in] potentialTargets - Container of the positions to compare distances to.
		 \return EntityID - Returns the position closest to the position passed in.
		*************************************************************************************/
		static vec2 GetCloserTarget(vec2 position, std::vector<vec2> const& potentialTargets);

		/*!***********************************************************************************
		 \brief Sets the position that the rat should move towards when CalculateMovement
						is called.

		 \param[in] id - EntityID of the rat.
		 \param[in] targetId - ID of the entity that has a position we are to move towards.
		 \param[in] capMaximum - Set to true to shift the target position within the range
					of the rat's maximum range per turn, false to allow the rat to exceed their
					max range.
		 \return Returns true if the target position is far enough away from the rat, false otherwise.
		*************************************************************************************/
		bool SetTarget(EntityID id, EntityID targetId, bool const capMaximum);

		/*!***********************************************************************************
		 \brief Sets the position that the rat should move towards when CalculateMovement 
						is called. 

		 \param[in] id - EntityID of the rat.
		 \param[in] r_targetPosition - position to move towards.
		 \param[in] capMaximum - Set to true to shift the target position within the range 
					of the rat's maximum range per turn, false to allow the rat to exceed their 
					max range.
		 \return Returns true if the target position is far enough away from the rat, false otherwise.
		*************************************************************************************/
		bool SetTarget(EntityID id, vec2 const& r_targetPosition, bool const capMaximum);

		/*!***********************************************************************************
		 \brief Moves the rat at a speed of [RatScriptData::movementSpeed] in the direction 
				of [RatScriptData::directionFromRatToPlayerCat]. After moving the rat, if 
				[RatScriptData::ratPlayerDistance] is within [RatScriptData::minDistanceToTarget]
				of the target position, the function returns true.

		 \param[in] id - EntityID of the rat.
		 \param[in] r_targetPosition - position to move towards.
		 \param[in] capMaximum - Set to true to shift the target position within the range
					of the rat's maximum range per turn, false to allow the rat to exceed their
					max range.
		 \return Returns true if the rat is within [RatScriptData::minDistanceToTarget] 
				of the target, false otherwise.
		*************************************************************************************/
		bool CalculateMovement(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Returns true if the rat is within [RatScriptData::minDistanceToTarget]
				of [RatScriptData::targetPosition], false otherwise.

		 \param[in] id - EntityID of the rat.
		 \return Returns true if the rat is within within [RatScriptData::minDistanceToTarget]
				of [RatScriptData::targetPosition], false otherwise.
		*************************************************************************************/
		static bool CheckDestinationReached(EntityID id);

		/*!***********************************************************************************
		 \brief Returns true if [newPosition] is within [minDistanceToTarget] of [targetPosition].

		 \param[in] minDistanceToTarget - Maximum distance [newPosition] can be from 
				[targetPosition] for the function to return true.
		 \param[in] newPosition - Position to compare with the target position.
		 \param[in] targetPosition - Position to get [newPosition]'s distance from.
		 \return Returns true if [newPosition] is within [minDistanceToTarget] of 
				[targetPosition], false otherwise.
		*************************************************************************************/
		static bool CheckDestinationReached(float const minDistanceToTarget, const vec2& newPosition, const vec2& targetPosition);

		/*!***********************************************************************************
		 \brief Reduces the health of the rat and plays the SFX. If the rat has run out of 
						health, the rat is killed using the KillRat function.

		 \param[in] ratId - EntityID of the rat.
		 \param[in] damageSourceId - EntityID of the entity that damaged the rat.
		 \param[in] damage - Amount to reduce the rat's health by.
		*************************************************************************************/
		void DamageRat(EntityID ratId, EntityID damageSourceId, int damage);

		/*!***********************************************************************************
		 \brief Kills the rat, setting it to its death state and playing the death SFX + anim.

		 \param[in] id - EntityID of the rat.
		*************************************************************************************/
		void KillRat(EntityID id);

		// ----- Private Members ----- //
	private:
			// Event listener IDs 
			GameStateController_v2_0* gameStateController{ nullptr }; // pointer to the game state controller
			GameStates_v2_0 previousGameState; // The game state in the previous frame

		// ----- Private Methods ----- // 
	private:
		// --- STATE CHANGE --- //

		/*!***********************************************************************************
		 \brief Creates the state manager if it has not been created.

		 \param[in] id EntityID of the entity to create the state manager for.
		*************************************************************************************/
		void CreateCheckStateManager(EntityID id);

		/*!***********************************************************************************
		 \brief Change the state of the rat to the state queued up.

		 \param[in] id - EntityID of the entity to change the state of.
		*************************************************************************************/
		void ChangeRatState(EntityID id);

		/*!***********************************************************************************
			\brief Returns true if the current game state is different from the game state
							in the previous frame, false otherwise.
		*************************************************************************************/
		inline bool GameStateJustChanged() const
		{
				return previousGameState != gameStateController->currentState;
		}

		// --- OBJECT CREATION --- //

		/*!***********************************************************************************
		 \brief Load the detection radius collider prefab and return its EntityID.

		 \param[in] r_data - Reference to this rat's data.

		 \return EntityID - ID of created entity.
		*************************************************************************************/
		EntityID CreateDetectionRadius(RatScript_v2_0_Data const& r_data);

		/*!***********************************************************************************
		 \brief Create a prefab to child the telegraphs to.

		 \param[in, out] r_data - Reference to this rat's data.
		*************************************************************************************/
		void CreateRatTelegraphPivot(RatScript_v2_0_Data& r_data);

		/*!***********************************************************************************
		 \brief Load the movement telegraph prefab and store it in the rat data.

		 \param[in, out] r_data - Reference to this rat's data.
		*************************************************************************************/
		void CreateRatPathTelegraph(RatScript_v2_0_Data& r_data);

		/*!***********************************************************************************
		 \brief Load the detection telegraph prefab and store it in the rat data.

		 \param[in, out] r_data - Reference to this rat's data.
		*************************************************************************************/
		void CreateRatDetectionTelegraph(RatScript_v2_0_Data& r_data);

		/*!***********************************************************************************
		 \brief Create an instance of the appropriate attack object for the rat according 
					to its type and store it in the rat data.

		 \param[in, out] r_data - Reference to this rat's data. 
		*************************************************************************************/
		void InstantiateRatAttack(RatScript_v2_0_Data& r_data);
	}; // end of class 
}
