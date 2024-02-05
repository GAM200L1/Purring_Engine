/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     PlayerControllerScript.cpp
 \date     03-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan\@digipen.edu
 \par      code %:     95%
 \par      changes:    Majority of the code

 \co-author            FOONG Jun Wei
 \par      email:      f.junwei\@digipen.edu
 \par      code %:     5%
 \par      changes:    rttr::instance GetScriptData()


 \brief  This file contains the definitions of the player controller

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "PlayerControllerScript.h"
#include "Input/InputSystem.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "Events/EventHandler.h"
#include "WindowManager.h"
#include "Graphics/CameraManager.h"

#include "Layers/LayerManager.h"
# define M_PI           3.14159265358979323846 

namespace PE 
{

	void PlayerControllerScript::Init(EntityID)
	{
		m_mouseClicked = false;
		ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, PlayerControllerScript::OnMouseClick, this)
	}
	void PlayerControllerScript::Update(EntityID id, float deltaTime)
	{
		if (m_mouseClicked)
			MoveTowardsClicked(id, deltaTime);

		switch (m_ScriptData[id].currentPlayerState)
		{
		case PlayerState::IDLE:
			
			MovePlayer(id,deltaTime);
			break;
		case PlayerState::MOVING:

			MovePlayer(id,deltaTime);
			break;
		case PlayerState::DEAD:
			break;
		}
	}
	void PlayerControllerScript::Destroy(EntityID id)
	{
		id;
	}

	void PlayerControllerScript::OnAttach(EntityID id)
	{
		if (!EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<RigidBody>()))
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<RigidBody>() });

		EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);

		m_ScriptData[id] = PlayerControllerScriptData();

	}

	void PlayerControllerScript::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	void PlayerControllerScript::MovePlayer(EntityID id, float deltaTime)
	{
		bool hasMoved = false;
		//movement without force
		if (InputSystem::IsKeyHeld(GLFW_KEY_W))
		{
			hasMoved = true;
			EntityManager::GetInstance().Get<Transform>(id).position += vec2{ 0.f,1.f } * m_ScriptData[id].speed * deltaTime;
			m_ScriptData[id].currentPlayerState = PlayerState::MOVING;
			m_mouseClicked = false;
		}
		if (InputSystem::IsKeyHeld(GLFW_KEY_A))
		{
			hasMoved = true;
			EntityManager::GetInstance().Get<Transform>(id).position += vec2{ -1.f,0.f } *m_ScriptData[id].speed * deltaTime;
			m_ScriptData[id].currentPlayerState = PlayerState::MOVING;
			m_mouseClicked = false;
		}
		if (InputSystem::IsKeyHeld(GLFW_KEY_S))
		{
			hasMoved = true;
			EntityManager::GetInstance().Get<Transform>(id).position += vec2{ 0.f,-1.f } *m_ScriptData[id].speed * deltaTime;
			m_ScriptData[id].currentPlayerState = PlayerState::MOVING;
			m_mouseClicked = false;
		}
		if (InputSystem::IsKeyHeld(GLFW_KEY_D))
		{
			hasMoved = true;
			EntityManager::GetInstance().Get<Transform>(id).position += vec2{ 1.f,0.f } *m_ScriptData[id].speed * deltaTime;
			m_ScriptData[id].currentPlayerState = PlayerState::MOVING;
			m_mouseClicked = false;
		}
		if(!hasMoved)
			m_ScriptData[id].currentPlayerState = PlayerState::IDLE;
	}

	void PlayerControllerScript::CheckState(EntityID id)
	{
		if (m_ScriptData[id].HP > 0)
		{
			if (EntityManager::GetInstance().Get<RigidBody>(id).velocity.x != 0 || EntityManager::GetInstance().Get<RigidBody>(id).velocity.y != 0)
			{
				m_ScriptData[id].currentPlayerState = PlayerState::MOVING;
			}
			else
			{
				m_ScriptData[id].currentPlayerState = PlayerState::IDLE;
			}
		}
		else
		{
			m_ScriptData[id].currentPlayerState = PlayerState::DEAD;
		}
	}

	std::map<EntityID, PlayerControllerScriptData>& PlayerControllerScript::GetScriptData()
	{
		return m_ScriptData;
	}


	rttr::instance PlayerControllerScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_ScriptData.at(id));
	}

	PlayerControllerScript::~PlayerControllerScript()
	{
	}

	void PlayerControllerScript::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);

		float xOffset{}, yOffset{};

		for (const auto& layer : LayerView<Graphics::Camera>())
		{
			for (EntityID objectID : InternalView(layer))
			{
				if (EntityManager::GetInstance().Get<Graphics::Camera>(objectID).GetIsMainCamera())
				{
					xOffset = EntityManager::GetInstance().Get<Transform>(objectID).position.x;
					yOffset = EntityManager::GetInstance().Get<Transform>(objectID).position.y;
				}
			}
		}
		m_currentMousePos = GETCAMERAMANAGER()->GetWindowToWorldPosition(static_cast<float>(MBPE.transX), static_cast<float>(MBPE.transY));

		m_currentMousePos.x += xOffset;
		m_currentMousePos.y += yOffset;

		m_mouseClicked = true;
	}

	void PlayerControllerScript::MoveTowardsClicked(EntityID id, float deltaTime)
	{
		Transform& currentObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);

		vec2 Direction = vec2(m_currentMousePos.x - currentObject.position.x, m_currentMousePos.y - currentObject.position.y);

		vec2 normalizedDirection = Direction.GetNormalized();


		//translate to player but this is not what we want
		if ((currentObject.position.x > m_currentMousePos.x + 1.5f || currentObject.position.x < m_currentMousePos.x - 1.5f) && (currentObject.position.y > m_currentMousePos.y + 1.5f || currentObject.position.y < m_currentMousePos.y - 1.5f))
		{
			currentObject.position += normalizedDirection * m_ScriptData[id].speed * deltaTime;
			m_ScriptData[id].currentPlayerState = PlayerState::MOVING;
		}
		else
		{
			m_mouseClicked = false;
			m_ScriptData[id].currentPlayerState = PlayerState::IDLE;
		}


	}
}