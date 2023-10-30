#include "prpch.h"
#include "PlayerControllerScript.h"
#include "Input/InputSystem.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
# define M_PI           3.14159265358979323846 

namespace PE 
{

	void PlayerControllerScript::Init(EntityID id)
	{
		id;
	}
	void PlayerControllerScript::Update(EntityID id, float deltaTime)
	{
		deltaTime;
		// Movement
		CheckState(id);
		switch (m_ScriptData[id].currentPlayerState)
		{
		case PlayerState::IDLE:
			MovePlayer(id);

			if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<AnimationComponent>()))
			{
				EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationIndex("playerAttack");
			}			
			break;
		case PlayerState::MOVING:
			MovePlayer(id);

			if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<AnimationComponent>()))
			{
				EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationIndex("playerWalk");
			}
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

		m_ScriptData[id] = PlayerControllerScriptData();

	}

	void PlayerControllerScript::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	void PlayerControllerScript::MovePlayer(EntityID id)
	{
		if (InputSystem::IsKeyHeld(GLFW_KEY_W))
		{
			EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ 0.f,1.f } *m_ScriptData[id].speed);
		}
		if (InputSystem::IsKeyHeld(GLFW_KEY_A))
		{
			EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ -1.f,0.f } *m_ScriptData[id].speed);
		}
		if (InputSystem::IsKeyHeld(GLFW_KEY_S))
		{
			EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ 0.f,-1.f } *m_ScriptData[id].speed);
		}
		if (InputSystem::IsKeyHeld(GLFW_KEY_D))
		{
			EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ 1.f,0.f } *m_ScriptData[id].speed);
		}
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

	PlayerControllerScript::~PlayerControllerScript()
	{
	}
}