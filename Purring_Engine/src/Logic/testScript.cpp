/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     testScript.cpp 
 \date     03-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu


 \brief  This file contains the definitions of testScript

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/

#include "prpch.h"
#include "testScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"

#include <limits>


# define M_PI           3.14159265358979323846 

namespace PE 
{
	void testScript::Init(EntityID id)
	{
		id;
	}
	void testScript::Update(EntityID id, float deltaTime)
	{
		PE::EntityManager::GetInstance().Get<PE::Transform>(id).orientation += static_cast<float>(180 * (M_PI / 180) * deltaTime * m_ScriptData[id].m_rotationSpeed);
	}
	void testScript::Destroy(EntityID id)
	{
		id;
	}

	testScript::~testScript()
	{
	}

	void testScript::OnAttach(EntityID id)
	{
		m_ScriptData[id] = TestScriptData();
	}

	void testScript::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	std::map<EntityID, TestScriptData>& testScript::GetScriptData()
	{
		return m_ScriptData;
	}

	rttr::instance testScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_ScriptData.at(id));
	}

}