/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     OrangeCatAttackStates_v2_0.cpp
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the definitions for the functions for the Orange Cat Attack class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"

#include "Hierarchy/HierarchyManager.h"
#include "Physics/CollisionManager.h"

#include "OrangeCatAttackStates_v2_0.h"
#include "CatController_v2_0.h"
#include "CatScript_v2_0.h"
#include "CatHelperFunctions.h"

namespace PE
{
	// ----- Create Orange Cat Stomp and Telegraphs ----- //
	void OrangeCatAttackVariables::CreateSeismicAndTelegraph(EntityID catID)
	{
		// create seismic //
		SerializationManager serializationManager;
		seismicID = serializationManager.LoadFromFile("Seismic_Prefab.json");
		
		Hierarchy::GetInstance().GetInstance().AttachChild(catID, seismicID);
		
		CatHelperFunctions::ToggleEntity(seismicID, false);
		
		// create telegraph //
		Transform const& catTransform = EntityManager::GetInstance().Get<Transform>(catID);

		telegraphID = serializationManager.LoadFromFile("OrangeCatAttackTelegraph_Prefab.json");
		Transform& telegraphTransform = EntityManager::GetInstance().Get<Transform>(telegraphID);

		Hierarchy::GetInstance().AttachChild(catID, telegraphID);
		telegraphTransform.relPosition.Zero();

		EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).isActive = false; // telegraph to not show until attack planning
		EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).toSave = false; // telegraph to not show until attack planning

		telegraphTransform.height = catTransform.height * seismicRadius;
		telegraphTransform.width = catTransform.width * seismicRadius;

		EntityManager::GetInstance().Get<Collider>(telegraphID).colliderVariant = CircleCollider();
		EntityManager::GetInstance().Get<Collider>(telegraphID).isTrigger = true;
	}

	// ----- ATTACK PLAN ----- //

	void OrangeCatAttack_v2_0PLAN::Enter(EntityID id)
	{
		// retrieve pointer to game controller
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

		// retrieves the data for the grey cat's attack
		p_attackData = &std::get<OrangeCatAttackVariables>((GETSCRIPTDATA(CatScript_v2_0, id))->attackVariables);

		// subscribe to mouse click event for selecting attack telegraphs
		m_mouseClickEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, OrangeCatAttack_v2_0PLAN::OnMouseClick, this);
		m_mouseReleaseEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonReleased, OrangeCatAttack_v2_0PLAN::OnMouseRelease, this);
	}

	void OrangeCatAttack_v2_0PLAN::Update(EntityID id, float deltaTime)
	{
		if (p_gsc->currentState == GameStates_v2_0::PAUSE) { return; }

CircleCollider const& r_telegraphCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(p_attackData->telegraphID).colliderVariant);

vec2 cursorPosition{ CatHelperFunctions::GetCursorPositionInWorld() };

bool collidingWithTelegraph = PointCollision(r_telegraphCollider, cursorPosition);

if (collidingWithTelegraph)
{
	CatHelperFunctions::SetColor(p_attackData->telegraphID, m_hoverColor);
	if (m_mouseClick)
	{
		(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected = true;
		CatHelperFunctions::SetColor(p_attackData->telegraphID, m_selectColor);
	}
}
else
{
	if (!(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected)
		CatHelperFunctions::SetColor(p_attackData->telegraphID, m_defaultColor);
}

if (m_mouseClick && !m_mouseClickedPrevious && !collidingWithTelegraph)
{
	(GETSCRIPTDATA(CatScript_v2_0, id))->planningAttack = false;

	if (!(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected)
		CatHelperFunctions::ToggleEntity(p_attackData->telegraphID, false);
}

m_mouseClickedPrevious = m_mouseClick;
	}

	void OrangeCatAttack_v2_0PLAN::CleanUp()
	{
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseClickEventListener);
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseReleaseEventListener);
	}

	void OrangeCatAttack_v2_0PLAN::Exit(EntityID id)
	{
		CatHelperFunctions::ToggleEntity(p_attackData->telegraphID, false);
	}

	void OrangeCatAttack_v2_0PLAN::ResetSelection(EntityID id)
	{
		CatHelperFunctions::SetColor(p_attackData->telegraphID, m_defaultColor);
		(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected = false;
	}

	void OrangeCatAttack_v2_0PLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		if (MBPE.button != 1)
			m_mouseClick = true;
	}

	void OrangeCatAttack_v2_0PLAN::OnMouseRelease(const Event<MouseEvents>& r_ME)
	{
		MouseButtonReleaseEvent MBRE = dynamic_cast<const MouseButtonReleaseEvent&>(r_ME);
		m_mouseClick = false;
	}

	void OrangeCatAttack_v2_0PLAN::ToggleTelegraphs(bool setToggle, bool ignoreSelected)
	{
		CatHelperFunctions::ToggleEntity(p_attackData->telegraphID, false);
	}

	// ----- ATTACK EXECUTE ----- //

	void OrangeCatAttack_v2_0EXECUTE::StateEnter(EntityID id)
	{
		// stores ID of the cat
		m_catID = id;

		p_catData = GETSCRIPTDATA(CatScript_v2_0, id);

		// retrieves the data for the grey cat's attack
		p_attackData = &std::get<OrangeCatAttackVariables>((GETSCRIPTDATA(CatScript_v2_0, id))->attackVariables);

		// subscribes to collision events
		m_collisionEnterEventListener = ADD_COLLISION_EVENT_LISTENER(PE::CollisionEvents::OnCollisionEnter, OrangeCatAttack_v2_0EXECUTE::SeismicCollided, this);

		m_seismicDelay = p_attackData->seismicDelay;
		m_seismicLifeTime = p_attackData->seismicLifeTime;
	}

	void OrangeCatAttack_v2_0EXECUTE::StateUpdate(EntityID id, float deltaTime)
	{
		GameStateController_v2_0* p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		if (p_gsc->currentState == GameStates_v2_0::PAUSE) { return; }

		if (m_seismicSlammed)
		{
			// @TODO check if following animation or a lifetime
			if (m_seismicLifeTime <= 0.f)
			{
				p_catData->finishedExecution = true;
				CatHelperFunctions::ToggleEntity(p_attackData->seismicID, false);
			}
		}
		else if (!p_catData->finishedExecution && p_catData->attackSelected && EntityManager::GetInstance().Get<AnimationComponent>(id).GetCurrentFrameIndex() == p_attackData->seismicSlamAnimationIndex)
		{
			if (m_seismicDelay <= 0.f)
			{
				// @TODO check if this is following animation too if it is do the animation index check
				CatHelperFunctions::ToggleEntity(p_attackData->seismicID, true);
				m_seismicSlammed = true;

				// @TODO add sound
			}
			else {
				m_seismicDelay -= deltaTime;
			}
		}
	}

	void OrangeCatAttack_v2_0EXECUTE::StateCleanUp()
	{
		REMOVE_KEY_COLLISION_LISTENER(m_collisionEnterEventListener);
	}

	void OrangeCatAttack_v2_0EXECUTE::StateExit(EntityID id)
	{
		p_catData->attackSelected = false;
		p_catData->finishedExecution = false;
		m_seismicSlammed = false;
		CatHelperFunctions::ToggleEntity(p_attackData->seismicID, false);
	}

	void OrangeCatAttack_v2_0EXECUTE::SeismicCollided(const Event<CollisionEvents> r_CE)
	{
		auto IsCatAndNotCaged =
		[&](EntityID id)
		{
			if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCat(id))
			{
				if (!GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCatCaged(id))
					return true;
			}
			return false;
		};
	}

	void OrangeCatAttack_v2_0EXECUTE::SeismicHitCat(const Event<CollisionEvents>& r_CE)
	{

	}

	void OrangeCatAttack_v2_0EXECUTE::SeismicHitRat(const Event<CollisionEvents>& r_CE)
	{

	}
}