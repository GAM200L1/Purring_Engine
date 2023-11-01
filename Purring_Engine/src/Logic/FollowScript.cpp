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

		Transform& currentObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
		Transform& targetObject = PE::EntityManager::GetInstance().Get<PE::Transform>(m_ScriptData[id].entityToFollow);

		vec2 Direction = vec2(targetObject.position.x - currentObject.position.x, targetObject.position.y - currentObject.position.y);

		vec2 normalizedDirection = Direction.GetNormalized();


		//translate to player but this is not what we want
		if(currentObject.position.x != targetObject.position.x && currentObject.position.y != targetObject.position.y)
			currentObject.position += normalizedDirection* m_ScriptData[id].speed * deltaTime;

		//we want it to keep a certain distance behind the player as the player move.
		//should keep track of player's position in nodes, each node will be each following player
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
		Transform& currentObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
		Transform& targetObject = PE::EntityManager::GetInstance().Get<PE::Transform>(m_ScriptData[id].entityToFollow);
		float dx = targetObject.position.x - currentObject.position.x;
		float dy = targetObject.position.y - currentObject.position.y;
		float rotation = atan2(dy, dx);
		PE::EntityManager::GetInstance().Get<PE::Transform>(id).orientation = rotation;
	}

}