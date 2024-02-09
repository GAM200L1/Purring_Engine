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
//#include "RatScript_v2_0.h"
#include "../RatScript.h"

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
								if (EntityManager::GetInstance().Get<EntityDescriptor>(ratId).isActive)// && data.isAlive)
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
								if (EntityManager::GetInstance().Get<EntityDescriptor>(ratId).isActive)// && data.isAlive)
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
				m_scriptData[id].p_ratsMap = &(dynamic_cast<RatScript*>(LogicSystem::m_scriptContainer[GETSCRIPTNAME(RatScript)])->m_scriptData);

				// Look for all the spawners in the scene

		}


		void RatController_v2_0::Update(EntityID id, float deltaTime)
		{
				if (!ratsPrinted)
				{
					auto const& myVec{ GetRats(id) };
					for (auto const& [ratId, rat] : myVec)
					{
							std::cout << "Rat id: " << ratId << ", type: " << rat << std::endl;
					}
					ratsPrinted = true;
				}
				else
				{
					// If rats have already been printed, refresh the m_cachedActiveRats without printing
					GetRats(id);
				}
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

		void RatController_v2_0::ApplyDamageToRat(EntityID ratID, int damage)
		{
			// Check if the given ID is valid and refers to an active, alive rat
			if (!IsRatAndIsAlive(ratID))
			{
				//std::cout << "Rat ID: " << ratID << " is not valid or the rat is not alive." << std::endl;
				return;
			}

			// Access the rat's script data from the map
			auto& ratsMap = *m_scriptData[mainInstance].p_ratsMap;
			auto it = ratsMap.find(ratID);

			if (it != ratsMap.end())
			{
				GETSCRIPTINSTANCEPOINTER(RatScript)->LoseHP(it->second.ratID, damage);

				//// Subtract the damage from the rat's health
				//it->second.ratHealth -= damage;

				//// Check if the rat's health drops below or equals zero
				//if (it->second.ratHealth <= 0)
				//{
				//	// Handle the rat's death (e.g., make it inactive, trigger death animation, etc.)
				//	//std::cout << "Rat ID: " << ratID << " has been defeated." << std::endl;
				//	ToggleEntity(ratID, false);  // Making the rat entity inactive
				//	it->second.isAlive = false;  // Marking the rat as not alive
				//}
			}
			else
			{
				//std::cout << "Rat ID: " << ratID << " not found in rats map." << std::endl;
			}
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

		bool RatController_v2_0::IsRatAndIsAlive(EntityID id) //@yeni
		{
			// Iterate through the cached active rats to find the rat with the given ID
			for (const auto& [ratId, ratType] : m_cachedActiveRats)
			{
				// Check if the current rat ID matches the given ID
				if (ratId == id)
				{
					return true;								// Since the rat is in the cached active rats vector, it is alive
				}
			}

			// If the rat ID was not found in the cached active rats, it is not alive
			return false;
		}

}