/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     DemoController.h
 \date     17-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains declarations for functions used for rats.

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "DemoController.h"

#include "../Logic/LogicSystem.h"

namespace PE
{
		EntityID DemoController::mainInstance{};

		// ----- PUBLIC GETTERS ----- //

		std::vector<std::pair<EntityID, EnumOldRatType>> const& DemoController::GetRats(EntityID id)
		{
				m_cachedActiveRats.clear();
				if (!(m_scriptData[id].p_ratsMap)) { return m_cachedActiveRats; } // null check

				for (auto const& [ratId, data] : *m_scriptData[id].p_ratsMap)
				{
						try
						{
								// Check that the entity is active and rat is alive
								if (EntityManager::GetInstance().Get<EntityDescriptor>(ratId).isActive)
								{
										m_cachedActiveRats.emplace_back(std::make_pair(ratId, EnumOldRatType::GUTTER));
								}
						}
						catch (...) { }
				}

				return m_cachedActiveRats;
		}
		
		std::vector<std::pair<EntityID, EnumCatTypes>> const& DemoController::GetCats(EntityID id)
		{
				m_cachedActiveCats.clear();
				if (!(m_scriptData[id].p_ratsMap)) { return m_cachedActiveCats; } // null check

				for (auto const& [catId, data] : *m_scriptData[id].p_catsMap)
				{
						try
						{
								// Check that the entity is active and rat is alive
								if (EntityManager::GetInstance().Get<EntityDescriptor>(catId).isActive)
								{
										m_cachedActiveCats.emplace_back(std::make_pair(catId, EnumCatTypes::MEOWSALOT));
								}
						}
						catch (...) { }
				}

				return m_cachedActiveCats;
		}

		
		unsigned int DemoController::RatCount(EntityID id)
		{
				if (!(m_scriptData[id].p_ratsMap)) { return 0; } // null check

				unsigned count{};

				for (auto const& [ratId, data] : *m_scriptData[id].p_ratsMap)
				{
						try
						{
								// Check that the entity is active and rat is alive
								if (EntityManager::GetInstance().Get<EntityDescriptor>(ratId).isActive)
								{
										++count;
								}
						}
						catch (...) { }
				}

				return count;
		}

		
		unsigned int DemoController::CatCount(EntityID id)
		{
				if (!(m_scriptData[id].p_catsMap)) { return 0; } // null check

				unsigned count{};

				for (auto const& [CatId, data] : *m_scriptData[id].p_catsMap)
				{
						try
						{
								// Check that the entity is active and rat is alive
								if (EntityManager::GetInstance().Get<EntityDescriptor>(CatId).isActive)
								{
										++count;
								}
						}
						catch (...) { }
				}

				return count;
		}



		// ----- PUBLIC FUNCTIONS ----- //

		void DemoController::Init(EntityID id)
		{
				// Look for all the rats in the scene
				m_scriptData[id].p_ratsMap = &(dynamic_cast<RatScript*>(LogicSystem::m_scriptContainer[GETSCRIPTNAME(RatScript)])->m_scriptData);
				m_scriptData[id].p_catsMap = &(dynamic_cast<CatScript*>(LogicSystem::m_scriptContainer[GETSCRIPTNAME(CatScript)])->m_scriptData);
		}


		void DemoController::Update(EntityID id, float deltaTime)
		{
				if (!ratsPrinted)
				{
						auto const& myVec{ GetRats(id) };
						for (auto const& [ratId, rat] : myVec)
						{
								std::cout << "Rat id: " << ratId << ", type: " << (int)rat << std::endl;
						}
						ratsPrinted = true;
				}

				if (!catsPrinted)
				{
						auto const& myVec{ GetCats(id) };
						for (auto const& [catId, cat] : myVec)
						{
								std::cout << "Cat id: " << catId << ", type: " << (int)cat << std::endl;
						}
						catsPrinted = true;
				}
		}


		void DemoController::OnAttach(EntityID id)
		{
				// Make this instance of the rat controller the main instance
				mainInstance = id;

				// Create script instance data
				m_scriptData[id] = DemoControllerData();

				// Reserve elements in container
				m_cachedActiveRats.reserve(20);
				m_cachedActiveCats.reserve(20);
		}


		void DemoController::OnDetach(EntityID id)
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
				m_cachedActiveCats.clear();

				ratsPrinted = false;
		}


		std::map<EntityID, DemoControllerData>& DemoController::GetScriptData()
		{
				return m_scriptData;
		}


		rttr::instance DemoController::GetScriptData(EntityID id)
		{
				return rttr::instance(m_scriptData.at(id));
		}

		void DemoController::ToggleEntity(EntityID id, bool setToActive)
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