/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     OrangeCatAttackStates_v2_0.h
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
	class OrangeCatAttack_v2_0PLAN : public CatAttack_v2_0PLAN_Base
	{
	public:
		// ----- Destructor ----- //
		virtual ~OrangeCatAttack_v2_0PLAN();

		virtual void StateEnter(EntityID id);

		virtual void StateUpdate(EntityID id, float deltaTime);

		virtual void StateCleanUp();

		virtual void StateExit(EntityID id);

		virtual std::string_view GetName();

		virtual void ResetSelection();

	private:
	};

	class OrangeCatAttack_v2_0EXECUTE : public CatAttack_v2_0EXECUTE_Base
	{
	public:
		// ----- Destructor ----- //
		virtual ~OrangeCatAttack_v2_0EXECUTE();

		virtual void StateEnter(EntityID id);

		virtual void StateUpdate(EntityID id, float deltaTime);

		virtual void StateCleanUp();

		virtual void StateExit(EntityID id);

		virtual std::string_view GetName();

	private:

	};

	struct OrangeCatAttackVariables : CatAttackVariables_Base
	{
		EntityID telegraphID; // id of the UI

		// projectile variables
		float stompRadius{ 20.f };
		float stompLifeTime{ 1.f };
		float stomopForce{ 1000.f };
	};
}