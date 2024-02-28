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

#include "Layers/LayerManager.h"
#include "Logic/LogicSystem.h"
#include "Logic/FollowScript.h"
#include <cstdlib>   // For srand and rand
#include <ctime>     // For time


namespace PE
{

	CatController_v2_0::~CatController_v2_0()
	{
	}

	void CatController_v2_0::Init(EntityID id)
	{
		m_lostGame = false;
		UpdateCurrentCats(id);
		UpdateCachedCats(id);
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
		if (!m_lostGame)
			m_cachedCats = m_currentCats;
	}

	// getters
	void CatController_v2_0::UpdateDeployableCats(EntityID mainInstanceID)
	{
		// checks if it is an active cat
		auto IsCatAndNotCaged =
		[&](EntityID id)
		{
			if (IsCat(id))
			{
				if (!IsCatCaged(id))
					return true;
			}
			return false;
		};
		
		m_deployableCats.clear();
		m_deployableCats.emplace_back(EnumCatType::MAINCAT);
		for (EntityID catID : (GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers)
		{
			if (IsCatAndNotCaged(catID))
				m_deployableCats.emplace_back((GETSCRIPTDATA(CatScript_v2_0, catID))->catType);
		}
	}

	void CatController_v2_0::UpdateCurrentCats(EntityID)
	{
		m_currentCats.clear();
		for (const auto& layer : LayerView<ScriptComponent>())
		{
			for (EntityID catID : InternalView(layer))
			{
				auto const& r_scripts = EntityManager::GetInstance().Get<ScriptComponent>(catID).m_scriptKeys;
				//if (IsCat(catID))
				for (auto& [scriptname, state] : r_scripts)
				{
					if (scriptname == "CatScript_v2_0")
					{
						EnumCatType const& r_catType = *GETSCRIPTDATA(CatScript_v2_0, catID).catType;
						m_currentCats.emplace_back(std::pair{ catID, r_catType }); // saves all cats, including the caged ones
						if (r_catType == EnumCatType::MAINCAT)
							m_mainCatID = catID;
					}
				}
			}
		}
	}

	void CatController_v2_0::KillCat(EntityID id)
	{
		if (IsCat(id) && !IsCatCaged(id))
		{
			EntityID catToRemove = id;
		
			if (!((GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.empty()))
			{
				catToRemove = (GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.back();
				(GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.pop_back();
			}
			//else kill cat that has been hit
			(GETSCRIPTDATA(CatScript_v2_0, catToRemove))->toggleDeathAnimation = true;
			if ((GETSCRIPTDATA(CatScript_v2_0, catToRemove))->catType == EnumCatType::MAINCAT)
			{
				(GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.clear();
				m_lostGame = true;
			}
		}
	}

	void CatController_v2_0::RemoveCatFromCurrent(EntityID id)
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

	bool CatController_v2_0::IsFollowCat(EntityID catID)
	{
		FollowScriptData_v2_0* p_followScript = GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID);
		return (std::find(p_followScript->followers.begin(), p_followScript->followers.end(), catID) != p_followScript->followers.end());
	}
}