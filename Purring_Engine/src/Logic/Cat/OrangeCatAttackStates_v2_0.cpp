/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     OrangeCatAttackStates_v2_0.cpp
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Attack Base class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "OrangeCatAttackStates_v2_0.h"

namespace PE
{

	// ----- ATTACK PLAN ----- //

	void OrangeCatAttack_v2_0PLAN::Enter(EntityID id)
	{

	}

	void OrangeCatAttack_v2_0PLAN::Update(EntityID id, float deltaTime)
	{

	}

	void OrangeCatAttack_v2_0PLAN::CleanUp()
	{

	}

	void OrangeCatAttack_v2_0PLAN::Exit(EntityID id)
	{

	}

	void OrangeCatAttack_v2_0PLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{

	}

	void OrangeCatAttack_v2_0PLAN::ResetSelection(EntityID id)
	{

	}

	// ----- ATTACK EXECUTE ----- //

	void OrangeCatAttack_v2_0EXECUTE::StateEnter(EntityID id)
	{

	}

	void OrangeCatAttack_v2_0EXECUTE::StateUpdate(EntityID id, float deltaTime)
	{

	}

	void OrangeCatAttack_v2_0EXECUTE::StateCleanUp()
	{

	}

	void OrangeCatAttack_v2_0EXECUTE::StateExit(EntityID id)
	{

	}

	void OrangeCatAttack_v2_0EXECUTE::SlamHitCat(const Event<CollisionEvents>& r_CE)
	{

	}

	void OrangeCatAttack_v2_0EXECUTE::SlamHitRat(const Event<CollisionEvents>& r_CE)
	{

	}
}