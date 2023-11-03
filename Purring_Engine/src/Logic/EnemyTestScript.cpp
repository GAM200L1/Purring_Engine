/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EnemyTestScript.cpp
 \date     02-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Script that controls the enemy states. For M2 demo purposes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "EnemyTestScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "LogicSystem.h"

#include <limits>


# define M_PI   3.14159265358979323846 
//#include "Input/InputSystem.h"

namespace PE {

	void EnemyTestScript::Init(EntityID id)
	{
		m_ScriptData[id].m_stateManager = new StateMachine();
		m_ScriptData[id].m_stateManager->ChangeState(new EnemyTestIDLE(),id);
	}

	void EnemyTestScript::Update(EntityID id, float deltaTime)
	{
		m_ScriptData[id].distanceFromPlayer = GetDistanceFromPlayer(id);
		m_ScriptData[id].m_stateManager->Update(id, deltaTime);

		if (m_ScriptData[id].Health <= 0)
			EntityManager::GetInstance().RemoveEntity(EntityManager::GetInstance().GetEntitiesInPool(ALL)[id]);
	}

	void EnemyTestScript::Destroy(EntityID)
	{
	}

	void EnemyTestScript::OnAttach(EntityID id)
	{
		if (!EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<RigidBody>()))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<RigidBody>() });
			EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
		}
		else
		{
			EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
		}
		if (m_ScriptData.find(id) == m_ScriptData.end())
		{
			m_ScriptData[id] = EnemyTestScriptData();
		}
		else
		{
			delete m_ScriptData[id].m_stateManager;
			m_ScriptData[id] = EnemyTestScriptData();
		}
	}

	void EnemyTestScript::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
		{
			delete m_ScriptData.at(id).m_stateManager;
			m_ScriptData.erase(id);
		}
	}

	std::map<EntityID, EnemyTestScriptData>& EnemyTestScript::GetScriptData()
	{
		return m_ScriptData;
	}

	rttr::instance EnemyTestScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_ScriptData.at(id));
	}

	EnemyTestScript::~EnemyTestScript()
	{
		for (auto& [key, val] : m_ScriptData)
		{
			delete val.m_stateManager;
		}
	}

	float EnemyTestScript::GetDistanceFromPlayer(EntityID id)
	{
		float distance{ 51 };
		Transform& currentObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
		Transform& targetObject = PE::EntityManager::GetInstance().Get<PE::Transform>(m_ScriptData[id].playerID);
		float dx = targetObject.position.x - currentObject.position.x;
		float dy = targetObject.position.y - currentObject.position.y;

		distance = sqrt(dx * dx + dy * dy);

		return distance;
	}

	void EnemyTestIDLE::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(EnemyTestScript, id);
		p_data->idleTimer = p_data->timerBuffer;
	}

	void EnemyTestIDLE::StateUpdate(EntityID id, float deltaTime)
	{			
		p_data->idleTimer -= deltaTime;
		//PE::EntityManager::GetInstance().Get<PE::Transform>(id).orientation += static_cast<float>(180 * (M_PI / 180) * deltaTime * 10);

		if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<AnimationComponent>()))
		{
			EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationIndex("ratAttack");
		}

		if (p_data->idleTimer <= 0)
		{
			p_data->patrolTimer = p_data->timerBuffer;
			p_data->m_stateManager->ChangeState(new EnemyTestPATROL(),id);
			return;
		}
	}

	void EnemyTestIDLE::StateExit(EntityID)
	{
	}

	std::string_view EnemyTestIDLE::GetName()
	{
		return "IDLE";
	}

	void EnemyTestPATROL::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(EnemyTestScript, id);
		p_data->bounce = true;
		p_data->patrolTimer = p_data->patrolBuffer;
	}

	void EnemyTestPATROL::StateUpdate(EntityID id, float deltaTime)
	{
		if (p_data->bounce)
			EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ 0.f,.25f } *p_data->speed);
		else
			EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ 0.f,-.25f } *p_data->speed);

		p_data->patrolTimer -= deltaTime;

		if (abs(p_data->distanceFromPlayer) <= p_data->TargetRange)
		{
			p_data->m_stateManager->ChangeState(new EnemyTestALERT(), id);
			return;
		}

		if (p_data->patrolTimer < p_data->patrolBuffer/2.0f && p_data->patrolTimer > 0.f)
		{
			p_data->bounce = false;
		}
		else if (p_data->patrolTimer < 0)
		{
			p_data->m_stateManager->ChangeState(new EnemyTestIDLE(), id);
			return;
		}
	}

	void EnemyTestPATROL::StateExit(EntityID)
	{
	}

	std::string_view EnemyTestPATROL::GetName()
	{
		return "PATROL";
	}

	void EnemyTestALERT::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(EnemyTestScript, id);
		p_data->alertTimer = p_data->timerBuffer;
	}

	void EnemyTestALERT::StateUpdate(EntityID id, float deltaTime)
	{
		p_data->alertTimer -= deltaTime;

		if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<AnimationComponent>()))
		{
			EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationIndex("ratAttack");
		}

		if (p_data->alertTimer <= 0 && abs(p_data->distanceFromPlayer) <= p_data->TargetRange)
		{
			p_data->m_stateManager->ChangeState(new EnemyTestTARGET(), id);
			return;
		}
		else if (p_data->alertTimer <= 0)
		{
			p_data->idleTimer = p_data->timerBuffer;
			p_data->m_stateManager->ChangeState(new EnemyTestIDLE(), id);
			return;
		}
	}

	void EnemyTestALERT::StateExit(EntityID)
	{
	}

	std::string_view EnemyTestALERT::GetName()
	{
		return "ALERT";
	}
	void EnemyTestTARGET::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(EnemyTestScript, id);
		p_data->alertTimer = p_data->timerBuffer/2.f;
	}
	void EnemyTestTARGET::StateUpdate(EntityID id, float deltaTime)
	{
		Transform& currentObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
		Transform& targetObject = PE::EntityManager::GetInstance().Get<PE::Transform>(p_data->playerID);
		float dx = targetObject.position.x - currentObject.position.x;
		float dy = targetObject.position.y - currentObject.position.y;

		float rotation = atan2(dy, dx);
		PE::EntityManager::GetInstance().Get<PE::Transform>(id).orientation = rotation;

		p_data->alertTimer -= deltaTime;

		if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<AnimationComponent>()))
		{
			EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationIndex("ratDeath");
		}

		if (p_data->alertTimer < 0 && p_data->distanceFromPlayer <= p_data->TargetRange)
		{
			p_data->m_stateManager->ChangeState(new EnemyTestATTACK(), id);
			return;
		}
		else if (p_data->alertTimer < 0)
		{
			p_data->m_stateManager->ChangeState(new EnemyTestIDLE(), id);
			return;
		}

	}
	void EnemyTestTARGET::StateExit(EntityID)
	{
	}
	std::string_view EnemyTestTARGET::GetName()
	{
		return "TARGET";
	}
	void EnemyTestATTACK::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(EnemyTestScript, id);
	}
	void EnemyTestATTACK::StateUpdate(EntityID id, float)
	{
		Transform& currentObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
		Transform& targetObject = PE::EntityManager::GetInstance().Get<PE::Transform>(p_data->playerID);
		vec2 toPlayer(targetObject.position.x - currentObject.position.x, targetObject.position.y - currentObject.position.y);

		if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<AnimationComponent>()))
		{
			EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationIndex("ratDeath");
		}

		EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(toPlayer * p_data->speed/2);

		p_data->m_stateManager->ChangeState(new EnemyTestTARGET(), id);
	}
	void EnemyTestATTACK::StateExit(EntityID)
	{
	}
	std::string_view EnemyTestATTACK::GetName()
	{
		return "ATTACK";
	}
}