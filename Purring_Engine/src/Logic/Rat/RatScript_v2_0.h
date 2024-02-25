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
	enum EnumRatType
	{
		GUTTER_V1,
		GUTTER,
		BRAWLER,
		SNIPER,
		HERALD
	};

	struct RatScript_v2_0_Data
	{
		~RatScript_v2_0_Data()
		{
			delete p_queuedState;
			delete p_stateManager;
		}

		// reference entities
		EntityID myID{ 0 };								// id of the rat with this data
		EntityID ratTelegraphID{ 0 };			// id of an additional invisible object with transform for rotating the arrow telegraph
		EnumRatType ratType{ EnumRatType::GUTTER };
		StateMachine* p_stateManager;
		
		// rat stats
		int ratHealth{ 3 };								// health of the rat, needs manual setting
		int ratMaxHealth{ 3 };						// maximum health of the rat

		// Movement Variables
		float ratPlayerDistance{ 0.f };					// stores distance of rat from player cat to determine movement
		vec2 targetPosition{ 0.f, 0.f };				// stores the target position the rat is supposed to move towards

		// Attack entities and variables
		EntityID pivotEntityID{ 0 };					// id of parent obj to rotate to adjust the orientation of the telegraphs
		EntityID telegraphArrowEntityID{ 0 };			// id of arrow telegraph
		EntityID attackTelegraphEntityID{ 0 };			// id of cross attack telegraph
		vec2 directionFromRatToPlayerCat{ 0.f, 0.f };	// stores the normalized vector pointing at player cat
		EntityID redTelegraphEntityID{ 0 };				// id of red detection telegraph

		int collisionDamage{ 1 };						// damage when touching the rat needs manual setting
		int attackDamage{ 1 };							// damage when properly attacked by the rat needs manual setting

		float attackDelay{ 0.f };						// delay before attacking cat, needs manual setting
		bool attacking{ false };						// a check for whether the rat is close enough to the player to attack
		bool hitCat{ false };							// a check for whether the rat has hit the player once in the entire execution sequence

		// Rat Idle
		bool shouldPatrol{ false };						// Flag to determine if the said rat should patrol

		std::map<std::string, std::string> animationStates;

		bool isAlive{ true }; // True if the rat is alive and should be updated

		bool shouldChangeState{};  // Flags that the state should change when [timeBeforeChangingState] is zero
		bool delaySet{ false }; // Whether the state change has been flagged
		float timeBeforeChangingState{ 0.f }; // Delay before state should change
		bool finishedExecution{ true }; // Keeps track of whether the execution phase has been completed
		bool hasRatStateChanged{ false }; // True if the rat state changed in the last frame, false otherwise

		// Detection and movement
		EntityID detectionRadiusId{};
		float detectionRadius{ 200.f };
		std::set<EntityID> catsInDetectionRadius;
		std::set<EntityID> catsExitedDetectionRadius;
		EntityID targetedCat{}; // Cat to move towards in the hunting phase
		
		EntityID attackRadiusId{};
		float attackRadius{101.f};
		std::set<EntityID> attackRangeInDetectionRadius;
		std::set<EntityID> attackRangeExitedDetectionRadius;

		float movementSpeed{ 200.f };
		float maxMovementRange{ 100.f }; // Total distance that the rat will move in one execution phase
		float minDistanceToTarget{ 1.f }; // Amount that the rat can be offset from their target before being considered "close enough"

		// Hunting and returning
		int maxHuntTurns{ 2 }; // Number of turn the rat will hunt before returning to their original position
		vec2 originalPosition{ 0.f, 0.f }; // Position to return to
		
		// Attack 
		int skillDamage{};
		int skillArea{}; // probably dependent on the attack and cannot be stored as a flat var

		// Patrol Points
		std::vector<vec2> patrolPoints;
		int patrolIndex{ 0 };							// Index of the current patrol point the rat is moving towards
		bool returnToFirstPoint{ false };				// Flag to indicate if the rat should return to the first patrol point after reaching the last


		bool animationStart{ false };

	private:
		State* p_queuedState{ nullptr }; // State to load

	public:

		/*!***********************************************************************************
		\brief Setter for the pointer to the state the rat should be set to. 
						Deletes any states currently being pointed to.

		\param p_newState - Pointer to the state the rat should be set to.
		\param bypassDeletion - Set to true to not delete the existing data being pointed at,
								false to delete any data being pointed at if setting a new state.
		*************************************************************************************/
		void SetQueuedState(State* p_newState, bool bypassDeletion)
		{
			if (!bypassDeletion && p_queuedState)
			{
				delete p_queuedState;
			}

			p_queuedState = p_newState;
		}

		/*!***********************************************************************************
		 \brief Getter for the pointer to the state the rat should be set to.
		*************************************************************************************/
		State* GetQueuedState()
		{
			return p_queuedState;
		}

	};
	
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


		// ----- Rat stuff ----- //
		void RatHitCat(EntityID id, const Event<CollisionEvents>& r_TE);

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

		 \param[in] r_targetPosition - Position that the rat aims to move towards.
		*************************************************************************************/
		void EnableTelegraphs(EntityID id, vec2 const& r_targetPosition);

		void DisableTelegraphs(EntityID id);


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


		// --- COLLISION DETECTION --- // 

		/*!***********************************************************************************
		 \brief Returns true if the entity passed in is a cat, false otherwise.

		 \param[in] id - EntityID of the entity to check.
		 \return bool - Returns true if the entity passed in is a cat, false otherwise.
		*************************************************************************************/
		static bool GetIsCat(EntityID const id);

		/*!***********************************************************************************
		 \brief Clears the containers used to store the cats being collided with.

		 \param[in] id - EntityID of the rat whose detection radius was entered.
		*************************************************************************************/
		void ClearCollisionContainers(EntityID const id);

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


		// ------------ MOVEMENT HELPER FUNCTIONS ------------ //

		static EntityID GetCloserTarget(vec2 position, std::set<EntityID> const& potentialTargets);
		static vec2 GetCloserTarget(vec2 position, std::vector<vec2> const& potentialTargets);

		/*!***********************************************************************************
		 \brief Sets the position that the rat should move towards when CalculateMovement
						is called.

		 \param[in] id - EntityID of the rat.
		 \param[in] targetId - ID of the entity that has a position we are to move towards.
		 \param[in] capMaximum - Set to true to shift the target position within the range
					of the rat's maximum range per turn, false to allow the rat to exceed their
					max range.
		*************************************************************************************/
		void SetTarget(EntityID id, EntityID targetId, bool const capMaximum);

		/*!***********************************************************************************
		 \brief Sets the position that the rat should move towards when CalculateMovement 
						is called. 

		 \param[in] id - EntityID of the rat.
		 \param[in] r_targetPosition - position to move towards.
		 \param[in] capMaximum - Set to true to shift the target position within the range 
					of the rat's maximum range per turn, false to allow the rat to exceed their 
					max range.
		*************************************************************************************/
		void SetTarget(EntityID id, vec2 const& r_targetPosition, bool const capMaximum);

		// Returns true if the target has been reached, false otherwise
		bool CalculateMovement(EntityID id, float deltaTime);
		static bool CheckDestinationReached(float const minDistanceToTarget, const vec2& newPosition, const vec2& targetPosition);
		
		void CatEnteredAttackRadius(EntityID const id, EntityID const catID);
		void CatExitedAttackRadius(EntityID const id, EntityID const catID);



		// ----- Private Members ----- //
	private:
			// Event listener IDs 
			int m_collisionEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};
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

		EntityID CreateDetectionRadius(RatScript_v2_0_Data const& r_data);

		EntityID CreateAttackRangeRadius(RatScript_v2_0_Data const& r_data);

		void CreateRatPathTelegraph(RatScript_v2_0_Data& r_data);
	}; // end of class 
}
