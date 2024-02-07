/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatPlanningState_v2_0.cpp
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Planning State

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"

#include "ECS/Entity.h"
#include "Events/MouseEvent.h"
#include "Physics/CollisionManager.h"

#include "CatPlanningState_v2_0.h"
#include "GreyCatAttackStates_v2_0.h"
#include "CatHelperFunctions.h"

namespace PE
{
	void Cat_v2_0PLAN::StateEnter(EntityID id)
	{
		// initializes the cat movement planning sub state
		p_catMovement->Enter(id);

		// initializes the cat attack planning sub state
		p_catAttack->Enter(id);

		m_mouseClickEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, Cat_v2_0PLAN::OnMouseClick, this);
		m_mouseHoldEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonHold, Cat_v2_0PLAN::OnMouseHeld, this);
		
	}

	void Cat_v2_0PLAN::StateUpdate(EntityID id, float deltatime)
	{
		/*CircleCollider const& r_catCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);

		if (m_mouseClicked && PointCollision(r_catCollider, CatHelperFunctions::GetCursorPositionInWorld()))
		{
			std::cout << "set\n";
			m_planningAttack = true;
		}

		if (m_planningAttack)
		{
			p_catAttack->Update(id, deltatime);
		}
		else
		{
			p_catMovement->Update(id, deltatime);
		}*/
		p_catMovement->Update(id, deltatime);
		//p_catAttack->Update(id, deltatime);

		m_mouseClicked = false;
	}

	void Cat_v2_0PLAN::StateCleanUp()
	{
		p_catMovement->CleanUp();
		p_catAttack->CleanUp();
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseClickEventListener);
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseHoldEventListener);
	}

	void Cat_v2_0PLAN::StateExit(EntityID id)
	{
		p_catMovement->Exit(id);
		p_catAttack->Exit(id);
	}

	void Cat_v2_0PLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		m_mouseClicked = true;
	}
	
	void Cat_v2_0PLAN::OnMouseHeld(const Event<MouseEvents>& r_ME)
	{
		MouseButtonHoldEvent MBHE = dynamic_cast<const MouseButtonHoldEvent&>(r_ME);
		m_mouseHold = true;
		if (MBHE.button != 1 && MBHE.button != 2 && MBHE.button != 3)
		{
			m_mouseHold = false;
		}
	}
}
