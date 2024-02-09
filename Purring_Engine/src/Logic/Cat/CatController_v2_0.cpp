/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
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
#include "CatHelperFunctions.h"
#include "FollowScript_v2_0.h"

#include "ECS/SceneView.h"
#include "Logic/LogicSystem.h"
#include "Logic/FollowScript.h"
#include <cstdlib>   // For srand and rand
#include <ctime>     // For time


namespace PE
{
	EntityID CatController_v2_0::mainInstance{};

	CatController_v2_0::~CatController_v2_0()
	{
	}

	void CatController_v2_0::Init(EntityID id)
	{
		m_lostGame = false;
		for (EntityID catID : SceneView<ScriptComponent>())
		{
			auto const& r_scripts = EntityManager::GetInstance().Get<ScriptComponent>(catID).m_scriptKeys;
			//if (IsCat(catID))
			for (auto &[scriptname,state] : r_scripts)
			{
				if (scriptname == "CatScript_v2_0")
				{
					EnumCatType const& r_catType = *GETSCRIPTDATA(CatScript_v2_0, catID).catType;
					m_currentCats.emplace_back(std::pair{ catID, r_catType }); // saves all cats, including the caged ones
					if (r_catType == EnumCatType::MAINCAT)
						m_mainCatID = catID;
					else if (GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetCurrentLevel() == 0)
						(GETSCRIPTDATA(CatScript_v2_0, catID))->finishedExecution = true;
				}
			}
		}
	}

	void CatController_v2_0::Update(EntityID id, float deltaTime)
	{
	}

	void CatController_v2_0::OnAttach(EntityID id)
	{
		mainInstance = id;
		m_scriptData[id] = CatController_v2_0Data{};
		m_currentCats.clear();
	}

	void CatController_v2_0::OnDetach(EntityID id)
	{
		auto iter = m_scriptData.find(id);
		if (iter != m_scriptData.end())
		{
			m_scriptData.erase(id);
		}
	}

	void CatController_v2_0::Destroy(EntityID id)
	{
		if (id != mainInstance) { return; }
		if (!m_lostGame)
			m_cachedCats = m_currentCats;
	}

	// getters
	std::vector<EnumCatType> CatController_v2_0::GetDeployableCats()
	{
		std::vector<EnumCatType> deployableCats{};
		for (auto const& [catID, type] : m_cachedCats)
		{
			// if cat is alive when caching
			if ((GETSCRIPTDATA(CatScript_v2_0, catID))->isCaged)
				continue;
			deployableCats.emplace_back(type);
		}
		return deployableCats;
	}

	void CatController_v2_0::KillCat(EntityID id)
	{
		if (IsCat(id) && !IsCatCaged(id))
		{
			EntityID catToRemove = id;
			if (GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetCurrentLevel() == 0)
			{
				// if in cat chain stage, kill the last cat in the chain
				if (!(GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.empty())
				{
					catToRemove = *(GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.end();
					(GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.pop_back();
				}
			}
			//else kill cat that has been hit
			(GETSCRIPTDATA(CatScript_v2_0, catToRemove))->toggleDeathAnimation = true;
			if ((GETSCRIPTDATA(CatScript_v2_0, catToRemove))->catType == EnumCatType::MAINCAT)
				m_lostGame = true;
		}
	}

	void CatController_v2_0::RemoveCatFromVector(EntityID id)
	{
		EnumCatType const& r_catType = (GETSCRIPTDATA(CatScript_v2_0, id))->catType;
		m_currentCats.erase(std::find(m_currentCats.begin(), m_currentCats.end(), std::pair{id, r_catType}));
		CatHelperFunctions::ToggleEntity(id, false);
	}

	int CatController_v2_0::GetCurrentMovementEnergy(EntityID catID)
	{
		return (GETSCRIPTDATA(CatScript_v2_0, catID))->catCurrentEnergy;
	}
	
	int CatController_v2_0::GetMaxMovementEnergy(EntityID catID)
	{
		return (GETSCRIPTDATA(CatScript_v2_0, catID))->catMaxMovementEnergy;
	}
	
	std::vector<std::pair<EntityID, EnumCatType>> CatController_v2_0::GetCurrentCats(EntityID id)
	{
		return m_currentCats;
	}

}