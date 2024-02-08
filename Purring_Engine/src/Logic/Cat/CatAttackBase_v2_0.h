/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatPlanningState_v2_0.h
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Planning State

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "ECS/Entity.h"

namespace PE
{
	struct CatAttackBase_v2_0
	{
		virtual ~CatAttackBase_v2_0() {}

		virtual void Enter(EntityID id) = 0;

		virtual void Update(EntityID id, float deltaTime) = 0;

		virtual void CleanUp() = 0;

		virtual void Exit(EntityID id) = 0;

		virtual void ResetSelection(EntityID id) = 0;
		virtual void ToggleTelegraphs(bool setToggle, bool ignoreSelected) = 0;
	};
}