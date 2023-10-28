#include "prpch.h"
#include "FollowScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"

namespace PE
{
	void FollowScript::Init(EntityID)
	{
	}

	void FollowScript::Update(EntityID id, float deltaTime)
	{
		LookAt(id);
		vec2 normalizedDirection = m_data[id].Direction.GetNormalized();
		Transform& currentObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
		Transform& targetObject = PE::EntityManager::GetInstance().Get<PE::Transform>(m_data[id].Following);

		// translate to player but this is not what we want
		//if(currentObject.position.x != targetObject.position.x && currentObject.position.y != targetObject.position.y)
		//	currentObject.position += normalizedDirection*m_data[id].speed * deltaTime;
	}

	void FollowScript::Destroy(EntityID)
	{
	}

	void FollowScript::OnAttach(EntityID)
	{
	}

	void FollowScript::OnDetach(EntityID)
	{
	}

	std::map<EntityID, FollowScriptData>& FollowScript::GetScriptData()
	{
		return m_data;
	}

	FollowScript::~FollowScript()
	{
	}

	void FollowScript::LookAt(EntityID id)
	{
		Transform& currentObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
		Transform& targetObject = PE::EntityManager::GetInstance().Get<PE::Transform>(m_data[id].Following);
		float dx = targetObject.position.x - currentObject.position.x;
		float dy = targetObject.position.y - currentObject.position.y;
		m_data[id].Direction = vec2(dx, dy);

		float rotation = atan2(dy, dx);
		PE::EntityManager::GetInstance().Get<PE::Transform>(id).orientation = rotation;
	}

}