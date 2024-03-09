/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatScript_v2_0_Data.h
 \date     09-03-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains declarations for structs that contain data about the rats.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "../StateManager.h"
#include "../ECS/Entity.h"
#include "../ECS/EntityFactory.h"
#include "../Math/MathCustom.h"

namespace PE
{
	enum class EnumRatType : char
	{
		GUTTER_V1,
		GUTTER,
		BRAWLER,
		SNIPER,
		RAT_TYPE_COUNT // Use this to get the number of rat types
	};

	enum class EnumRatAnimations : char
	{
		IDLE,
		WALK,
		ATTACK,
		HURT,
		DEATH
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
		EntityID detectionRadiusId{};
		EntityID pivotEntityID{ 0 };					// id of parent obj to rotate to adjust the orientation of the telegraphs
		EntityID telegraphArrowEntityID{ 0 };			// id of arrow telegraph
		EntityID attackTelegraphEntityID{ 0 };			// id of cross attack telegraph
		StateMachine* p_stateManager;

		bool isAlive{ true }; // True if the rat is alive and should be updated
		EnumRatType ratType{ EnumRatType::GUTTER };

		// animation
		AnimationComponent* p_ratAnimationComponent = nullptr;
		std::map<std::string, std::string> animationStates; // animation states of the rat <state name, file name>
		
		// rat stats
		int ratHealth{ 0 };								// health of the rat, needs manual setting
		int ratMaxHealth{ 3 };						// maximum health of the rat
		std::unordered_set<EntityID> hitBy; // IDs of attacks that the rat has been hit by during this execution phase

		// Movement Variables
		float ratPlayerDistance{ 0.f };					// stores distance of rat from player cat to determine movement
		vec2 targetPosition{ 0.f, 0.f };				// stores the target position the rat is supposed to move towards
		vec2 directionFromRatToPlayerCat{ 0.f, 0.f };	// stores the normalized vector pointing at player cat

		// Attack variables
		float attackRadius{ 101.f };
		bool attacking{ false };						// a check for whether the rat is close enough to the player to attack
		bool hitCat{ false };							// a check for whether the rat has hit the player once in the entire execution sequence

		// State change helper variables
		bool shouldChangeState{};  // Flags that the state should change when [timeBeforeChangingState] is zero
		bool delaySet{ false }; // Whether the state change has been flagged
		float timeBeforeChangingState{ 0.f }; // Delay before state should change
		bool finishedExecution{ true }; // Keeps track of whether the execution phase has been completed
		bool hasRatStateChanged{ false }; // True if the rat state changed in the last frame, false otherwise

		// Detection and movement
		float detectionRadius{ 200.f };
		std::set<EntityID> catsInDetectionRadius;
		std::set<EntityID> catsExitedDetectionRadius;
		EntityID targetedCat{}; // Cat to move towards in the hunting phase
		std::unordered_set<EntityID> attackedCats{}; // Cats that were attacked

		float movementSpeed{ 200.f };
		float maxMovementRange{ 100.f }; // Total distance that the rat will move in one execution phase
		float minDistanceToTarget{ 1.f }; // Amount that the rat can be offset from their target before being considered "close enough"

		// Hunting and returning
		int maxHuntTurns{ 2 }; // Number of turn the rat will hunt before returning to their original position
		vec2 originalPosition{ 0.f, 0.f }; // Position to return to
		
		// Patrol Points
		std::vector<vec2> patrolPoints;
		int patrolIndex{ 0 };										// Index of the current patrol point the rat is moving towards
		bool returnToFirstPoint{ false };				// Flag to indicate if the rat should return to the first patrol point after reaching the last

	private:
		State* p_queuedState{ nullptr }; // State to load

	public:

		/*!***********************************************************************************
		\brief Helper function to get the name of the rat based on its type.

		\param ratType - Rat type to get the name of.
		*************************************************************************************/
		static std::string GetRatName(EnumRatType ratType);

		/*!***********************************************************************************
		\brief Helper function to get the name of the rat based on its type.

		\param ratTypeIndex - Index of rat type to get the name of.
		*************************************************************************************/
		static std::string GetRatName(char ratTypeIndex);

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

	}; // end of struct RatScript_v2_0_Data

} // end of namespace PE
