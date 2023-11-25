/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatAttackScript.h
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
		// reference entities
		EntityID catID{ 0 };
		EntityID projectileID{ 0 };

		// cat stats
		int catHealth{ 1 };		
		int catMaxEnergy{ 21 };

		// attack variables
		int attackDamage{ 0 };
		int requiredAttackPoints{ 0 };
		EnumCatAttackDirection attackDirection{ EnumCatAttackDirection::NONE };
		std::map<EnumCatAttackDirection, EntityID> telegraphIDs;
		
		// bullet variables
		float bulletDelay{ 0.7f };
		float bulletRange{ 3.f };
		float bulletLifeTime{ 0.1f };
		float bulletForce{ 1000.f };

		// movement variables
		float minDistance{ 30.f }; float maxDistance{ 50.f };
		float nodeSize{ 10.f }; // Size (in pixels) of each node
		float movementSpeed{ 300.f }; // Speed to move the cat along the path
		float forgivenessOffset{ 1.f }; // Amount that the cat can be offset from the path node before attempting to move to the next one
		unsigned currentPositionIndex{}; // Index of the position in the pathPositions container that the cat should move towards
		
		std::vector<vec2> pathPositions{};
		std::vector<EntityID> pathQuads{};

		// state manager
		StateMachine* p_stateManager;
		bool shouldChangeState{};
		bool delaySet{ false };
		float timeBeforeChangingState{0.f};
	};


	// ----- CAT SCRIPT ----- //
	class CatScript : public Script
	{
	public:
		// ----- Public Variables ------ //

		std::map<EntityID, CatScriptData> m_scriptData;

	public:

		// ----- Destructor ----- //
		virtual ~CatScript();

		// ----- Public Functions ----- //
		virtual void Init(EntityID id);

		virtual void Update(EntityID id, float deltaTime);

		virtual void Destroy(EntityID id){}

		virtual void OnAttach(EntityID id);

		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Creates a statemanager and sets the starting state. If the statemanager exists,
						don't bother making a new one.

		 \param[in] id EntityID of the entity to en/disable.
		 \param[in] setToActive Whether this entity should be set to active or inactive.
		*************************************************************************************/
		void MakeStateManager(EntityID id);

		/*!***********************************************************************************
		 \brief Sets the flag for the state to be changed after the delay passed in.

		 \param[in] id EntityID of the entity to change the state of.
		 \param[in] stateChangeDelay Time in seconds before switching to the next state. 
										Set to zero by default.
		*************************************************************************************/
		void TriggerStateChange(EntityID id, float const stateChangeDelay = 0.f);

		/*!***********************************************************************************
		 \brief Changes the state if the flag has been set and the delay is zero and below.

		 \param[in] id EntityID of the entity to change the state of.
		 \param[in] deltaTime Time in seconds that have passed since the last frame.

		 \return true - True if the state should change.
		 \return false - False if the state should NOT change.
		*************************************************************************************/
		bool CheckShouldStateChange(EntityID id, float const deltaTime);

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
		 \brief Returns the current energy the player has left.

		 \return int - Returns the current energy the player has left.
		*************************************************************************************/
		static inline int GetCurrentEnergyLevel() { return catEnergy; }
		
		/*!***********************************************************************************
		 \brief Returns the maximum energy that the cat can have.

		 \return int - Returns the maximum energy that the cat can have.
		*************************************************************************************/
		static inline int GetMaximumEnergyLevel() { return catMaxEnergy; }
		
		/*!***********************************************************************************
		 \brief Sets the energy the player has left.

		 \param[in] int - Energy level to set.
		*************************************************************************************/
		static void SetCurrentEnergyLevel(int const energyLevel) { catEnergy = energyLevel; }
		
		/*!***********************************************************************************
		 \brief Sets the maximum energy the player can have.

		 \param[in] int - Energy level to set.
		*************************************************************************************/
		static void SetMaximumEnergyLevel(int const energyLevel) { catMaxEnergy = energyLevel; }


		std::map<EntityID, CatScriptData>& GetScriptData(){ return m_scriptData; }

		rttr::instance GetScriptData(EntityID id){ return rttr::instance(m_scriptData.at(id)); }
	

	private:

	  // ----- Private Variables ----- //
		static int catEnergy; // Current energy level of the cat
		static int catMaxEnergy; // Maximum energy of the cat


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


		void CreateProjectile(EntityID id);


		/*!***********************************************************************************
		 \brief Creates a path node to visualise the path drawn by the player. 

		 \param[in] id EntityID of the entity that this script is attached to.
		*************************************************************************************/
		void CreatePathNode(EntityID id);
	};
}