/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatController_v2_0.cpp
 \date     15-1-2024

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains declarations for functions used for a grey player cat's states.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"

#include "CatController_v2_0.h"
#include "ECS/SceneView.h"
#include "Logic/LogicSystem.h"



namespace PE
{

	CatController_v2_0::~CatController_v2_0()
	{
	}

	void CatController_v2_0::Init(EntityID id)
	{
		for (EntityID catID : SceneView<ScriptComponent>())
		{
			auto const& r_scripts = EntityManager::GetInstance().Get<ScriptComponent>(catID).m_scriptKeys;
			//if (IsCat(catID))
			for (auto &[scriptname,state] : r_scripts)
			{
				if (scriptname == "CatScript_v2_0")
				{
					//std::pair<EntityID, EnumCatType> pair{ catID, GREYCAT }; //*GETSCRIPTDATA(CatScript_v2_0, catID).catType };
					std::pair<EntityID, EnumCatType> pair{ catID, *GETSCRIPTDATA(CatScript_v2_0, catID).catType };	
					m_cacheCats.push_back(pair);
					m_currentCats.push_back(pair);
				}

			}
		}
	}

	void CatController_v2_0::Update(EntityID id, float deltaTime)
	{
		int countCat{ 0 };
		// gets the cats of the current frame
		for (EntityID catID : SceneView<ScriptComponent>())
		{
			auto const& r_scripts = EntityManager::GetInstance().Get<ScriptComponent>(catID).m_scriptKeys;
			//if (IsCat(catID))
			for (auto& [scriptname, state] : r_scripts)
			{
				countCat += r_scripts.count("CatScript_v2_0");
			}
		}

		if (countCat != m_currentCats.size())
		{
			m_currentCats.clear();
			for (EntityID catID : SceneView<ScriptComponent>())
			{
				auto const& r_scripts = EntityManager::GetInstance().Get<ScriptComponent>(catID).m_scriptKeys;
				//if (IsCat(catID))
				for (auto& [scriptname, state] : r_scripts)
				{
					if (scriptname == "CatScript_v2_0")
					{
						//std::pair<EntityID, EnumCatType> pair{ catID, GREYCAT }; //*GETSCRIPTDATA(CatScript_v2_0, catID).catType };
						std::pair<EntityID, EnumCatType> pair{ catID, *GETSCRIPTDATA(CatScript_v2_0, catID).catType };
						m_currentCats.push_back(pair);
					}

				}
			}
		}
	}

	void CatController_v2_0::OnAttach(EntityID id)
	{
		mainInstance = id;
		m_scriptData[id] = CatController_v2_0Data{};
	}

	void CatController_v2_0::OnDetach(EntityID id)
	{
		auto iter = m_scriptData.find(id);
		if (iter != m_scriptData.end())
		{
			m_scriptData.erase(id);
		}
		if (id == mainInstance) { mainInstance = 0; }

		m_currentCats.clear();
	}

	void CatController_v2_0::Destroy(EntityID id)
	{
		CacheCurrentCats();
	}

	// getters
	void CatController_v2_0::CacheCurrentCats()
	{
		m_cacheCats = m_currentCats;
	}

	int CatController_v2_0::GetCurrentMovementEnergy(EntityID catID)
	{
		return (GETSCRIPTDATA(CatScript_v2_0, catID))->catCurrentEnergy;
	}
	
	int CatController_v2_0::GetMaxMovementEnergy(EntityID catID)
	{
		return (GETSCRIPTDATA(CatScript_v2_0, catID))->catMaxMovementEnergy;
	}
	
	std::vector<std::pair<EntityID, EnumCatType>> CatController_v2_0::GetCachedCats(EntityID id)
	{
		return m_cacheCats;
	}
	
	std::vector<std::pair<EntityID, EnumCatType>> CatController_v2_0::GetCurrentCats(EntityID id)
	{
		return m_currentCats;
	}

}