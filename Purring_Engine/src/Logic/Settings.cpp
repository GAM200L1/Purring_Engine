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
#include "Settings.h"
#include "ECS/Entity.h"

#include "GUISystem.h"

namespace PE 
{
	SettingsScript::SettingsScript()
	{

	}

	void SettingsScript::Init(EntityID id)
	{
		id;
	}
	void SettingsScript::Update(EntityID id, float deltaTime)
	{
		id;
		deltaTime;

	}
	void SettingsScript::Destroy(EntityID id)
	{
		id;
	}

	SettingsScript::~SettingsScript()
	{
	}

	void SettingsScript::OnAttach(EntityID id)
	{
		m_scriptData[id] = SettingsScriptData();
	}

	void SettingsScript::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
			m_scriptData.erase(id);
	}

	std::map<EntityID, SettingsScriptData>& SettingsScript::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance SettingsScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}
}