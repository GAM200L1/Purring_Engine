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
		if (GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->currentState == GameStates_v2_0::PAUSE)
		{
			p_catMovement->EndPathDrawing(id);
			return;
		}
		CircleCollider const& r_catCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);
		vec2 cursorPosition = CatHelperFunctions::GetCursorPositionInWorld();
		//std::cout << "Cursor Pos: " << cursorPosition.x << ", " << cursorPosition.y << '\n';
		bool const collidedCurrent = PointCollision(r_catCollider, cursorPosition);

		// check for double click
		if (!p_data->planningAttack)
		{
			// if in previous frame and current frame the mouse has always been there allow double click
			// if previous frame not clicked and this frame clicked increment double click
			if (p_data->doubleClickTimer >= 0.35f) // resets double click when 1 second has passed
			{
				p_data->startDoubleClickTimer = false;
				p_data->doubleClickTimer = 0.f;
			}
			else
			{
				// if mouse triggered
				if (m_mouseClicked && !m_mouseClickPrevious)
				{	
					// second time triggered
					if (p_data->startDoubleClickTimer && collidedCurrent && m_collidedPreviously && GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetSelectedCat(id))
					{
						p_data->planningAttack = true;
						p_catAttack->ToggleTelegraphs(true, false);
						p_data->startDoubleClickTimer = false;
						p_data->doubleClickTimer = 0.f;
					}
					else if (collidedCurrent) // first time clicking
					{ p_data->startDoubleClickTimer = true; }
				}

				// if mouse triggered once
				if (p_data->startDoubleClickTimer)
					p_data->doubleClickTimer += deltatime;
			}
		}
		else // if planning attack
		{
			p_catAttack->Update(id, deltatime, cursorPosition, m_mouseClicked, m_mouseClickPrevious);
		}
		
		if (!p_data->planningAttack && !p_data->attackSelected && GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetSelectedCat(id))
		{
			p_catMovement->Update(id, deltatime, cursorPosition, m_mousePositionPrevious, m_mouseClicked, m_mouseClickPrevious);
		}
		if (p_data->catType == EnumCatType::MAINCAT)
			p_catMovement->StopHeartAnimation(id);
		
		// save previous frame data
		m_mouseClickPrevious = m_mouseClicked;
		m_collidedPreviously = collidedCurrent;
		m_mousePositionPrevious = cursorPosition;
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
		
		p_data->startDoubleClickTimer = false;
		p_data->doubleClickTimer = 0.f;
		
		p_catMovement->Exit(id);
		p_catAttack->Exit(id);

		if (!GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->godMode)
			EntityManager::GetInstance().Get<Collider>(p_data->catID).isTrigger = false;
	}

	void Cat_v2_0PLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		if (r_ME.GetType() == MouseEvents::MouseButtonPressed)
		{
			MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
			if (MBPE.button == 0)
				m_mouseClicked = true;
		}
	}
	
	void Cat_v2_0PLAN::OnMouseRelease(const Event<MouseEvents>& r_ME)
	{
		if (r_ME.GetType() == MouseEvents::MouseButtonReleased)
		{
			MouseButtonReleaseEvent MBRE = dynamic_cast<const MouseButtonReleaseEvent&>(r_ME);
			//if (m_collidedPreviously && PointCollision(std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(p_data->catID).colliderVariant), CatHelperFunctions::GetCursorPositionInWorld()))
			if (MBRE.button == 0)
				m_mouseClicked = false;
		}
	}

	void Cat_v2_0PLAN::ResetMovement(EntityID id)
	{
		if (p_data->planningAttack)
		{
			p_catAttack->ToggleTelegraphs(false, false);
			p_data->planningAttack = false;
		}
		p_catMovement->ResetDrawnPath();
		p_catMovement->StopHeartAnimation(id);
		PE::GlobalMusicManager::GetInstance().PlaySFX(std::string{ "AudioObject/UI Scribble SFX2.prefab" }, false);
	}

	void Cat_v2_0PLAN::ResetAttack(EntityID id)
	{
		if (!p_data->planningAttack) // if not currently planning attack, 
			p_catAttack->ToggleTelegraphs(false, false);
		p_catAttack->ResetSelection(p_data->catID);
		PE::GlobalMusicManager::GetInstance().PlaySFX(std::string{ "AudioObject/UI Scribble SFX2.prefab" }, false);
	}
	// add mouse released
}
