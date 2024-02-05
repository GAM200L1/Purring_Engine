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
		GUTTER,
		BRAWLER,
		SNIPER,
		HERALD
	};

	struct RatScript_v2_0_Data
	{
		~RatScript_v2_0_Data()
		{
			delete p_stateManager;
		}

		// reference entities
		EntityID myID{ 0 };								// id of the rat with this data
		EntityID ratTelegraphID{ 0 };					// id of an additional invisible object with transform for rotating the arrow telegraph
		EntityID mainCatID{ 3 };						// needs manual setting
		EnumRatType ratType{ EnumRatType::GUTTER };
		StateMachine* p_stateManager;

		// Movement Variables
		float ratPlayerDistance{ 0.f };					// stores distance of rat from player cat to determine movement

		// Attack entities and variables
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
		bool finishedExecution{ false }; // Keeps track of whether the execution phase has been completed

		// Detection and movement
		EntityID detectionRadiusId{};
		float detectionRadius{ 200.f };
		std::set<EntityID> catsInDetectionRadius;
		std::set<EntityID> catsExitedDetectionRadius;

		float movementSpeed{ 200.f };

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
		 \brief Adjusts the scale of the transform to the value passed in.

		 \param[in] transformId ID of the entity to update the transform of.
		 \param[in] width Width to set the transform to.
		 \param[in] height Height to set the transform to.
		*************************************************************************************/
		static void ScaleEntity(EntityID const transformId, float const width, float const height);


		// ----- Rat stuff ----- //
		void RatHitCat(EntityID id, const Event<CollisionEvents>& r_TE);

		void CheckFollowOrMain(EntityID mainCat, EntityID collidedCat, EntityID damagingID, EntityID rat);

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

		void TriggerStateChange(EntityID id, float const stateChangeDelay);

		// --- COLLISION DETECTION --- // 

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

		// ----- Private Members ----- //
	private:
			// Event listener IDs 
			int m_collisionEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};

		// ----- Private Methods ----- // 
	private:
		// --- STATE CHANGE --- //

		/*!***********************************************************************************
		 \brief Creates the state manager if it has not been created.

		 \param[in] id EntityID of the entity to create the state manager for.
		*************************************************************************************/
		void CreateCheckStateManager(EntityID id);

		EntityID CreateDetectionRadius(RatScript_v2_0_Data const& r_data);
	}; // end of class 
}
