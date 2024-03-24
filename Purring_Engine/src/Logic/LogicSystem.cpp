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
#include "ResourceManager/ResourceManager.h"
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
#include "EndingCutsceneController.h"
#include "FpsScript.h"

#include "Rat/RatScript_v2_0.h"
#include "Rat/RatController_v2_0.h"
#include "UI/HealthBarScript_v2_0.h"
#include "Logic/Rat/RatIdle_v2_0.h"

#include "Layers/LayerManager.h"

#include "Logic/Settings.h"

#include "Cat/CatScript_v2_0.h"
#include "Cat/CatController_v2_0.h"

#include "Cat/CatScript_v2_0.h"
#include "Cat/FollowScript_v2_0.h"

#include "Boss/BossRatScript.h"
#include "Boss/BossRatHealthBarScript.h"
#include "ObjectAttachScript.h"


#include "TutorialController.h"
#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE

std::map<std::string, PE::Script*> PE::LogicSystem::m_scriptContainer;
int PE::LogicSystem::m_currentQueueNumber = 0;
std::vector<std::pair<std::optional<EntityID>, std::string>> PE::LogicSystem::m_createScriptObjectQueue;
std::vector<std::pair<std::optional<EntityID>, std::string>> PE::LogicSystem::m_newScriptObjectQueue;


PE::LogicSystem::LogicSystem()
{
	//m_createScriptObjectQueue.reserve(100);
	//m_createScriptObjectQueue.resize(100);
	//m_newScriptObjectQueue.reserve(100);
	//m_newScriptObjectQueue.resize(100);
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
	REGISTER_SCRIPT(EndingCutsceneController);
	REGISTER_SCRIPT(FpsScript);

	REGISTER_SCRIPT(CatScript_v2_0);
	REGISTER_SCRIPT(CatController_v2_0);
	REGISTER_SCRIPT(FollowScript_v2_0);

	REGISTER_SCRIPT(BossRatScript);
	REGISTER_SCRIPT(ObjectAttachScript);
	REGISTER_SCRIPT(BossRatHealthBarScript);

	REGISTER_SCRIPT(SettingsScript);
	REGISTER_SCRIPT(TutorialController);
}

void PE::LogicSystem::UpdateSystem(float deltaTime)
{
	//loop all objects in scene view
	//get only "script" objects

#ifndef GAMERELEASE
	if (Editor::GetInstance().IsRunTime())
	{
#endif
		//ClearCreatedList();
		CreateQueuedObjects();

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



#ifndef GAMERELEASE
	}
#endif
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

void PE::LogicSystem::CreateQueuedObjects()
{
	for(auto i = m_createScriptObjectQueue.begin(); i != m_createScriptObjectQueue.end(); ++i)
	{
		i->first = ResourceManager::GetInstance().LoadPrefabFromFile(i->second);
		m_newScriptObjectQueue.emplace_back(*i);	
	}
	m_createScriptObjectQueue.clear();
}

std::optional<EntityID> PE::LogicSystem::GetCreatedEntity(int key)
{
	return m_newScriptObjectQueue.at(key).first;
}

void PE::LogicSystem::ClearCreatedList()
{
	m_newScriptObjectQueue.clear();
	m_currentQueueNumber = 0;
}

int PE::LogicSystem::AddNewEntityToQueue(std::string prefab)
{
	m_createScriptObjectQueue.push_back(std::make_pair(std::nullopt, prefab));
	return m_currentQueueNumber++;
}


nlohmann::json PE::ScriptComponent::ToJson(EntityID id) const
{
	nlohmann::json ret;

	for (auto [k, v] : m_scriptKeys)
	{
		ret[k.c_str()]["state"] = v;
		rttr::type scriptDataType = rttr::type::get_by_name(k.c_str());
		for (auto& prop : scriptDataType.get_properties())
		{
			rttr::instance inst = PE::LogicSystem::m_scriptContainer.at(k.c_str())->GetScriptData(id);

			if (!inst.is_valid()) // if no data for this script type, just break out of this loop
				break;
			rttr::variant var = prop.get_value(inst);
			if (var.get_type().get_name() == "float")
			{
				float val = var.get_value<float>();
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()] = val;
			}
			else if (var.get_type().get_name() == "enumPE::PlayerState")
			{
				PlayerState val = var.get_value<PlayerState>();
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()] = val;
			}
			else if (var.get_type().get_name() == "int")
			{
				int val = var.get_value<int>();
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()] = val;
			}
			else if (var.get_type().get_name() == "unsigned__int64")
			{
				size_t val = var.get_value<size_t>();
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()] = val;
			}
			else if (var.get_type().get_name() == "bool")
			{
				bool val = var.get_value<bool>();
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()] = val;
			}
			else if (var.get_type().get_name() == "classstd::vector<unsigned__int64,classstd::allocator<unsigned__int64> >")
			{
				std::vector<EntityID> val = var.get_value<std::vector<EntityID>>();
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()] = val;
			}
			else if (var.get_type().get_name() == "structPE::vec2")
			{
				PE::vec2 val = var.get_value<PE::vec2>();
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["x"] = val.x;
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["y"] = val.y;
			}
			else if (var.get_type().get_name() == "structPE::vec4")
			{
				PE::vec4 val = var.get_value<PE::vec4>();
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["x"] = val.x;
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["y"] = val.y;
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["z"] = val.z;
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["w"] = val.w;
			}
			else if (var.get_type().get_name() == "classstd::vector<structPE::vec2,classstd::allocator<structPE::vec2> >")
			{
				std::vector<PE::vec2> val = var.get_value<std::vector<PE::vec2>>();
				size_t cnt{};
				for (const auto& vec : val)
				{
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()][std::to_string(cnt)]["x"] = vec.x;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()][std::to_string(cnt)]["y"] = vec.y;
					++cnt;
				}
			}
			else if (var.get_type().get_name() == "classstd::map<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>,classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>,structstd::less<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>>,classstd::allocator<structstd::pair<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>const,classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>>> >")
			{
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()] = var.get_value<std::map<std::string, std::string>>();
			}
			else if (var.get_type().get_name() == "enumPE::EnumCatType")
			{
				PE::EnumCatType val = var.get_value<EnumCatType>();
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()] = val;
			}
			else if (var.get_type().get_name() == "classstd::variant<structPE::GreyCatAttackVariables,structPE::OrangeCatAttackVariables>")
			{
				auto vari = var.get_value<std::variant<GreyCatAttackVariables, OrangeCatAttackVariables>>();
				if (!vari.index())
				{
					GreyCatAttackVariables val = std::get<GreyCatAttackVariables>(vari);
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["GreyCatAttackVariables"]["projectileID"] = val.projectileID;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["GreyCatAttackVariables"]["damage"] = val.damage;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["GreyCatAttackVariables"]["attacKDirection"] = val.attackDirection;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["GreyCatAttackVariables"]["teleGraphIDs"] = val.telegraphIDs;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["GreyCatAttackVariables"]["bulletDelay"] = val.bulletDelay;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["GreyCatAttackVariables"]["bulletRange"] = val.bulletRange;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["GreyCatAttackVariables"]["bulletLifeTime"] = val.bulletLifeTime;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["GreyCatAttackVariables"]["bulletForce"] = val.bulletForce;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["GreyCatAttackVariables"]["bulletFireAnimationIndex"] = val.bulletFireAnimationIndex;
				}
				else
				{
					OrangeCatAttackVariables val = std::get<OrangeCatAttackVariables>(vari);
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["OrangeCatAttackVariables"]["seismicID"] = val.seismicID;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["OrangeCatAttackVariables"]["telegraphID"] = val.telegraphID;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["OrangeCatAttackVariables"]["damage"] = val.damage;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["OrangeCatAttackVariables"]["seismicRadius"] = val.seismicRadius;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["OrangeCatAttackVariables"]["seismicDelay"] = val.seismicDelay;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["OrangeCatAttackVariables"]["seismicForce"] = val.seismicForce;
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()]["OrangeCatAttackVariables"]["seismicSlamAnimationIndex"] = val.seismicSlamAnimationIndex;
				}
			}
			else if (var.get_type().get_name() == "unsignedint")
			{
				unsigned val = var.get_value<unsigned>();
				ret[k.c_str()]["data"][prop.get_name().to_string().c_str()] = val;
			}
			else if (var.get_type().get_name() == "enumPE::EnumRatType")
			{
					PE::EnumRatType val = var.get_value<EnumRatType>();
					ret[k.c_str()]["data"][prop.get_name().to_string().c_str()] = val;
			}
			else
			{
				std::cout << var.get_type().get_name() << std::endl;
			}
		}
	}

	return ret;
}
