#pragma once
#include "System.h"
#include "Script.h"

#define REGISTER_SCRIPT(name) 	PE::LogicSystem::m_scriptContainer[#name] = new name()
#define GETSCRIPTDATA(script,id) &reinterpret_cast<script*>(LogicSystem::m_scriptContainer[#script])->GetScriptData()[id]

namespace PE {
	class LogicSystem : public System
	{
	public:
		static std::map<std::string, Script*> m_scriptContainer;
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
	};

	//holds script on an object
	//probably need to be able to hold multiple scripts
	//nevermind each script is its own component
	//so in each script it will have to register itself as a component

	// The ScriptComponent struct is used to manage multiple scripts in an entity - Hans
	struct ScriptComponent
	{
		// Default constructor
		ScriptComponent() {}

		//std::vector<std::string> m_scriptKeys;

		// A map to store script keys along with their state (represented by ScriptState enum) - Hans
		std::map<std::string, ScriptState> m_scriptKeys;
		void addScript(std::string key, EntityID id)
		{
			// Search for the script key in the map - Hans
			auto itr = m_scriptKeys.find(key);

			// If the key is not already in the map, add it with an initial state of ScriptState::INIT - Hans
			if (itr == m_scriptKeys.end())
			{
				m_scriptKeys[key] = ScriptState::INIT;
				LogicSystem::m_scriptContainer[key]->OnAttach(id);
			}
		}
		void removeScript(std::string key, EntityID id)
		{
			// Search for the script key in the map - Hans
			auto itr = m_scriptKeys.find(key);

			// If the key is found, erase it from the map - Hans
			if(itr != m_scriptKeys.end())
			m_scriptKeys.erase(itr);
			LogicSystem::m_scriptContainer[key]->OnDetach(id);
		}

		// Destructor 
		~ScriptComponent(){}
	};
}