#pragma once
#include "System.h"
#include "Script.h"

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

		//to load scripts into the script vector?
		//void LoadScripts();
	private:
		//this is to hold all the existing types of scripts?
		//may need to change to a map with a key to a script
		//std::vector<Script> m_scriptContainer;
		Script* test;
	};

	//holds script on an object
	//probably need to be able to hold multiple scripts
	//nevermind each script is its own component
	//so in each script it will have to register itself as a component

	struct ScriptComponent
	{
		std::vector<std::string> m_scriptKeys;
		ScriptComponent() 
		{ 
			//m_scriptKeys.push_back("test");
			//m_scriptKeys.push_back("test2");
		}

		void addScript(std::string key)
		{
			auto itr = std::find(m_scriptKeys.begin(), m_scriptKeys.end(), key);
			if (itr == m_scriptKeys.end())
			m_scriptKeys.push_back(key);
		}
		void removeScript(std::string key)
		{
			auto itr = std::find(m_scriptKeys.begin(), m_scriptKeys.end(), key);
			if(itr != m_scriptKeys.end())
			m_scriptKeys.erase(itr);
		}
		void removeScript(int keycode)
		{
				m_scriptKeys.erase(m_scriptKeys.begin() + keycode);
		}

	};

}