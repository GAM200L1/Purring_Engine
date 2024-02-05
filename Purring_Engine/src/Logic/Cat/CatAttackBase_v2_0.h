/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatAttackBase_v2_0.h
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
#include "CatController_v2_0.h"
#include "Events/MouseEvent.h"
#include "Events/CollisionEvent.h"

namespace PE
{
	struct CatAttackVariables_Base
	{
		int damage;
	};

	class CatAttack_v2_0PLAN_Base : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~CatAttack_v2_0PLAN_Base() = 0;

		virtual void StateEnter(EntityID id) = 0;

		virtual void StateUpdate(EntityID id, float deltaTime) = 0;

		virtual void StateCleanUp() = 0;

		virtual void StateExit(EntityID id) = 0;

		virtual std::string_view GetName() = 0;

	private:
		CatAttackVariables_Base* p_attackVariables{ nullptr };
		
		vec3 const m_defaultColor{ 0.545f, 1.f, 0.576f };
		vec3 const m_hoverColor{ 1.f, 0.859f, 0.278f };
		vec3 const m_selectColor{ 1.f, 0.784f, 0.f };

		bool m_showTelegraphs{ false }; // True if telegraphs are to be displayed
		
		bool m_mouseClick{ false }; // set to true when mouse is clicked
		bool m_mouseClickedPrevious{ false }; // Set to true if the mouse was pressed in the previous frame, false otherwise
		int m_mouseEventListener; // Stores the handler for the mouse click event
		int m_triggerEnterEventListener; // Stores the handler for the collision enter event
		int m_triggerStayEventListener; // Stores the handler for the collision stay event

		void OnMouseClick(const Event<MouseEvents>& r_ME)
		{
			MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
			m_mouseClickedPrevious = m_mouseClick;
			m_mouseClick = true;
		}
		virtual void ResetSelection() = 0;
	};

	class CatAttack_v2_0EXECUTE_Base : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~CatAttack_v2_0EXECUTE_Base() = 0;

		virtual void StateEnter(EntityID id) = 0;

		virtual void StateUpdate(EntityID id, float deltaTime) = 0;

		virtual void StateCleanUp() = 0;

		virtual void StateExit(EntityID id) = 0;

		virtual std::string_view GetName() = 0;

	private:
		CatAttackVariables_Base* p_attackVariables{ nullptr };
		
		float m_attackDuration; // how long attack will last
		int m_collisionEventListener;

		void AttackHitCat(const Event<CollisionEvents>& r_CE)
		{
			if (r_CE.GetType() == CollisionEvents::OnCollisionEnter)
			{
				OnCollisionEnterEvent OCEE = dynamic_cast<const OnCollisionEnterEvent&>(r_CE);
			}
		}

		virtual void AttackHitRat(const Event<CollisionEvents>& r_CE) = 0;
	};

	struct CatAttackStates
	{
		CatAttack_v2_0PLAN_Base* attackPlan = nullptr;
		CatAttack_v2_0EXECUTE_Base* attackExecute = nullptr;
	};
}