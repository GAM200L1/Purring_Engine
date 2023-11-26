/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatScript.h
 \date     15-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu
 \par      code %:     50%
 \par      changes:    Declared the cat script class and added functions for the 
											 player cat's attack states.

 \co-author            Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 \par      code %:     50%
 \par      changes:    21-11-2023
											 Added functions for the player cat's movement states.

 \brief
	This file contains declarations for functions used for a grey player cat's states.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "StateManager.h"
#include "GameStateManager.h"
#include "Events/EventHandler.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/SceneView.h"
#include "Logic/LogicSystem.h"

namespace PE
{
	//! Enum denoting cardinal directions for attack
	enum EnumCatAttackDirection
	{
		NONE = 0,
		EAST = 1,
		NORTH = 2,
		WEST = 3,
		SOUTH = 4
	};

	//! struct with variables needed by cat scripts
	struct CatScriptData
	{
		// Set to true if the entity this script is attached to should be treated as the main cat
		bool isMainCat{ false }; // The main cat (controls the movement of the chain). Only one main cat can exist in the scene at a time. 
		bool isFollowing{ false }; // Set to true if this cat is currently following another cat

		// reference entities
		EntityID catID{ 0 }; // ID of entity that this script belongs to
		EntityID projectileID{ 0 }; // ID of projectile entity for player attack

		// cat stats
		int catHealth{ 1 };		
		int catMaxEnergy{ 21 };

		// attack variables
		int attackDamage{ 1 };
		int requiredAttackPoints{ 0 }; // Amount of attack points the player needs in order to execute this attack
		EnumCatAttackDirection attackDirection{ EnumCatAttackDirection::NONE }; // Direction of attack chosen
		std::map<EnumCatAttackDirection, EntityID> telegraphIDs; // IDs of entities used to visualise the directions the player can attack in
		
		// bullet variables
		float bulletDelay{ 0.7f };
		float bulletRange{ 3.f };
		float bulletLifeTime{ 0.1f };
		float bulletForce{ 1000.f };

		// movement variables
		float minDistance{ 10.f }; float maxDistance{ 50.f }; // Min and max distance enforced between each path node
		float nodeSize{ 10.f }; // Size (in pixels) of each node
		float movementSpeed{ 300.f }; // Speed to move the cat along the path
		float forgivenessOffset{ 1.f }; // Amount that the cat can be offset from the path node before attempting to move to the next one
		unsigned currentPositionIndex{}; // Index of the position in the pathPositions container that the cat should move towards
		
		std::vector<vec2> pathPositions{}; // Positions of the nodes of the player drawn path
		std::vector<vec2> followCatPositions{}; // positions for the follower cats in the cat chain to use
		std::vector<EntityID> pathQuads{}; // IDs of entities to visualise the path nodes

		// state manager
		StateMachine* p_stateManager{ nullptr }; // Cat state manager. Deleted in destructor.
		bool shouldChangeState{};  // Flags that the state should change when [timeBeforeChangingState] is zero
		bool delaySet{ false }; // Whether the state change has been flagged
		float timeBeforeChangingState{0.f}; // Delay before state should change
		bool finishedExecution{ false }; // Keeps track of whether the execution phase has been completed

		std::map<std::string, std::string> animationStates; // Animation states the cat should have. <name, animation file>
	};


	// ----- CAT SCRIPT ----- //
	class CatScript : public Script
	{
	public:
		// ----- Public Variables ------ //

		std::map<EntityID, CatScriptData> m_scriptData; // Data associated with each instance of the script

	public:

		// ----- Destructor ----- //
		virtual ~CatScript(); // Deletes the statemanager.

		// ----- Public Functions ----- //

		/*!***********************************************************************************
		 \brief Initializes the script, spawning all necessary entities.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief Called every frame. Updates the animations of the cat.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		 \param[in] deltaTime - Time in seconds since the last frame.
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);


		/*!***********************************************************************************
		 \brief Does nothing.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		virtual void Destroy(EntityID id){}

		/*!***********************************************************************************
		 \brief Called when first attached to an entity. Used to reset to default values.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		 \brief Called removed from an entity. Used to clean up the script.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Creates a statemanager and sets the starting state. If the statemanager exists,
						don't bother making a new one.

		 \param[in] id - EntityID of the entity to en/disable.
		 \param[in] setToActive - Whether this entity should be set to active or inactive.
		*************************************************************************************/
		void MakeStateManager(EntityID id);

		/*!***********************************************************************************
		 \brief Sets the flag for the state to be changed after the delay passed in.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] stateChangeDelay - Time in seconds before switching to the next state. 
										Set to zero by default.
		*************************************************************************************/
		void TriggerStateChange(EntityID id, float const stateChangeDelay = 0.f);

		/*!***********************************************************************************
		 \brief Changes the state if the flag has been set and the delay is zero and below.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] deltaTime - Time in seconds that have passed since the last frame.

		 \return true - True if the state should change.
		 \return false - False if the state should NOT change.
		*************************************************************************************/
		bool CheckShouldStateChange(EntityID id, float const deltaTime);

		/*!***********************************************************************************
		 \brief Helper function to en/disables an entity.

		 \param[in] id - EntityID of the entity to en/disable.
		 \param[in] setToActive - Whether this entity should be set to active or inactive.
		*************************************************************************************/
		static void ToggleEntity(EntityID id, bool setToActive);

		/*!***********************************************************************************
		 \brief Adjusts the position of the transform to the value passed in.

		 \param[in] transformId - ID of the entity to update the transform of.
		 \param[in] r_position - Position to set the transform to.
		*************************************************************************************/
		static void PositionEntity(EntityID const transformId, vec2 const& r_position);

		/*!***********************************************************************************
		 \brief Adjusts the scale of the transform to the value passed in.

		 \param[in] transformId - ID of the entity to update the transform of.
		 \param[in] width - Width to set the transform to.
		 \param[in] height - Height to set the transform to.
		*************************************************************************************/
		static void ScaleEntity(EntityID const transformId, float const width, float const height);


		// ----- Getters/RTTR ----- //

		/*!***********************************************************************************
		 \brief Returns the position of the transform of the entity passed in.

		 \param[in] transformId - ID of the entity to retrieve the position of.
		*************************************************************************************/
		static vec2 GetEntityPosition(EntityID const transformId);

		/*!***********************************************************************************
		 \brief Returns the scale of the transform of the entity passed in.

		 \param[in] transformId - ID of the entity to retrieve the scale of.
		*************************************************************************************/
		static vec2 GetEntityScale(EntityID const transformId);

		/*!***********************************************************************************
		 \brief Returns the position of the cursor in the world.

		 \return vec2 - Returns the position of the cursor in the world.
		*************************************************************************************/
		static vec2 GetCursorPositionInWorld();

		/*!***********************************************************************************
		 \brief Returns the current energy the player has left.

		 \return int - Returns the current energy the player has left.
		*************************************************************************************/
		static inline int GetCurrentEnergyLevel() { return m_catEnergy; }
		
		/*!***********************************************************************************
		 \brief Returns the maximum energy that the cat can have.

		 \return int - Returns the maximum energy that the cat can have.
		*************************************************************************************/
		static inline int GetMaximumEnergyLevel() { return m_catMaxEnergy; }
		
		/*!***********************************************************************************
		 \brief Returns the starting maximum energy that the cat can have.

		 \return int - Returns the starting maximum energy that the cat can have.
		*************************************************************************************/
		static inline int GetBaseMaximumEnergyLevel() { return m_catBaseMaxEnergy; }

		/*!***********************************************************************************
		 \brief Get the ID of the main cat

		 \return EntityID - ID of the main cat
		*************************************************************************************/
		static EntityID GetMainCat() { return m_mainCatId; }

		/*!***********************************************************************************
		 \brief Identifies if the entity passed in is a cat. Assumes that cats have
						the keyword "Cat" somewhere in their name in entityDescriptor.

		 \param[in] id - ID of the entity to check.
		*************************************************************************************/
		static bool IsCat(EntityID const id);

		/*!***********************************************************************************
		 \brief Identifies if the entity passed in is an enemy. Assumes that enemies have
						the keyword "Rat" somewhere in their name in entityDescriptor.

		 \param[in] id - ID of the entity to check.
		*************************************************************************************/
		static bool IsEnemy(EntityID const id);

		/*!***********************************************************************************
		 \brief Identifies if the entity passed in is an obstacle. Assumes that obstacles
						have the keyword "Obstacle" somewhere in their name in entityDescriptor.

		 \param[in] id - ID of the entity to check.
		*************************************************************************************/
		static bool IsObstacle(EntityID const id);

		/*!***********************************************************************************
		 \brief Identifies if the cat state passed in matches the game state passed in.

		 \param[in] catStateName - Name of the cat's state.
		 \param[in] gameState - Name of the game state to compare with the cat's state.
		*************************************************************************************/
		static bool DoesGameStateMatchCatState(std::string const& catStateName, GameStates gameState);

		/*!***********************************************************************************
		 \brief Sets the energy the player has left.

		 \param[in] int - Energy level to set.
		*************************************************************************************/
		static void SetCurrentEnergyLevel(int const energyLevel) { m_catEnergy = energyLevel; }
		
		/*!***********************************************************************************
		 \brief Sets the maximum energy the player can have.

		 \param[in] int - Energy level to set.
		*************************************************************************************/
		static void SetMaximumEnergyLevel(int const energyLevel) { m_catMaxEnergy = energyLevel; }
				
		/*!***********************************************************************************
		 \brief Stores the ID of the main cat

		 \param[in] mainCatId - ID of cat to be used as the main cat
		*************************************************************************************/
		static void SetMainCat(EntityID mainCatId) { m_mainCatId = mainCatId; }

		/*!***********************************************************************************
		 \brief Reduces the health of the player

		 \param[in] id - ID of the cat to reduce the health of
		 \param[in] damageTaken - Amount of damage taken by the player
		*************************************************************************************/
		void LoseHP(EntityID id, int damageTaken);

		/*!***********************************************************************************
		 \brief Returns the container of script data.
		*************************************************************************************/
		std::map<EntityID, CatScriptData>& GetScriptData(){ return m_scriptData; }

		rttr::instance GetScriptData(EntityID id){ return rttr::instance(m_scriptData.at(id)); }
	

	private:

	  // ----- Private Variables ----- //
		static int m_catEnergy; // Current energy level of the cat
		static int m_catMaxEnergy; // Maximum energy of the cat
		static int m_catBaseMaxEnergy; // Starting maximum energy of the cat
		static EntityID m_mainCatId; // ID of the main cat


	private:

		// ----- Private Functions ----- //
		
		/*!***********************************************************************************
		 \brief Helper function for creating entities which are for the cat's attack telegraphs

		 \param[in] id			EntityID of the cat which the telegraphs are for
		 \param[in] isXAxis		bool to check if the box is elongated by its x Axis
		 \param[in] isNegative	bool to check if the box is positioned on the west or south of the cat
								if it is lengthen by its width or height respectively
		*************************************************************************************/
		void CreateAttackTelegraphs(EntityID id, bool isSide, bool isNegative);


		/*!***********************************************************************************
		 \brief Helper function for creating entities for the cat's attack projectile

		 \param[in] id			EntityID of the cat which the projectile belongs to
		*************************************************************************************/
		void CreateProjectile(EntityID id);


		/*!***********************************************************************************
		 \brief Creates a path node to visualise the path drawn by the player. 

		 \param[in] id EntityID of the entity that this script is attached to.
		*************************************************************************************/
		void CreatePathNode(EntityID id);
	};
}