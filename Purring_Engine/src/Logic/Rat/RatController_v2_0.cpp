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
//#include "../RatScript.h"

#include "../Logic/LogicSystem.h"

namespace PE
{
		EntityID RatController_v2_0::mainInstance{};

		// ----- PUBLIC GETTERS ----- //

		std::vector<std::pair<EntityID, EnumRatType>> const& RatController_v2_0::GetRats(EntityID id)
		{
				RefreshRats(id);
				return m_cachedActiveRats;
		}

		
		unsigned int RatController_v2_0::RatCount(EntityID id)
		{
				RefreshRats(id); // I don't know if the rat controller's refresh
				return static_cast<unsigned int>(m_cachedActiveRats.size());
		}



		// ----- PUBLIC FUNCTIONS ----- //

		void RatController_v2_0::Init(EntityID id)
		{
				// Store a pointer to the maps with the rat scripts
				m_scriptData[id].p_ratsMap = &(dynamic_cast<RatScript*>(LogicSystem::m_scriptContainer[GETSCRIPTNAME(RatScript)])->m_scriptData);
				m_scriptData[id].p_ratsV2Map = &(dynamic_cast<RatScript_v2_0*>(LogicSystem::m_scriptContainer[GETSCRIPTNAME(RatScript_v2_0)])->m_scriptData);
		}


		void RatController_v2_0::Update(EntityID id, float)
		{
				// Allow the cached rats to be updated once this frame
				refreshedThisFrame = false; 

				RefreshRats(id); // Refresh the cached rats

				if (!ratsPrinted)
				{
					auto const& myVec{ GetRats(id) };
					for (auto const& [ratId, rat] : myVec)
					{
							std::cout << "Rat id: " << ratId << ", type: " << (char)rat << std::endl;
					}
					ratsPrinted = true;
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

		void RatController_v2_0::ApplyDamageToRat(EntityID ratID, EntityID attackId, int damage)
		{
			RefreshRats(mainInstance);

			// Iterate through the cached active rats to find the rat with the given ID
			for (const auto& [cachedRatId, ratType] : m_cachedActiveRats)
			{
				// Check if the current rat ID matches the given ID
				if (ratID == cachedRatId)
				{
					if (ratType == EnumRatType::GUTTER_V1)
					{				
						// Check if the rat has hit this cat before
						if((GETSCRIPTDATA(RatScript, ratID).hitBy) &&
						(GETSCRIPTDATA(RatScript, ratID).hitBy)->find(attackId) == (GETSCRIPTDATA(RatScript, ratID).hitBy)->end())
						{
							GETSCRIPTINSTANCEPOINTER(RatScript)->LoseHP(ratID, damage);
						}
						(GETSCRIPTDATA(RatScript, ratID).hitBy)->emplace(attackId);
					}
					else
					{
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
					} // end of if (ratType == EnumRatType::GUTTER_V1)
					break; 
				} // end of if (ratID == cachedRatId)

			}
		} // end of ApplyDamageToRat()

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
			if(EntityManager::GetInstance().Has<EntityDescriptor>(id))
			{
				// Toggle the entity
				EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = setToActive;
			}
		}

		bool RatController_v2_0::IsRatAndIsAlive(EntityID id) const 
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

		int RatController_v2_0::GetRatHealth(EntityID id) const
		{
			// Iterate through the cached active rats to find the rat with the given ID
			for (const auto& [ratId, ratType] : m_cachedActiveRats)
			{
				// Check if the current rat ID matches the given ID
				if (ratId == id)
				{
					if (ratType == EnumRatType::GUTTER_V1)
					{
						return *(GETSCRIPTDATA(RatScript, id).health);
					}
					else
					{
						return *(GETSCRIPTDATA(RatScript_v2_0, id).ratHealth);
					} // end of if (ratType == EnumRatType::GUTTER_V1)
				}
			}

			// ID couldn't be found in cached rats
			return 0;
		}

		int RatController_v2_0::GetRatMaxHealth(EntityID id) const
		{
			// Iterate through the cached active rats to find the rat with the given ID
			for (const auto& [ratId, ratType] : m_cachedActiveRats)
			{
				// Check if the current rat ID matches the given ID
				if (ratId == id)
				{
					if (ratType == EnumRatType::GUTTER_V1)
					{
						return *(GETSCRIPTDATA(RatScript, id).maxHealth);
					}
					else
					{
						return *(GETSCRIPTDATA(RatScript_v2_0, id).ratMaxHealth);
					} // end of if (ratType == EnumRatType::GUTTER_V1)
				}
			}

			// ID couldn't be found in cached rats
			return 0;
		}

		void RatController_v2_0::RefreshRats(EntityID id)
		{
			// Ensure that the container of rats is only refreshed once per frame
			// Not too sure if this will cause issues later
			if(refreshedThisFrame) { return; }
			refreshedThisFrame = true;

			m_cachedActiveRats.clear();

			if (!(m_scriptData[id].p_ratsMap) || !(m_scriptData[id].p_ratsV2Map)) { return; } // null check - this null check didn't make any sense lmao

			//	Get all rats that still have the V1 script on them
			for (auto const& [ratId, data] : *m_scriptData[id].p_ratsMap)
			{
				// Check that the entity is active and rat is alive
				if (EntityManager::GetInstance().Has<EntityDescriptor>(ratId) &&
					EntityManager::GetInstance().Get<EntityDescriptor>(ratId).isActive)// && data.isAlive)
				{
					m_cachedActiveRats.emplace_back(std::make_pair(ratId, EnumRatType::GUTTER_V1));
				}
			}

			// Get all the rats with the V2 script on them
			for (auto const& [ratId, data] : *m_scriptData[id].p_ratsV2Map)
			{
				// Check that the entity is active and rat is alive
				if (EntityManager::GetInstance().Has<EntityDescriptor>(ratId) &&
					EntityManager::GetInstance().Get<EntityDescriptor>(ratId).isActive && data.isAlive)
				{
					m_cachedActiveRats.emplace_back(std::make_pair(ratId, data.ratType));
				}
			}
		}

}