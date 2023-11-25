/*!*********************************************************************************** 
 \project  Purring Engine 
 \module   CSD2401-A 
 \file     LogicSystem.h 
 \date     03-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan\@digipen.edu
 \par      code %:     85% 
 \par      changes:    Majority of the code 

 \co-author            FOONG Jun Wei 
 \par      email:      f.junwei\@digipen.edu 
 \par      code %:     15% 
 \par      changes:    Serialize and deserialize scriptcomponent & data


 \brief  This file contains the base class declerations of Scripts class/type

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/
#include "prpch.h"
#include "LogicSystem.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "testScript.h"
#include "testScript2.h"
#include "EnemyTestScript.h"
#include "PlayerControllerScript.h"
#include "FollowScript.h"
#include "CameraManagerScript.h"

#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE

std::map<std::string, PE::Script*> PE::LogicSystem::m_scriptContainer;

PE::LogicSystem::LogicSystem()
{
}
	
PE::LogicSystem::~LogicSystem()
{
}

void PE::LogicSystem::InitializeSystem()
{
	REGISTER_SCRIPT(testScript);
	REGISTER_SCRIPT(testScript2);
	REGISTER_SCRIPT(PlayerControllerScript);
	REGISTER_SCRIPT(EnemyTestScript);
	REGISTER_SCRIPT(FollowScript);
	REGISTER_SCRIPT(CameraManagerScript);
}

void PE::LogicSystem::UpdateSystem(float deltaTime)
{
	//loop all objects in scene view
	//get only "script" objects

#ifndef GAMERELEASE
	if(Editor::GetInstance().IsRunTime())
#endif
	for (EntityID objectID : SceneView<ScriptComponent>())
	{
		if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isActive || !EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isAlive)
			continue;

		ScriptComponent& sc = EntityManager::GetInstance().Get<ScriptComponent>(objectID);
		for (auto& [key, state] : sc.m_scriptKeys)
		{
			if (m_scriptContainer.find(key) != m_scriptContainer.end())
			{
				switch (state)
				{
				case ScriptState::INIT:
					m_scriptContainer.find(key)->second->Init(objectID);
					state = ScriptState::UPDATE;
					break;
				case ScriptState::UPDATE:
					m_scriptContainer.find(key)->second->Update(objectID, deltaTime);
					break;
				case ScriptState::EXIT:
					m_scriptContainer.find(key)->second->Destroy(objectID);
					break;
				}
			}
		}
	}
}

void PE::LogicSystem::DestroySystem()
{
	for (auto & [key, val] : m_scriptContainer)
	{
		delete val;
	}
}

std::string PE::LogicSystem::GetName()
{
	return "LogicSystem";
}

void PE::LogicSystem::DeleteScriptData(EntityID id)
{
	for (auto& [key, val] : m_scriptContainer)
	{
#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
#endif
		m_scriptContainer.find(key)->second->Destroy(id);

		val->OnDetach(id);
	}

}
