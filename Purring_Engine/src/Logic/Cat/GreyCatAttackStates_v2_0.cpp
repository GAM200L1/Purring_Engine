/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GreyCatAttackStates_v2_0.cpp
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Attack Base class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"

#include "GreyCatAttackStates_v2_0.h"
#include "CatScript_v2_0.h"
#include "CatHelperFunctions.h"

#include "Hierarchy/HierarchyManager.h"
#include "Physics/CollisionManager.h"

namespace PE
{

	// ----- ATTACK PLAN ----- //

	void GreyCatAttack_v2_0PLAN::Enter(EntityID id)
	{
		// retrieve pointer to the game  controller
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

		// retrieves the data for the grey cat's attack
		p_attackData = &std::get<GreyCatAttackVariables>((GETSCRIPTDATA(CatScript_v2_0, id))->attackVariables);
		
		// set to cat id to be safe
		p_attackData->attackDirection = std::pair(EnumCatAttackDirection_v2_0::NONE, id);
		
		// subscribe to mouse click event for selecting attack telegraphs
		m_mouseClickEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, GreyCatAttack_v2_0PLAN::OnMouseClick, this);
		m_mouseReleaseEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonReleased, GreyCatAttack_v2_0PLAN::OnMouseRelease, this);
	}

	void GreyCatAttack_v2_0PLAN::Update(EntityID id, float deltaTime)
	{
		if (p_gsc->currentState == GameStates_v2_0::PAUSE) { return; }
		
		vec2 cursorPosition{ CatHelperFunctions::GetCursorPositionInWorld() };

		try
		{
			
			//if (!m_showTelegraphs) // if telegraphs not showing, enable when the cat is selected
			//{
			//	if (m_mouseClick && PointCollision(r_catCollider, cursorPosition))
			//	{
			//		m_showTelegraphs = true;
			//		// shows all telegraphs
					ToggleAll(true, false);
				//}
			//}
			//else
			{
				for (auto const& r_telegraph : p_attackData->telegraphIDs) // for every telegraph
				{
					AABBCollider const& r_telegraphCollider = std::get<AABBCollider>(EntityManager::GetInstance().Get<Collider>(r_telegraph.second).colliderVariant);

					bool collidedWithTelegraph = PointCollision(r_telegraphCollider, cursorPosition);
					if (r_telegraph == p_attackData->attackDirection) { continue; }
					// check if the mouse is hovering any of the boxes, if yes, boxes should change color
					if (collidedWithTelegraph)
					{
						CatHelperFunctions::SetColor(r_telegraph.second, m_hoverColor);
						if (m_mouseClick) // selects an attack direction
						{
							// @TODO: Add select direction sfx
							p_attackData->attackDirection = r_telegraph;
							(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected = true;
							CatHelperFunctions::SetColor(r_telegraph.second, m_selectColor);
							break;
						}
					}
					else // if not hovering any telegraphs, set to default color
					{
						CatHelperFunctions::SetColor(r_telegraph.second, m_defaultColor);
						
						// disables telegraphs if anywhere but the telegraphs are clicked
						if (m_mouseClick) 
						{
							(GETSCRIPTDATA(CatScript_v2_0, id))->planningAttack = false;
							ToggleAll(false, true);
						}
					}
				}
			}
			std::cout << m_rightMouseClick << ' ';
			// if right click and attack has been selected, deselect the attack
			if (m_rightMouseClick && (GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected)
			{
				ResetSelection(id);
			}
		}
		catch (...) {} // colliders are not correct

		m_mouseClickedPrevious = m_mouseClick;
	}

	void GreyCatAttack_v2_0PLAN::CleanUp()
	{
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseClickEventListener);
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseReleaseEventListener);
	}

	void GreyCatAttack_v2_0PLAN::Exit(EntityID id)
	{
		// toggles all telegraphs except the selected one to false
		ToggleAll(false, false);
	}

	void GreyCatAttack_v2_0PLAN::ResetSelection(EntityID id)
	{
		m_showTelegraphs = false;
		p_attackData->attackDirection = std::pair(EnumCatAttackDirection_v2_0::NONE, id);
		(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected = false;
	}

	void GreyCatAttack_v2_0PLAN::CreateProjectileTelegraphs(EntityID id, float bulletRange, std::map<EnumCatAttackDirection_v2_0, EntityID>& r_telegraphIDs)
	{
		auto CreateOneTelegraph = 
			[&](bool isXAxis, bool isNegative)
			{
				Transform const& catTransform = EntityManager::GetInstance().Get<Transform>(id);

				SerializationManager serializationManager;

				EntityID telegraphID = serializationManager.LoadFromFile("PlayerAttackTelegraph_Prefab.json");
				Transform& telegraphTransform = EntityManager::GetInstance().Get<Transform>(telegraphID);

				//EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).parent = id; // telegraph follows the cat entity
				Hierarchy::GetInstance().AttachChild(id, telegraphID); // new way of attatching parent child
				telegraphTransform.relPosition.Zero();
				EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).isActive = false; // telegraph to not show until attack planning
				EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).toSave = false; // telegraph to not show until attack planning


				// set size of telegraph
				telegraphTransform.height = catTransform.height * 0.75f;
				telegraphTransform.width = catTransform.width * bulletRange;
				EnumCatAttackDirection_v2_0 dir;
				AABBCollider telegraphCollider;

				// Set the dimensions of the telegraph based on the axis it's on
				if (isXAxis)
				{
					telegraphTransform.relPosition.x = ((isNegative) ? -1.f : 1.f) * ((telegraphTransform.width * 0.5f) + (catTransform.width * 0.5f) + 10.f);
					dir = (isNegative) ? EnumCatAttackDirection_v2_0::WEST : EnumCatAttackDirection_v2_0::EAST;
				}
				else
				{
					telegraphTransform.relOrientation = PE_PI * 0.5f;
					telegraphTransform.relPosition.y = ((isNegative) ? -1.f : 1.f) * ((telegraphTransform.width * 0.5f) + (catTransform.width * 0.5f) + 10.f);

					telegraphCollider.scaleOffset.x = telegraphTransform.height / telegraphTransform.width;
					telegraphCollider.scaleOffset.y = telegraphTransform.width / telegraphTransform.height;

					dir = (isNegative) ? EnumCatAttackDirection_v2_0::SOUTH : EnumCatAttackDirection_v2_0::NORTH;
				}

				// Configure the collider
				EntityManager::GetInstance().Get<Collider>(telegraphID).colliderVariant = telegraphCollider;
				EntityManager::GetInstance().Get<Collider>(telegraphID).isTrigger = true;

				r_telegraphIDs.emplace(dir, telegraphID);
			};

		CreateOneTelegraph(true, false); // east
		CreateOneTelegraph(true, true); // west
		CreateOneTelegraph(false, false); // north
		CreateOneTelegraph(false, true); // south
	}

	void GreyCatAttack_v2_0PLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		if (MBPE.button == 1)
			m_rightMouseClick = true;
		else
			m_mouseClick = true;
	}

	void GreyCatAttack_v2_0PLAN::OnMouseRelease(const Event<MouseEvents>& r_ME)
	{
		MouseButtonReleaseEvent MBRE = dynamic_cast<const MouseButtonReleaseEvent&>(r_ME);
		if (MBRE.button == 1)
			m_rightMouseClick = false;
		else
			m_mouseClick = false;
	}

	void GreyCatAttack_v2_0PLAN::ToggleAll(bool setToggle, bool ignoreSelected)
	{
		for (auto const& r_telegraph : p_attackData->telegraphIDs)
		{
			if (r_telegraph == p_attackData->attackDirection && ignoreSelected) { continue; } // don't disable attack that has been selected
			CatHelperFunctions::ToggleEntity(r_telegraph.second, setToggle);
		}
	}

	// ----- ATTACK EXECUTE ----- //

	void GreyCatAttack_v2_0EXECUTE::StateEnter(EntityID id)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::StateUpdate(EntityID id, float deltaTime)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::StateCleanUp()
	{

	}

	void GreyCatAttack_v2_0EXECUTE::StateExit(EntityID id)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::ProjectileHitCat(const Event<CollisionEvents>& r_CE)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::ProjectileHitRat(const Event<CollisionEvents>& r_CE)
	{

	}
}