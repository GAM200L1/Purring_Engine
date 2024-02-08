/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatController_v2_0.h
 \date     17-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains declarations for functions used for rats.

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "RatController_v2_0.h"
#include "RatScript_v2_0.h"

#include "../Logic/LogicSystem.h"

namespace PE
{
		EntityID RatController_v2_0::mainInstance{};

		// ----- PUBLIC GETTERS ----- //

		std::vector<std::pair<EntityID, EnumRatType>> const& RatController_v2_0::GetRats(EntityID id)
		{
				m_cachedActiveRats.clear();
				if (!(m_scriptData[id].p_ratsMap)) { return m_cachedActiveRats; } // null check

				for (auto const& [ratId, data] : *m_scriptData[id].p_ratsMap)
				{
						try
						{
								// Check that the entity is active and rat is alive
								if (EntityManager::GetInstance().Get<EntityDescriptor>(ratId).isActive && data.isAlive)
								{
										m_cachedActiveRats.emplace_back(std::make_pair(ratId, EnumRatType::GUTTER));
								}
						}
						catch (...) { }
				}

				return m_cachedActiveRats;
		}

		
		unsigned int RatController_v2_0::RatCount(EntityID id)
		{
				if (!(m_scriptData[id].p_ratsMap)) { return 0; } // null check

				unsigned count{};

				for (auto const& [ratId, data] : *m_scriptData[id].p_ratsMap)
				{
						try
						{
								// Check that the entity is active and rat is alive
								if (EntityManager::GetInstance().Get<EntityDescriptor>(ratId).isActive && data.isAlive)
								{
										++count;
								}
						}
						catch (...) { }
				}

				return count;
		}



		// ----- PUBLIC FUNCTIONS ----- //

		void RatController_v2_0::Init(EntityID id)
		{
				// Look for all the rats in the scene
				m_scriptData[id].p_ratsMap = &(dynamic_cast<RatScript_v2_0*>(LogicSystem::m_scriptContainer[GETSCRIPTNAME(RatScript_v2_0)])->m_scriptData);

				// Look for all the spawners in the scene

		}


		void RatController_v2_0::Update(EntityID id, float deltaTime)
		{
				/*if (!ratsPrinted)
				{
						auto const& myVec{ GetRats(id) };
						for (auto const& [ratId, rat] : myVec)
						{
								std::cout << "Rat id: " << ratId << ", type: " << rat << std::endl;
						}
						ratsPrinted = true;
				}*/
		}


		void RatController_v2_0::OnAttach(EntityID id)
		{
				// Make this instance of the rat controller the main instance
				mainInstance = id;

				// Create script instance data
				m_scriptData[id] = RatController_v2_0_Data();

				// Reserve elements in container
				m_cachedActiveRats.reserve(20);
		}


		void RatController_v2_0::OnDetach(EntityID id)
		{
				// Delete this instance's script data
				auto it = m_scriptData.find(id);
				if (it != m_scriptData.end())
				{
						m_scriptData[id].p_ratsMap = nullptr;
						m_scriptData.erase(id);
				}

				// Reset the main instance ID if this instance is the main instance
				if (id == mainInstance) { mainInstance = 0; }

				// Clear the cache
				m_cachedActiveRats.clear();

				ratsPrinted = false;
		}


		std::map<EntityID, RatController_v2_0_Data>& RatController_v2_0::GetScriptData()
		{
				return m_scriptData;
		}


		rttr::instance RatController_v2_0::GetScriptData(EntityID id)
		{
				return rttr::instance(m_scriptData.at(id));
		}

		void RatController_v2_0::ToggleEntity(EntityID id, bool setToActive)
		{
				// Exit if the entity is not valid
				if (!EntityManager::GetInstance().IsEntityValid(id))
				{
						return;
				}

				try
				{
						// Toggle the entity
						EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = setToActive;
				}
				catch (...) { }
		}
}