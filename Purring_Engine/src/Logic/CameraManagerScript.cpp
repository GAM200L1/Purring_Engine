/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CameraManagerScript.cpp
 \date     02-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Script that switches the main camera between multiple cameras in the scene.
	For M2 demo purposes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "CameraManagerScript.h"
#include "Input/InputSystem.h"
#include "Events/EventHandler.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "Graphics/Camera.h"
#include "Graphics/CameraManager.h"
#include "Editor/Editor.h"
namespace PE
{
	void CameraManagerScript::Init(EntityID id)
	{
		ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, CameraManagerScript::OnKeyTriggered, this)
	}

	void CameraManagerScript::Update(EntityID id, float)
	{
		if (m_keyPressed)
		{
			ChangeCamera(id);
			m_keyPressed = false;
		}

	}
	void CameraManagerScript::Destroy(EntityID)
	{

	}
	void CameraManagerScript::OnAttach(EntityID id)
	{
		m_ScriptData[id] = CameraManagerScriptData();
		m_ScriptData[id].CameraIDs.push_back(static_cast<EntityID>(-1));
		m_ScriptData[id].CameraIDs.push_back(static_cast<EntityID>(-1));
		m_ScriptData[id].CameraIDs.push_back(static_cast<EntityID>(-1));
		m_ScriptData[id].CameraIDs.push_back(static_cast<EntityID>(-1));
		m_ScriptData[id].CameraIDs.push_back(static_cast<EntityID>(-1));
		m_ScriptData[id].CameraIDs.push_back(static_cast<EntityID>(-1));
		m_ScriptData[id].CameraIDs.push_back(static_cast<EntityID>(-1));
		m_ScriptData[id].CameraIDs.push_back(static_cast<EntityID>(-1));
		m_ScriptData[id].CameraIDs.push_back(static_cast<EntityID>(-1));
		m_ScriptData[id].CameraIDs.push_back(static_cast<EntityID>(-1));
	}
	void CameraManagerScript::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}
	std::map<EntityID, CameraManagerScriptData>& CameraManagerScript::GetScriptData()
	{
		return m_ScriptData;
	}
	rttr::instance CameraManagerScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_ScriptData.at(id));
	}
	CameraManagerScript::~CameraManagerScript()
	{
	}

	void CameraManagerScript::OnKeyTriggered(const Event<KeyEvents>& r_event)
	{
		PE::KeyTriggeredEvent KTE;

		//dynamic cast
		if (r_event.GetType() == PE::KeyEvents::KeyTriggered)
		{
			KTE = dynamic_cast<const PE::KeyTriggeredEvent&>(r_event);
			if(Editor::GetInstance().IsRunTime())
			if (KTE.keycode >= GLFW_KEY_0 && KTE.keycode <= GLFW_KEY_9)
			{
				m_keyPressed = true;
				m_key = KTE.keycode;
			}
		}
	}
	void CameraManagerScript::ChangeCamera(EntityID id)
	{
		int count = m_key - '0';

		if (count <= m_ScriptData[id].NumberOfCamera)
		{
			if (EntityManager::GetInstance().Has(m_ScriptData[id].CameraIDs[count], EntityManager::GetInstance().GetComponentID<Graphics::Camera>()))
				EntityManager::GetInstance().Get<Graphics::Camera>(m_ScriptData[id].CameraIDs[count]).SetMainCamera(m_ScriptData[id].CameraIDs[count]);
		}

	}
}