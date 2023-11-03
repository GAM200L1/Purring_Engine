#pragma once
#include "System.h"
#include "Script.h"
#include "Data/json.hpp"
#include "PlayerControllerScript.h"
#include "Math/MathCustom.h"

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

		nlohmann::json ToJson(EntityID id) const
		{
			nlohmann::json ret;
			/*rttr::type currType = rttr::type::get_by_name(PE::EntityManager::GetInstance().GetComponentID<ScriptComponent>().to_string());
			for (auto& prop : currType.get_properties())
			{
				std::string nm(prop.get_name());	
				rttr::variant vp = prop.get_value(m_scriptKeys);
			}*/
			for (auto [k, v] : m_scriptKeys)
			{
				ret[k.c_str()]["state"] = v;
				//ret[k.c_str()]["data"] = 69;
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
					
				}
			}
		
			return ret;
		}

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