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
		int catHealth{ 0 };		
		int catEnergy{ 0 };
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
		float bulletForce{ 1500.f };

		// movement variables
		std::vector<vec2> pathPositions{};
		std::vector<EntityID> pathQuads{};

		// state manager
		StateMachine* p_stateManager;
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
		 \brief Helper function to en/disables an entity.

		 \param[in] id EntityID of the entity to en/disable.
		 \param[in] setToActive Whether this entity should be set to active or inactive.
		*************************************************************************************/
		static void ToggleEntity(EntityID id, bool setToActive);

		// ----- Getters/RTTR ----- //
		std::map<EntityID, CatScriptData>& GetScriptData(){ return m_scriptData; }

		rttr::instance GetScriptData(EntityID id){ return rttr::instance(m_scriptData.at(id)); }
	
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


		/*!***********************************************************************************
		 \brief Calls the reset function of the movement or attack planning states.

		 \param[in] id EntityID of the entity that this script is attached to.
		*************************************************************************************/
		void ResetValues(EntityID id);
	};
}