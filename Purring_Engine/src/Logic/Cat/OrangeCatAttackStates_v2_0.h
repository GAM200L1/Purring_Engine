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
#include "Logic/LogicSystem.h"
#include "Logic/StateManager.h"

#include "Events/MouseEvent.h"
#include "Events/CollisionEvent.h"

#include "CatScript_v2_0.h"

namespace PE
{
	struct OrangeCatAttackVariables
	{
		int damage;

		EntityID telegraphID; // id of the UI

		// projectile variables
		float stompRadius{ 20.f };
		float stompLifeTime{ 1.f };
		float stomopForce{ 1000.f };
	};

	class OrangeCatAttack_v2_0PLAN : public State
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

		// Telegraph colors
		vec3 const m_defaultColor{ 0.545f, 1.f, 0.576f };
		vec3 const m_hoverColor{ 1.f, 0.859f, 0.278f };
		vec3 const m_selectColor{ 1.f, 0.784f, 0.f };
	};

	class OrangeCatAttack_v2_0EXECUTE : public State
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
}