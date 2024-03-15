/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatPlanningState_v2_0.cpp
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the definition for functions for the Cat Planning State

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"

#include "ECS/Entity.h"
#include "Events/MouseEvent.h"
#include "Physics/CollisionManager.h"
#include "AudioManager/GlobalMusicManager.h"

#include "CatPlanningState_v2_0.h"
#include "GreyCatAttackStates_v2_0.h"
#include "CatHelperFunctions.h"
#include "CatController_v2_0.h"

namespace PE
{
	void Cat_v2_0PLAN::StateEnter(EntityID id)
	{
		if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsFollowCat(id) || GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCatCaged(id)) { return; } // if cat is following cat or cage cat in the chain

		p_data = GETSCRIPTDATA(CatScript_v2_0, id);
		
		EntityManager::GetInstance().Get<Collider>(p_data->catID).isTrigger = true;

		// initializes the cat movement planning sub state
		p_catMovement->Enter(id);

		// initializes the cat attack planning sub state
		p_catAttack->Enter(id);

		m_mouseClickEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, Cat_v2_0PLAN::OnMouseClick, this);
		m_mouseReleaseEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonReleased, Cat_v2_0PLAN::OnMouseRelease, this);
	}

	void Cat_v2_0PLAN::StateUpdate(EntityID id, float deltatime)
	{	
		if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsFollowCat(id) || GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCatCaged(id)) { return; } // if cat is following cat or cage cat in the chain

		CircleCollider const& r_catCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);
		vec2 const& r_cursorPosition = CatHelperFunctions::GetCursorPositionInWorld();

		if (p_data->planningAttack)
		{
			p_catAttack->ToggleTelegraphs(true, false);
			p_catAttack->Update(id, deltatime);
		}
		else
		{
			if (m_timer >= 1.f) // resets double click when 1 second has passed
			{
				m_doubleClick = 0;
				m_timer = 0.f;
			}

			if (m_doubleClick >= 2)
			{
				p_data->planningAttack = true;
				m_doubleClick = 0;
				p_catAttack->ForceZeroMouse();
			}
			
			if (PointCollision(r_catCollider, r_cursorPosition))
			{
				if (r_cursorPosition != m_prevCursorPosition && m_mouseClicked && !p_data->attackSelected) // if current and prev cursor position are not the same, update movement
				{
					m_moving = true;
				}
			}
		}

		if ((m_moving && !p_data->planningAttack) || p_catMovement->CheckInvalid())
			p_catMovement->Update(id, deltatime);
		

		m_timer += deltatime;
		m_prevCursorPosition = r_cursorPosition;
		m_mouseClickPrevious = m_mouseClicked;
		m_collidedPreviously = PointCollision(r_catCollider, r_cursorPosition);
	}

	void Cat_v2_0PLAN::StateCleanUp()
	{
		p_catMovement->CleanUp();
		p_catAttack->CleanUp();

		REMOVE_MOUSE_EVENT_LISTENER(m_mouseClickEventListener);
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseReleaseEventListener);
	}

	void Cat_v2_0PLAN::StateExit(EntityID id)
	{
		if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsFollowCat(id) || GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCatCaged(id)) { return; } // if cat is following cat or cage cat in the chain

		p_catMovement->Exit(id);
		p_catAttack->Exit(id);

		EntityManager::GetInstance().Get<Collider>(p_data->catID).isTrigger = false;
	}

	void Cat_v2_0PLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		if (!p_data->planningAttack && !GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetSelectedCat(p_data->catID)) { return; }
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		if (MBPE.button == 1 && p_data->attackSelected)
		{
			if (!p_data->planningAttack) // if not currently planning attack, 
				p_catAttack->ToggleTelegraphs(false, false);
			p_catAttack->ResetSelection(p_data->catID);
			// Play undo audio
			PE::GlobalMusicManager::GetInstance().PlaySFX(std::string{ "AudioObject/UI Scribble SFX2.prefab" }, false);
			m_doubleClick = 0;
		}
		else if (MBPE.button == 1)
		{
			p_catMovement->ResetDrawnPath();
			p_catAttack->ToggleTelegraphs(false, false);
			p_data->planningAttack = false;
			// Play undo audio
			PE::GlobalMusicManager::GetInstance().PlaySFX(std::string{ "AudioObject/UI Scribble SFX2.prefab" }, false);
			m_doubleClick = 0;
		}
		else
			m_mouseClicked = true;
	}
	
	void Cat_v2_0PLAN::OnMouseRelease(const Event<MouseEvents>& r_ME)
	{
		MouseButtonReleaseEvent MBRE = dynamic_cast<const MouseButtonReleaseEvent&>(r_ME);
		if (m_mouseClicked)
		{
			if (m_collidedPreviously && PointCollision(std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(p_data->catID).colliderVariant), CatHelperFunctions::GetCursorPositionInWorld()))
				++m_doubleClick;
			m_mouseClicked = false;
		}
		m_moving = false;
	}

	// add mouse released
}
