/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GreyCatAttackStates_v2_0.h
 \date     3-2-2024

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
	//! Enum denoting cardinal directions for attack
	enum EnumCatAttackDirection
	{
		NONE = 0,
		EAST = 1,
		NORTH = 2,
		WEST = 3,
		SOUTH = 4
	};

	struct GreyCatAttackVariables
	{
		// damage of the attack
		int damage;

		// Telegraph variables
		EnumCatAttackDirection attackDirection{ EnumCatAttackDirection::NONE }; // Direction of attack chosen
		std::map<EnumCatAttackDirection, EntityID> telegraphIDs; // IDs of entities used to visualise the directions the player can attack in
		
		// Telegraph colors
		vec3 const m_defaultColor{ 0.545f, 1.f, 0.576f };
		vec3 const m_hoverColor{ 1.f, 0.859f, 0.278f };
		vec3 const m_selectColor{ 1.f, 0.784f, 0.f };

		// projectile variables
		float bulletDelay{ 0.7f };
		float bulletRange{ 3.f };
		float bulletLifeTime{ 1.f };
		float bulletForce{ 1000.f };
	};

	class GreyCatAttack_v2_0PLAN : public State
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
		
		GreyCatAttackVariables* p_attackData;

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

		void ResetSelection();
	};

	class GreyCatAttack_v2_0EXECUTE : public State
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
		
		GreyCatAttackVariables* p_data;

		float m_attackDuration; // how long attack will last
		int m_collisionEventListener;

		virtual void AttackHitCat(const Event<CollisionEvents>& r_CE);

		virtual void AttackHitRat(const Event<CollisionEvents>& r_CE);
	};
}