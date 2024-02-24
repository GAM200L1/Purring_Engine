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

		telegraphTransform.height = catTransform.height * stompRadius;
		telegraphTransform.width = catTransform.width * stompRadius;

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

	}

	void OrangeCatAttack_v2_0EXECUTE::StateUpdate(EntityID id, float deltaTime)
	{

	}

	void OrangeCatAttack_v2_0EXECUTE::StateCleanUp()
	{

	}

	void OrangeCatAttack_v2_0EXECUTE::StateExit(EntityID id)
	{

	}

	void OrangeCatAttack_v2_0EXECUTE::SlamHitCat(const Event<CollisionEvents>& r_CE)
	{

	}

	void OrangeCatAttack_v2_0EXECUTE::SlamHitRat(const Event<CollisionEvents>& r_CE)
	{

	}
}