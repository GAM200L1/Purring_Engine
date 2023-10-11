#include "prpch.h"
#include "LogicSystem.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "testScript.h"
#include "testScript2.h"

std::map<std::string, PE::Script*> PE::LogicSystem::m_scriptContainer;

PE::LogicSystem::LogicSystem()
{
}

PE::LogicSystem::~LogicSystem()
{
}

void PE::LogicSystem::InitializeSystem()
{
	m_scriptContainer["test"] = new testScript();
	m_scriptContainer["test2"] = new testScript2();
}

void PE::LogicSystem::UpdateSystem(float deltaTime)
{
	//loop all objects in scene view
	//get only "script" objects
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
