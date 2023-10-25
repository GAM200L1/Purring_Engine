#include "prpch.h"
#include "EnemyTestScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
# define M_PI           3.14159265358979323846 
//#include "Input/InputSystem.h"

namespace PE {

	void EnemyTestScript::Init(EntityID id)
	{
		id;
	}

	void EnemyTestScript::Update(EntityID id, float deltaTime)
	{
		switch (m_ScriptData[id].EnemyCurrentState)
		{
		case EnemyState::IDLE:
		{
			PE::EntityManager::GetInstance().Get<PE::Transform>(id).orientation += static_cast<float>(180 * (M_PI / 180) * deltaTime * 10);
			m_ScriptData[id].idleTimer -= deltaTime;
			if (m_ScriptData[id].idleTimer <= 0)
			{
				m_ScriptData[id].patrolTimer = m_ScriptData[id].alertBuffer;
				ChangeEnemyState(id, EnemyState::PATROL);
				m_ScriptData[id].bounce = true;
			}
			break;
		}
		case EnemyState::PATROL:
		{
			if (m_ScriptData[id].bounce)
				EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ 0.f,.25f } *m_ScriptData[id].speed);
			else
				EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ 0.f,-.25f } *m_ScriptData[id].speed);

			m_ScriptData[id].patrolTimer -= deltaTime;


			if (m_ScriptData[id].patrolTimer < 2.f && m_ScriptData[id].patrolTimer > 0.f)
			{
				m_ScriptData[id].bounce = false;
			}
			else if (m_ScriptData[id].patrolTimer < 0)
			{
				m_ScriptData[id].idleTimer = m_ScriptData[id].alertBuffer;
				ChangeEnemyState(id, EnemyState::IDLE);
			}

			if (abs(GetDistanceFromPlayer(id)) <= 100)
			{
				m_ScriptData[id].alertTimer = m_ScriptData[id].alertBuffer;
				ChangeEnemyState(id, EnemyState::ALERT);
			}

			break;
		}
		case EnemyState::TARGET:
		{
			RotateToPlayer(id);
			break;
		}
		case EnemyState::ALERT:
		{
			m_ScriptData[id].alertTimer -= deltaTime;

			if (m_ScriptData[id].alertTimer <= 0 && abs(GetDistanceFromPlayer(id)) <= 100)
			{
				ChangeEnemyState(id, EnemyState::TARGET);
			}
			else if (m_ScriptData[id].alertTimer <= 0)
			{
				m_ScriptData[id].idleTimer = m_ScriptData[id].alertBuffer;
				ChangeEnemyState(id, EnemyState::IDLE);
			}


			break;
		}
		}
	}

	void EnemyTestScript::Destroy(EntityID id)
	{
		id;
	}

	void EnemyTestScript::OnAttach(EntityID id)
	{
		if (!EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<RigidBody>()))
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<RigidBody>() });

		m_ScriptData[id] = EnemyTestScriptData();
	}

	void EnemyTestScript::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	std::map<EntityID, EnemyTestScriptData>& EnemyTestScript::GetScriptData()
	{
		return m_ScriptData;
	}

	EnemyTestScript::~EnemyTestScript()
	{
	}

	void EnemyTestScript::ChangeEnemyState(EntityID id, EnemyState nextState)
	{
		switch (nextState) {
		case EnemyState::IDLE:	std::cout << "IDLE state" << std::endl;
			break;
		case EnemyState::ALERT:std::cout << "ALERT state" << std::endl;
			break;
		case EnemyState::PATROL:std::cout << "PATROL state" << std::endl;
			break;
		case EnemyState::TARGET:std::cout << "TARGET state" << std::endl;
			break;
		}

		m_ScriptData[id].EnemyCurrentState = nextState;

	}

	void EnemyTestScript::RotateToPlayer(EntityID id)
	{
		Transform& currentObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
		Transform& targetObject = PE::EntityManager::GetInstance().Get<PE::Transform>(m_ScriptData[id].playerID);
		float dx = targetObject.position.x - currentObject.position.x;
		float dy = targetObject.position.y - currentObject.position.y;

		float rotation = atan2(dy, dx);
		PE::EntityManager::GetInstance().Get<PE::Transform>(id).orientation = rotation;
	}

	float EnemyTestScript::GetDistanceFromPlayer(EntityID id)
	{
		float distance{ 51 };
		Transform& currentObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
		Transform& targetObject = PE::EntityManager::GetInstance().Get<PE::Transform>(m_ScriptData[id].playerID);
		float dx = targetObject.position.x - currentObject.position.x;
		float dy = targetObject.position.y - currentObject.position.y;

		distance = sqrt(dx * dx + dy * dy);

		//std::cout << distance << std::endl;

		return distance;
	}

}