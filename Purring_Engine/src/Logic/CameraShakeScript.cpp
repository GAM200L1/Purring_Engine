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
#include "CameraShakeScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "Animation/Animation.h"
#include "LogicSystem.h"

#include <limits>


# define M_PI           3.14159265358979323846 

namespace PE 
{
	CameraShakeScript::CameraShakeScript()
	{
		REGISTERANIMATIONFUNCTION(TestFunction, PE::CameraShakeScript);
		REGISTERANIMATIONFUNCTION(TestFunction2, PE::CameraShakeScript);
		REGISTERANIMATIONFUNCTION(TestFunction3, PE::CameraShakeScript);


	}

	void CameraShakeScript::Init(EntityID id)
	{
		id;
	}
	void CameraShakeScript::Update(EntityID id, float deltaTime)
	{
		//PE::EntityManager::GetInstance().Get<PE::Transform>(id).orientation += static_cast<float>(180 * (M_PI / 180) * deltaTime * m_ScriptData[id].m_rotationSpeed);

	
	
	}
	void CameraShakeScript::Destroy(EntityID id)
	{
		id;
	}

	CameraShakeScript::~CameraShakeScript()
	{
	}

	void CameraShakeScript::OnAttach(EntityID id)
	{
		m_ScriptData[id] = CameraShakeScriptData();
	}

	void CameraShakeScript::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	std::map<EntityID, CameraShakeScriptData>& CameraShakeScript::GetScriptData()
	{
		return m_ScriptData;
	}

	rttr::instance CameraShakeScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_ScriptData.at(id));
	}

	void CameraShakeScript::TestFunction(EntityID)
	{
		std::cout << "Test Function Called" << std::endl;
	}

	void CameraShakeScript::TestFunction2(EntityID)
	{
		std::cout << "Test Function 2 Called" << std::endl;
	}

	void CameraShakeScript::TestFunction3(EntityID)
	{
		std::cout << "Test Function 3 Called" << std::endl;
	}
}