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

		// cat attack


		// movement variables
		int catMaxMovementEnergy{ 21 };
		float minDistance{ 10.f }; float maxDistance{ 50.f }; // Min and max distance enforced between each path node
		float nodeSize{ 10.f }; // Size (in pixels) of each node
		float movementSpeed{ 300.f }; // Speed to move the cat along the path
		float forgivenessOffset{ 1.f }; // Amount that the cat can be offset from the path node before attempting to move to the next one
		unsigned currentPositionIndex{}; // Index of the position in the pathPositions container that the cat should move towards

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
	};

	struct CatAttack
	{
		virtual void Attack();
	};

	class ProjectileAttack : public CatAttack
	{
		virtual void Attack(){}
	};
}