#include "prpch.h"
#include "LogicSystem.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "testScript.h"
#include "testScript2.h"
#include "../../Application/src/Scripts/EnemyTestScript.h"
#include "PlayerControllerScript.h"
#include "Editor/Editor.h"

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
}

void PE::LogicSystem::UpdateSystem(float deltaTime)
{
	//loop all objects in scene view
	//get only "script" objects
	if(Editor::GetInstance().IsRunTime())
	for (EntityID objectID : SceneView<ScriptComponent>())
	{
		ScriptComponent& sc = EntityManager::GetInstance().Get<ScriptComponent>(objectID);

		for (auto scripts : sc.m_scriptKeys)
		{
			if (m_scriptContainer.find(scripts) != m_scriptContainer.end())
			{
				m_scriptContainer.find(scripts)->second->Update(objectID, deltaTime);
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
