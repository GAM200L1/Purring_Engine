/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     ObjectAttachScript.cpp
 \date     06-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu


 \brief  This file contains the definitions of ObjectAttachScript

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "ObjectAttachScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"

namespace PE
{
	void ObjectAttachScript::Init(EntityID id)
	{
		id;
	}
	void ObjectAttachScript::Update(EntityID id, float deltaTime)
	{
		if (EntityManager::GetInstance().Has<Transform>(id))
		{
			if (EntityManager::GetInstance().Has<Transform>(m_scriptData[id].ObjectToAttachTo))
			{
				EntityManager::GetInstance().Get<Transform>(id).position = EntityManager::GetInstance().Get<Transform>(m_scriptData[id].ObjectToAttachTo).position;
			}
		}
	}
	void ObjectAttachScript::Destroy(EntityID id)
	{
		id;
	}

	ObjectAttachScript::~ObjectAttachScript()
	{
	}

	void ObjectAttachScript::OnAttach(EntityID id)
	{
		m_scriptData[id] = ObjectAttachScriptData();
	}

	void ObjectAttachScript::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
			m_scriptData.erase(id);
	}

	std::map<EntityID, ObjectAttachScriptData>& ObjectAttachScript::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance ObjectAttachScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

}