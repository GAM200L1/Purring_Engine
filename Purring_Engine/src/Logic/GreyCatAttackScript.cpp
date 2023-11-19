/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GreyCatAttackScript.cpp
 \date     15-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains definitions for functions used for a grey cat's attack state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "GreyCatAttackScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/SceneView.h"
#include "Logic/LogicSystem.h"
#include "Physics/CollisionManager.h"

namespace PE
{
	// ----- GREY CAT ATTACK SCRIPT ----- //
	void GreyCatAttackScript::Init(EntityID id)
	{
		m_scriptData[id].m_stateManager = new StateMachine{};
		m_scriptData[id].m_stateManager->ChangeState(new GreyCatAttackPLAN{}, id);
	}
	
	void GreyCatAttackScript::Update(EntityID id, float deltaTime)
	{
		m_scriptData[id].m_stateManager->Update(id, deltaTime);
	}
	
	void GreyCatAttackScript::Destroy(EntityID id)
	{}
	
	void GreyCatAttackScript::OnAttach(EntityID id)
	{
		// check if the given entity has transform, rigidbody, and collider. if it does not, assign it one
		if (!EntityManager::GetInstance().Has<Transform>(id))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<Transform>() });
		}
		if (!EntityManager::GetInstance().Has<RigidBody>(id))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<RigidBody>() });
			EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
		}
		if (!EntityManager::GetInstance().Has<Collider>(id))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<Collider>() });
			EntityManager::GetInstance().Get<Collider>(id).colliderVariant = CircleCollider(); // cat default colliders is circle
		}
		
		if (m_scriptData.find(id) == m_scriptData.end())
		{
			m_scriptData[id] = GreyCatAttackScriptData{};
		}
		else
		{
			delete m_scriptData[id].m_stateManager;
			m_scriptData[id] = GreyCatAttackScriptData{};
		}

		CreateAttackSelectBoxes(id, true, false); // east box
		CreateAttackSelectBoxes(id, true, true); // west box
		CreateAttackSelectBoxes(id, false, false); // north box
		CreateAttackSelectBoxes(id, false, true); // south box
	}
	
	void GreyCatAttackScript::OnDetach(EntityID id)
	{
		if (m_scriptData.find(id) != m_scriptData.end())
		{
			delete m_scriptData[id].m_stateManager;
			m_scriptData.erase(id);
		}
	}

	std::map<EntityID, GreyCatAttackScriptData>& GreyCatAttackScript::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance GreyCatAttackScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

	GreyCatAttackScript::~GreyCatAttackScript()
	{
		for (auto& [key, value] : m_scriptData)
		{
			delete value.m_stateManager;
		}
	}

	void GreyCatAttackScript::CreateAttackSelectBoxes(EntityID id, bool isXAxis, bool isNegative)
	{
		// create the east direction entity
		EntityID boxID = EntityFactory::GetInstance().CreateEntity<Transform, Collider, Graphics::Renderer>();
		EntityManager::GetInstance().Get<Collider>(boxID).colliderVariant = AABBCollider();

		EntityManager::GetInstance().Get<Graphics::Renderer>(boxID).SetColor(0.f, 1.f, 0.f, 1.f); // sets the color of the box to be green
		EntityManager::GetInstance().Get<EntityDescriptor>(boxID).parent = id;

		vec2 boxPositionOffset{ 0.f,0.f };
		vec2 boxScaleOffset{ 1.f,1.f };
		EnumGreyCatAttackDirection dir;
		
		if (isXAxis)
		{
			boxScaleOffset.x = m_scriptData[id].bulletRange * EntityManager::GetInstance().Get<Transform>(id).width;
			boxPositionOffset.x = (m_scriptData[id].bulletRange * 0.5f);
			boxPositionOffset.x *= (isNegative) ? -1 : 1;
			dir = (isNegative) ? EnumGreyCatAttackDirection::WEST : EnumGreyCatAttackDirection::EAST;
		}
		else
		{
			boxScaleOffset.y = m_scriptData[id].bulletRange * EntityManager::GetInstance().Get<Transform>(id).height;
			boxPositionOffset.y = (m_scriptData[id].bulletRange * 0.5f);
			boxPositionOffset.y *= (isNegative) ? -1 : 1;
			dir = (isNegative) ? EnumGreyCatAttackDirection::SOUTH : EnumGreyCatAttackDirection::NORTH;
		}

		// set the position of the selection box to be half the range away from the center of the cat
		EntityManager::GetInstance().Get<Transform>(boxID).position.x = EntityManager::GetInstance().Get<Transform>(id).position.x + boxPositionOffset.x;
		EntityManager::GetInstance().Get<Transform>(boxID).position.y = EntityManager::GetInstance().Get<Transform>(id).position.y + boxPositionOffset.y;

		// set the scale of the selection box
		EntityManager::GetInstance().Get<Transform>(boxID).width = EntityManager::GetInstance().Get<Transform>(id).width * boxScaleOffset.x;
		EntityManager::GetInstance().Get<Transform>(boxID).height = EntityManager::GetInstance().Get<Transform>(id).height * boxScaleOffset.y;

		m_scriptData[id].selectBoxIDs.emplace(dir, boxID);
	}


	// ----- GREY CAT ATTACK PLAN STATE MANAGER ----- //
	void GreyCatAttackPLAN::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(GreyCatAttackScript, id);
		ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, GreyCatAttackPLAN::OnMouseClick, this);
	}
	void GreyCatAttackPLAN::StateUpdate(EntityID id, float deltaTime)
	{
		if (m_showBoxes)
		{
			for (auto const& selectBox : p_data->selectBoxIDs)
			{
				AABBCollider& selectBoxCollider = std::get<AABBCollider>(EntityManager::GetInstance().Get<Collider>(selectBox.second).colliderVariant);
				if (PointCollision(selectBoxCollider, vec2{ 0.f, 0.f }))
				{
					EntityManager::GetInstance().Get<Graphics::Renderer>(selectBox.second).SetColor(1.f, 0.f, 0.f, 1.f); // sets the color of the box to be red if hovered
					if (m_mouseClick)
					{
						p_data->attackDirection = selectBox.first;
						m_mouseClick = false;
						std::cout << "yippee\n";
						/*p_data->m_stateManager->ChangeState(new GreyCatAttackEXECUTE{}, id);
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
	
	void GreyCatAttackPLAN::StateExit(EntityID id)
	{}

	std::string_view GreyCatAttackPLAN::GetName()
	{
		return "AttackPLAN";
	}

	void GreyCatAttackPLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		m_mouseClick = true;
	}
}