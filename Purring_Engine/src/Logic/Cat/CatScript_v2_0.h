/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatScript_v2_0.h
 \date     15-1-2024

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu
 \par      code %:     50%
 \par      changes:    Defined the cat script class and added functions for the
											 player cat's attack states.

 \co-author            Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 \par      code %:     50%
 \par      changes:    21-11-2023
											 Added functions for the player cat's movement states.

 \brief
	This file contains the structs and declarations of functions for the base of a cat object.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "ECS/Entity.h"
#include "Logic/LogicSystem.h"
#include "Logic/StateManager.h"
#include "CatController_v2_0.h"

namespace PE
{
	struct CatScript_v2_0Data
	{
		// id of cat and its type
		EntityID catID{ 0 };
		EnumCatType catType{ EnumCatType::GREYCAT };
		EnumCatState currentCatState{};

		// cat attack
		Script* p_catAttack = nullptr;

		// movement variables
		int catMaxMovementEnergy{ 21 };
		int catCurrentEnergy{ catMaxMovementEnergy };

		float minDistance{ 10.f }; float maxDistance{ 50.f }; // Min and max distance enforced between each path node
		float nodeSize{ 10.f }; // Size (in pixels) of each node
		float movementSpeed{ 300.f }; // Speed to move the cat along the path
		float forgivenessOffset{ 1.f }; // Amount that the cat can be offset from the path node before attempting to move to the next one
		unsigned currentPositionIndex{}; // Index of the position in the pathPositions container that the cat should move towards
		
		std::vector<vec2> pathPositions{}; // Positions of the nodes of the player drawn path
		std::vector<vec2> followCatPositions{}; // positions for the follower cats in the cat chain to use
		std::vector<EntityID> pathQuads{}; // IDs of entities to visualise the path nodes

		// state manager
		StateMachine* p_stateManager{ nullptr };

		// animation
		std::map<std::string, std::string> animationStates; // animation states of the cat <name, file>
	};

	class CatScript_v2_0 : public Script
	{
	public:
		// ----- Public Variables ----- //
		std::map<EntityID, CatScript_v2_0Data> m_scriptData; // data associated with each instance of the script
		
	public:

		virtual ~CatScript_v2_0();

		virtual void Init(EntityID id);

		virtual void Update(EntityID id, float deltaTime);

		virtual void Destroy(EntityID id){}

		virtual void OnAttach(EntityID id);
		
		virtual void OnDetach(EntityID id);


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
		 \brief Returns the container of script data.
		*************************************************************************************/
		std::map<EntityID, CatScript_v2_0Data>& GetScriptData() { return m_scriptData; }

		rttr::instance GetScriptData(EntityID id) { return rttr::instance(m_scriptData.at(id)); }
		
	private:
		// ----- Private Functions ----- //

		/*!***********************************************************************************
		 \brief Creates a path node to visualise the path drawn by the player.

		 \param[in] id EntityID of the entity that this script is attached to.
		*************************************************************************************/
		void CreatePathNode(EntityID id);
		
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

	};
}