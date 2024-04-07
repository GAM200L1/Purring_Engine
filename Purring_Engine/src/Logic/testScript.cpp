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
#include "Animation/Animation.h"
#include "LogicSystem.h"
#include "Logic/CameraShakeScript.h"
#include "System.h"
#include "Graphics/CameraManager.h"

#include <limits>


# define M_PI           3.14159265358979323846 

namespace PE 
{
	testScript::testScript()
	{
		REGISTERANIMATIONFUNCTION(TestFunction, PE::testScript);
		REGISTERANIMATIONFUNCTION(TestFunction2, PE::testScript);
		REGISTERANIMATIONFUNCTION(TestFunction3, PE::testScript);


	}

	void testScript::Init(EntityID id)
	{
		id;
	}
	void testScript::Update(EntityID id, float deltaTime)
	{
		//PE::EntityManager::GetInstance().Get<PE::Transform>(id).orientation += static_cast<float>(180 * (M_PI / 180) * deltaTime * m_ScriptData[id].m_rotationSpeed);
		m_ScriptData[id].m_timer -= deltaTime;

		if (m_ScriptData[id].m_timer < 0)
		{
			if (!m_ScriptData[id].isCreated)
			{
				m_ScriptData[id].newEntityKey = ADDNEWENTITYTOQUEUE("testObject.prefab");
				m_ScriptData[id].isCreated = true;
			}
			else if (m_ScriptData[id].newEntityKey > -1 && GETCREATEDENTITY(m_ScriptData[id].newEntityKey).has_value())
			{
				Transform curT = EntityManager::GetInstance().Get<PE::Transform>(id);
				PE::EntityManager::GetInstance().Get<PE::Transform>(GETCREATEDENTITY(m_ScriptData[id].newEntityKey).value()).position = vec2(curT.position.x + curT.width, curT.position.y + curT.height);
				m_ScriptData[id].m_timer = 100000;
			}
		}
	
	
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

	void testScript::TestFunction(EntityID)
	{
		std::cout << "Test Function Called" << std::endl;
	}

	void testScript::TestFunction2(EntityID id)
	{
		std::cout << "Test Function 2 Called" << std::endl;
		GETSCRIPTINSTANCEPOINTER(CameraShakeScript)->Shake(GETCAMERAMANAGER()->GetMainCameraId());
	}

	void testScript::TestFunction3(EntityID)
	{
		std::cout << "Test Function 3 Called" << std::endl;
	}
}