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
#include "CatScript.h"
//#include "GameStateController.h"
#include "RatScript.h"
#include "GameStateController_v2_0.h"
#include "DeploymentScript.h"
#include "MainMenuController.h"
#include "IntroCutsceneController.h"
#include "FpsScript.h"

#include "Rat/RatScript_v2_0.h"
#include "Rat/RatController_v2_0.h"
#include "UI/HealthBarScript_v2_0.h"
#include "Logic/Rat/RatIdle_v2_0.h"

#include "Cat/CatScript_v2_0.h"
#include "Cat/CatController_v2_0.h"

#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE

std::map<std::string, PE::Script*> PE::LogicSystem::m_scriptContainer;
bool PE::LogicSystem::restartingScene = false;

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
	//REGISTER_SCRIPT(CatScript);
	REGISTER_SCRIPT(RatScript);
	//REGISTER_SCRIPT(GameStateController);
	REGISTER_SCRIPT(GameStateController_v2_0);
	REGISTER_SCRIPT(DeploymentScript);
	REGISTER_SCRIPT(MainMenuController);

	REGISTER_SCRIPT(RatScript_v2_0);
	REGISTER_SCRIPT(RatController_v2_0);
	REGISTER_SCRIPT(HealthBarScript_v2_0);
	//REGISTER_SCRIPT(RatIdle_v2_0);
	REGISTER_SCRIPT(IntroCutsceneController);
	REGISTER_SCRIPT(FpsScript);

	REGISTER_SCRIPT(CatScript_v2_0);
	REGISTER_SCRIPT(CatController_v2_0);
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
					state = ScriptState::DEAD;
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
		{ 
#endif
			m_scriptContainer.find(key)->second->Destroy(id);
			
#ifndef GAMERELEASE
		}
#endif
		val->OnDetach(id);
	}

}
