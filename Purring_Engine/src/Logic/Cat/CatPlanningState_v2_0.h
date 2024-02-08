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
#include "Logic/LogicSystem.h"
#include "Logic/StateManager.h"

#include "Events/MouseEvent.h"
#include "Events/CollisionEvent.h"

#include "CatAttackBase_v2_0.h"
#include "CatMovementStates_v2_0.h"


namespace PE
{
	class Cat_v2_0PLAN : public State
	{
	public:
		// ----- Destructor ----- //
		Cat_v2_0PLAN() = delete;

		Cat_v2_0PLAN(CatAttackBase_v2_0* p_catAttackTypePLAN, CatMovement_v2_0PLAN* p_catMovementPLAN) : p_catAttack{ p_catAttackTypePLAN }, p_catMovement{ p_catMovementPLAN }, p_data{ nullptr } {}

		virtual ~Cat_v2_0PLAN() 
		{
			delete p_catMovement;
			delete p_catAttack;
		}

		virtual void StateEnter(EntityID id) override;

		virtual void StateUpdate(EntityID id, float deltaTime) override;

		virtual void StateCleanUp();

		virtual void StateExit(EntityID id) override;

		virtual std::string_view GetName() { return "PLANNING"; }

	private:
		CatAttackBase_v2_0* p_catAttack{ nullptr };
		CatMovement_v2_0PLAN* p_catMovement{ nullptr };
		CatScript_v2_0Data* p_data{ nullptr };

		int m_mouseClickEventListener{};
		int m_mouseReleaseEventListener{};

		bool m_mouseRelease{ false };
		bool m_mouseClicked{ false };
		bool m_mouseClickPrevious{ false };
		//bool m_rightMouseClicked{ false };

		bool m_planningAttack{ false };

		int m_doubleClick{ 0 };
		float m_timer{ 0.f };

		vec2 m_prevCursorPosition{ 0.f, 0.f };

		void OnMouseClick(const Event<MouseEvents>& r_ME);
		void OnMouseRelease(const Event<MouseEvents>& r_ME);
	};
}