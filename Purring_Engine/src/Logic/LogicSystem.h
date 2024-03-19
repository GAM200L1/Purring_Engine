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


 \brief  This file contains the base class declaration of Scripts class/type

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/
#pragma once
#include "System.h"
#include "Script.h"
#include "Data/json.hpp"
#include "PlayerControllerScript.h"
#include "Math/MathCustom.h"
#include <variant>

#define REGISTER_SCRIPT(name) 	PE::LogicSystem::m_scriptContainer[#name] = new name()
#define GETSCRIPTDATA(script,id) &reinterpret_cast<script*>(LogicSystem::m_scriptContainer[#script])->GetScriptData()[id]
#define CHECKSCRIPTDATA(script, id) reinterpret_cast<script*>(LogicSystem::m_scriptContainer[#script])->GetScriptData().count(id)
#define GETSCRIPTINSTANCEPOINTER(script) reinterpret_cast<script*>(LogicSystem::m_scriptContainer[#script])
#define GETSCRIPTNAME(script) #script
//will return a key to get the object next frame
#define ADDNEWENTITYTOQUEUE(prefab) PE::LogicSystem::AddNewEntityToQueue(prefab)
#define GETCREATEDENTITY(key) PE::LogicSystem::GetCreatedEntity(key)

namespace PE {
	class LogicSystem : public System
	{
	public:
		static std::map<std::string, Script*> m_scriptContainer;
		static int m_currentQueueNumber;
		static std::vector<std::pair<std::optional<EntityID>, std::string>> m_createScriptObjectQueue;
		static std::vector<std::pair<std::optional<EntityID>, std::string>> m_newScriptObjectQueue;
		static void ClearCreatedList();
	public:
		LogicSystem();
		virtual ~LogicSystem();

	public:
		/*!***********************************************************************************
		 \brief     Initialize the system. Should be called once before any updates.
		*************************************************************************************/
		void InitializeSystem() override;

		/*!***********************************************************************************
		\brief     Update the system each frame.
		\param     deltaTime Time passed since the last frame, in seconds.
		*************************************************************************************/
		void UpdateSystem(float deltaTime) override;

		/*!***********************************************************************************
		 \brief     Clean up the system resources. Should be called once after all updates.
		*************************************************************************************/
		void DestroySystem() override;

		/*!***********************************************************************************
		 \brief     Get the system's name, useful for debugging and identification.
		 \return    std::string The name of the system.
		*************************************************************************************/
		std::string GetName() override;
	
		/*!***********************************************************************************
		 \brief						To be called when deleting an object to delete all related data
		 \param [In] EntityID id	The ID to delete data from
		*************************************************************************************/
		static void DeleteScriptData(EntityID id);

		static std::optional<EntityID> GetCreatedEntity(int key);
		static int AddNewEntityToQueue(std::string prefab);

	private:
		void CreateQueuedObjects();


	private:
	};

	//holds script on an object
	//probably need to be able to hold multiple scripts
	//nevermind each script is its own component
	//so in each script it will have to register itself as a component

	struct ScriptComponent
	{
		ScriptComponent() {}
		std::map<std::string, ScriptState> m_scriptKeys;

		/*!***********************************************************************************
		 \brief Adds a script to the container and calls the attach function
		 
		 \param[in,out] key - Key of the script state
		 \param[in,out] id - ID of the script
		*************************************************************************************/
		void addScript(std::string key, EntityID id)
		{
			auto itr = m_scriptKeys.find(key);
			if (itr == m_scriptKeys.end())
			{
				m_scriptKeys[key] = ScriptState::INIT;
				LogicSystem::m_scriptContainer[key]->OnAttach(id);
			}
		}

		/*!***********************************************************************************
		 \brief Removes a script from the container 
		 
		 \param[in,out] key - Key of the script state
		 \param[in,out] id - ID of the script
		*************************************************************************************/
		void removeScript(std::string key, EntityID id)
		{
			auto itr = m_scriptKeys.find(key);
			if(itr != m_scriptKeys.end())
			m_scriptKeys.erase(itr);
			LogicSystem::m_scriptContainer[key]->OnDetach(id);
		}

		/*!***********************************************************************************
		 \brief Serializes the data to a JSON file
		 
		 \param[in,out] id - ID of script to get the data of
		 \return nlohmann::json - JSON object with data from script
		*************************************************************************************/
		nlohmann::json ToJson(EntityID id) const;
		

		/*!***********************************************************************************
		 \brief Load the script data from a file
		 
		 \param[in,out] j - JSON object with data to laod into the script
		 \return ScriptComponent& - Reference to updated script component
		*************************************************************************************/
		ScriptComponent& Deserialize(const nlohmann::json& j)
		{
			for (const auto& k : j.items())
			{
				for (const auto& l : k.value().items())
				{
					if (l.key() == "state")
					{
						m_scriptKeys[k.key().c_str()] = l.value().get<ScriptState>();
						break; // can leave once state set, values for data cant be set here~~
					}
					
				}
			}
			return *this;
		}

		~ScriptComponent(){}
	};

}