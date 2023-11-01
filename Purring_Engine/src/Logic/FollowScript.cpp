#include "prpch.h"
#include "FollowScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
# define M_PI           3.14159265358979323846 // temp definition of pi, will need to discuss where shld we leave this later on

namespace PE
{
	void FollowScript::Init(EntityID)
	{
	}

	void FollowScript::Update(EntityID id, float deltaTime)
	{
		vec2 NewPosition = EntityManager::GetInstance().Get<Transform>(id).position;

		if (!(NewPosition.x == m_ScriptData[id].CurrentPosition.x && NewPosition.y == m_ScriptData[id].CurrentPosition.y)) 
		{
			vec2 directionalvector = NewPosition - m_ScriptData[id].CurrentPosition;
			float newRotation = atan2(directionalvector.y, directionalvector.x);
			m_ScriptData[id].NextPosition = NewPosition + vec2(m_ScriptData[id].Distance * cosf(newRotation - M_PI), m_ScriptData[id].Distance * sinf(newRotation - M_PI));

			EntityManager::GetInstance().Get<Transform>(m_ScriptData[id].FollowingObject[0]).position = m_ScriptData[id].NextPosition;

			float rotationOffset = newRotation - m_ScriptData[id].Rotation;

			if (rotationOffset != 0)
				EntityManager::GetInstance().Get<Transform>(id).orientation = EntityManager::GetInstance().Get<Transform>(id).orientation + rotationOffset;

			EntityManager::GetInstance().Get<Transform>(m_ScriptData[id].FollowingObject[0]).orientation = EntityManager::GetInstance().Get<Transform>(id).orientation;

			m_ScriptData[id].Rotation = newRotation;
			m_ScriptData[id].CurrentPosition = EntityManager::GetInstance().Get<Transform>(id).position;
		}

	}

	void FollowScript::Destroy(EntityID)
	{

	}

	void FollowScript::OnAttach(EntityID id)
	{
		m_ScriptData[id] = FollowScriptData();
	}

	void FollowScript::OnDetach(EntityID)
	{
	}

	std::map<EntityID, FollowScriptData>& FollowScript::GetScriptData()
	{
		return m_ScriptData;
	}

	FollowScript::~FollowScript()
	{
	}

	void FollowScript::LookAt(EntityID id)
	{

	}

}