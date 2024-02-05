/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GreyCatAttackStates_v2_0.h
 \date     21-11-2023

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Attack Base class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "CatAttackBase_v2_0.h"

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

	class GreyCatAttack_v2_0PLAN : public CatAttack_v2_0PLAN_Base
	{
	public:
		// ----- Destructor ----- //
		virtual ~GreyCatAttack_v2_0PLAN();

		virtual void StateEnter(EntityID id);

		virtual void StateUpdate(EntityID id, float deltaTime);

		virtual void StateCleanUp();

		virtual void StateExit(EntityID id);

		virtual std::string_view GetName();
		
	private:
	};

	class GreyCatAttack_v2_0EXECUTE : public CatAttack_v2_0EXECUTE_Base
	{
	public:
		// ----- Destructor ----- //
		virtual ~GreyCatAttack_v2_0EXECUTE();

		virtual void StateEnter(EntityID id);

		virtual void StateUpdate(EntityID id, float deltaTime);

		virtual void StateCleanUp();

		virtual void StateExit(EntityID id);

		virtual std::string_view GetName();

	private:

	};

	struct GreyCatAttackVariables : CatAttackVariables_Base
	{
		// Telegraph variables
		EnumCatAttackDirection attackDirection{ EnumCatAttackDirection::NONE }; // Direction of attack chosen
		std::map<EnumCatAttackDirection, EntityID> telegraphIDs; // IDs of entities used to visualise the directions the player can attack in
		
		// projectile variables
		float bulletDelay{ 0.7f };
		float bulletRange{ 3.f };
		float bulletLifeTime{ 1.f };
		float bulletForce{ 1000.f };
	};
}