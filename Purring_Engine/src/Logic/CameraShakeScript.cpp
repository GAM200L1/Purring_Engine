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
#include "LogicSystem.h"

#include "Graphics/Camera.h"
#include "Graphics/CameraManager.h"

namespace PE 
{
	CameraShakeScript::CameraShakeScript()
	{

	}

	void CameraShakeScript::Init(EntityID id)
	{
		id;
	}
	void CameraShakeScript::Update(EntityID id, float deltaTime)
	{
		if (m_ScriptData[id].shakeEnabled)
		{
			if (m_ScriptData[id].elapsedTime < m_ScriptData[id].shakeDuration)
			{
				m_ScriptData[id].elapsedTime += deltaTime;
				float shakeAmount = m_ScriptData[id].shakeAmount * (1 - m_ScriptData[id].elapsedTime / m_ScriptData[id].shakeDuration);
				float x = (rand() % 100) / 100.0f * shakeAmount - shakeAmount * 0.5f;
				float y = (rand() % 100) / 100.0f * shakeAmount - shakeAmount * 0.5f;

				EntityManager::GetInstance().Get<PE::Transform>(id).position = m_ScriptData[id].originalPosition + vec2(x, y);
			}
			else
			{
				EntityManager::GetInstance().Get<PE::Transform>(id).position = m_ScriptData[id].originalPosition;
				m_ScriptData[id].elapsedTime = 0;
				m_ScriptData[id].shakeEnabled = false;
			}
		}
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

	void CameraShakeScript::Shake(EntityID id)
	{
		m_ScriptData[id].originalPosition = EntityManager::GetInstance().Get<PE::Transform>(id).position;
		m_ScriptData[id].shakeEnabled = true;
	}
}