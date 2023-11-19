/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatAttackScript.cpp
 \date     15-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains definitions for functions used for a grey cat's attack state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "CatScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/SceneView.h"
#include "Logic/LogicSystem.h"
#include "Physics/CollisionManager.h"

namespace PE
{
	// ----- Helper function for creating entities which are selectable boxes for the cat attacks ----- //
	void CatScript::CreateAttackSelectBoxes(EntityID id, bool isXAxis, bool isNegative)
	{
		// create the east direction entity
		EntityID boxID = EntityFactory::GetInstance().CreateEntity<Transform, Collider, Graphics::Renderer>();
		EntityManager::GetInstance().Get<Collider>(boxID).colliderVariant = AABBCollider();
		EntityManager::GetInstance().Get<Collider>(boxID).isTrigger = true;

		EntityManager::GetInstance().Get<Graphics::Renderer>(boxID).SetColor(0.f, 1.f, 0.f, 1.f); // sets the color of the box to be green
		EntityManager::GetInstance().Get<EntityDescriptor>(boxID).parent = id;
		EntityManager::GetInstance().Get<EntityDescriptor>(boxID).isActive = false;

		vec2 boxPositionOffset{ 0.f,0.f };
		vec2 boxScaleOffset{ 1.f,1.f };
		EnumCatAttackDirection dir;
		
		if (isXAxis)
		{
			boxScaleOffset.x = m_scriptData[id].bulletRange;
			boxPositionOffset.x = (EntityManager::GetInstance().Get<Transform>(id).width * 0.5f) + (m_scriptData[id].bulletRange * 0.5f * EntityManager::GetInstance().Get<Transform>(boxID).width);
			boxPositionOffset.x *= (isNegative) ? -1 : 1;
			dir = (isNegative) ? EnumCatAttackDirection::WEST : EnumCatAttackDirection::EAST;
		}
		else
		{
			boxScaleOffset.y = m_scriptData[id].bulletRange;
			boxPositionOffset.y = (EntityManager::GetInstance().Get<Transform>(id).height * 0.5f) + (m_scriptData[id].bulletRange * 0.5f * EntityManager::GetInstance().Get<Transform>(boxID).height);
			boxPositionOffset.y *= (isNegative) ? -1 : 1;
			dir = (isNegative) ? EnumCatAttackDirection::SOUTH : EnumCatAttackDirection::NORTH;
		}

		// set the position of the selection box to be half the range away from the center of the cat
		EntityManager::GetInstance().Get<Transform>(boxID).relPosition.x = EntityManager::GetInstance().Get<Transform>(id).position.x + boxPositionOffset.x;
		EntityManager::GetInstance().Get<Transform>(boxID).relPosition.y = EntityManager::GetInstance().Get<Transform>(id).position.y + boxPositionOffset.y;

		// set the scale of the selection box
		EntityManager::GetInstance().Get<Transform>(boxID).width = EntityManager::GetInstance().Get<Transform>(id).width * boxScaleOffset.x;
		EntityManager::GetInstance().Get<Transform>(boxID).height = EntityManager::GetInstance().Get<Transform>(id).height * boxScaleOffset.y;

		m_scriptData[id].selectBoxIDs.emplace(dir, boxID);
	}


	// ----- CAT ATTACK PLAN STATE ----- //
	void CatAttackPLAN::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(CatScript, id);
		ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, CatAttackPLAN::OnMouseClick, this);
	}
	void CatAttackPLAN::StateUpdate(EntityID id, float deltaTime)
	{
		//double mouseX{}, mouseY{};
		//InputSystem::GetCursorViewportPosition(m_window, mouseX, mouseY); // I'll change this to take floats in next time...
		//vec2 cursorPosition{ m_cameraManager->GetWindowToWorldPosition(static_cast<float>(mouseX), static_cast<float>(mouseY)) };
		if (m_showBoxes)
		{
			for (auto const& selectBox : p_data->selectBoxIDs)
			{
				AABBCollider const& selectBoxCollider = std::get<AABBCollider>(EntityManager::GetInstance().Get<Collider>(selectBox.second).colliderVariant);
				if (PointCollision(selectBoxCollider, vec2{0.f, 0.f}))
				{
					EntityManager::GetInstance().Get<Graphics::Renderer>(selectBox.second).SetColor(1.f, 0.f, 0.f, 1.f); // sets the color of the box to be red if hovered
					if (m_mouseClick)
					{
						p_data->attackDirection = selectBox.first;
						m_mouseClick = false;
						std::cout << "yippee\n";
						/*p_data->m_stateManager->ChangeState(new CatAttackEXECUTE{}, id);
						return;*/
					}
				}
				else
				{
					EntityManager::GetInstance().Get<Graphics::Renderer>(selectBox.second).SetColor(0.f, 1.f, 0.f, 1.f); // sets the color of the box to be green if not hovering
				}
			}
		}
	}
	
	void CatAttackPLAN::StateExit(EntityID id)
	{}

	std::string_view CatAttackPLAN::GetName()
	{
		return "AttackPLAN";
	}

	void CatAttackPLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		m_mouseClick = true;
	}
}