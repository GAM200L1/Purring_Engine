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
#include "CatAttackBase_v2_0.h"

namespace PE
{
	struct OrangeCatAttackVariables
	{
		EntityID seismicID{ 0 }; // id of the seismic attack
		EntityID telegraphID{ 0 }; // id of the UI

		int damage{ 0 };

		// projectile variables
		float stompRadius{ 20.f };
		float stompLifeTime{ 1.f };
		float stomopForce{ 1000.f };
	};

	class OrangeCatAttack_v2_0PLAN : public CatAttackBase_v2_0
	{
	public:
		// ----- Destructor ----- //
		virtual ~OrangeCatAttack_v2_0PLAN() { p_attackData = nullptr; }

		virtual void Enter(EntityID id);

		virtual void Update(EntityID id, float deltaTime);

		virtual void CleanUp();

		virtual void Exit(EntityID id);

		virtual void ResetSelection(EntityID id);

		virtual void ToggleTelegraphs(bool setToggle, bool ignoreSelected);

		virtual void ForceZeroMouse() { m_mouseClicked = 0; }

	private:
		
		// data
		OrangeCatAttackVariables* p_attackData;

		// Telegraph colors
		vec3 const m_defaultColor{ 0.545f, 1.f, 0.576f };
		vec3 const m_hoverColor{ 1.f, 0.859f, 0.278f };
		vec3 const m_selectColor{ 1.f, 0.784f, 0.f };

		// checks
		bool m_mouseClicked{ false }; // Set to true when the mouse is pressed, false otherwise
		bool m_mouseClickedPrevious{ false }; // Set to true if the mouse was pressed in the previous frame, false otherwise
		int m_mouseEventListener; // Stores the handler for the mouse click event
		//int m_triggerEnterEventListener; // Stores the handler for the collision enter event
		//int m_triggerStayEventListener; // Stores the handler for the collision stay event

		// ----- Private Functions ----- //
		void OnMouseClick(const Event<MouseEvents>& r_ME);

	};

	class OrangeCatAttack_v2_0EXECUTE : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~OrangeCatAttack_v2_0EXECUTE() { p_attackData = nullptr; }

		virtual void StateEnter(EntityID id);

		virtual void StateUpdate(EntityID id, float deltaTime);

		virtual void StateCleanUp();

		virtual void StateExit(EntityID id);

		virtual std::string_view GetName() { return "AttackEXECUTE"; }

	private:
		OrangeCatAttackVariables* p_attackData;

		int m_collisionEventListener;

		void SlamHitCat(const Event<CollisionEvents>& r_CE);

		void SlamHitRat(const Event<CollisionEvents>& r_CE);
	};
}