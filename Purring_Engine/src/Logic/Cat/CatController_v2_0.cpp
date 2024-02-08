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
#include "CatHelperFunctions.h"

#include "ECS/SceneView.h"
#include "Logic/LogicSystem.h"
#include "Logic/FollowScript.h"



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
					EnumCatType const& r_catType = *GETSCRIPTDATA(CatScript_v2_0, catID).catType;
					m_currentCats.emplace_back(std::pair{ catID, r_catType }); // saves all cats, including the caged ones
					//if (GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetCurrentLevel() == 0 && r_catType == EnumCatType::MAINCAT) // if in the first level, only take main cat
					//{
					//	m_cacheCats.emplace_back(std::pair{ catID, r_catType });
					//	m_currentCats.emplace_back(std::pair{ catID, r_catType });
					//	//m_scriptData[id].mainCatID = catID;
					//	return;
					//}
					//else
					//{
					//	std::pair<EntityID, EnumCatType> pair{ catID, r_catType };
					//	m_cacheCats.push_back(pair);
					//	m_currentCats.push_back(pair);
					//}
				}
			}
		}
	}

	void CatController_v2_0::Update(EntityID id, float deltaTime)
	{
		//GameStateController_v2_0* p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		//// if at cat chain level
		//if (p_gsc->GetCurrentLevel() == 0)
		//{
		//	/*FollowScriptData const& r_mainCatFollowScript = *GETSCRIPTDATA(FollowScript, m_scriptData[id].mainCatID);
		//	for (EntityID followingCatID : r_mainCatFollowScript.FollowingObject)
		//	{
		//		bool alreadySaved{ false };
		//		for (auto const& [catID, type] : m_currentCats)
		//		{
		//			if (followingCatID == catID)
		//			{
		//				alreadySaved = true;
		//				break;
		//			}
		//		}
		//		if (!alreadySaved)
		//			m_currentCats.emplace_back(std::pair{ followingCatID, *GETSCRIPTDATA(CatScript_v2_0, followingCatID).catType });
		//	}*/
		//}
		//else if (p_gsc->GetCurrentLevel() == 1)
		//{
		//	//m_currentCats.clear();
		//	//for (EntityID catID : SceneView<ScriptComponent>())
		//	//{
		//	//	auto const& r_scripts = EntityManager::GetInstance().Get<ScriptComponent>(catID).m_scriptKeys;
		//	//	//if (IsCat(catID))
		//	//	for (auto& [scriptname, state] : r_scripts)
		//	//	{
		//	//		if (scriptname == "CatScript_v2_0")
		//	//		{
		//	//			EnumCatType const& r_catType = *GETSCRIPTDATA(CatScript_v2_0, catID).catType;
		//	//			std::pair<EntityID, EnumCatType> pair{ catID, r_catType };
		//	//			m_currentCats.push_back(pair);
		//	//		}
		//	//	}
		//	//}
		//}
		//	//size_t countCat{ 0 };
		//	//// gets the cats of the current frame
		//	//for (EntityID catID : SceneView<ScriptComponent>())
		//	//{
		//	//	auto const& r_scripts = EntityManager::GetInstance().Get<ScriptComponent>(catID).m_scriptKeys;
		//	//	//if (IsCat(catID))
		//	//	for (auto& [scriptname, state] : r_scripts)
		//	//	{
		//	//		countCat += r_scripts.count("CatScript_v2_0");
		//	//	}
		//	//}

		//	//if (countCat != m_currentCats.size())
		//	//{
		//	//	m_currentCats.clear();
		//	//	for (EntityID catID : SceneView<ScriptComponent>())
		//	//	{
		//	//		auto const& r_scripts = EntityManager::GetInstance().Get<ScriptComponent>(catID).m_scriptKeys;
		//	//		//if (IsCat(catID))
		//	//		for (auto& [scriptname, state] : r_scripts)
		//	//		{
		//	//			if (scriptname == "CatScript_v2_0")
		//	//			{
		//	//				//std::pair<EntityID, EnumCatType> pair{ catID, GREYCAT }; //*GETSCRIPTDATA(CatScript_v2_0, catID).catType };
		//	//				std::pair<EntityID, EnumCatType> pair{ catID, *GETSCRIPTDATA(CatScript_v2_0, catID).catType };
		//	//				m_currentCats.push_back(pair);
		//	//			}

		//	//		}
		//	//	}
		//	//}
		//	p_gsc = nullptr;
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
		//if (id == mainInstance) { mainInstance = 0; }

		m_currentCats.clear();
	}

	void CatController_v2_0::Destroy(EntityID id)
	{
		m_cachedCats = m_currentCats;
	}

	// getters
	std::vector<EnumCatType> CatController_v2_0::GetDeployableCats()
	{
		std::vector<EnumCatType> deployableCats{};
		for (auto const& [catID, type] : m_cachedCats)
		{
			// if cat is alive when caching
			if (!(GETSCRIPTDATA(CatScript_v2_0, catID))->isCaged)
				deployableCats.emplace_back(type);
		}
		return deployableCats;
	}

	void CatController_v2_0::KillCat(EntityID id)
	{
		(GETSCRIPTDATA(CatScript_v2_0, id))->toggleDeathAnimation = true;
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