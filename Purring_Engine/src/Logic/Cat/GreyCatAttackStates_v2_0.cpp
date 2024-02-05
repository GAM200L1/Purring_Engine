/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GreyCatAttackStates_v2_0.cpp
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Attack Base class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "GreyCatAttackStates_v2_0.h"

namespace PE
{

	// ----- ATTACK PLAN ----- //

	void GreyCatAttack_v2_0PLAN::StateEnter(EntityID id)
	{

	}

	void GreyCatAttack_v2_0PLAN::StateUpdate(EntityID id, float deltaTime)
	{

	}

	void GreyCatAttack_v2_0PLAN::StateCleanUp()
	{

	}

	void GreyCatAttack_v2_0PLAN::StateExit(EntityID id)
	{

	}

	void GreyCatAttack_v2_0PLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{

	}

	void GreyCatAttack_v2_0PLAN::ResetSelection()
	{

	}

	// ----- ATTACK EXECUTE ----- //

	void GreyCatAttack_v2_0EXECUTE::StateEnter(EntityID id)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::StateUpdate(EntityID id, float deltaTime)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::StateCleanUp()
	{

	}

	void GreyCatAttack_v2_0EXECUTE::StateExit(EntityID id)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::ProjectileHitCat(const Event<CollisionEvents>& r_CE)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::ProjectileHitRat(const Event<CollisionEvents>& r_CE)
	{

	}
}