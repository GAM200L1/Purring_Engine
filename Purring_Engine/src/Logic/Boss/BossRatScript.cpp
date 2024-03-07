/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatScript.cpp
\date     24 - 02 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Definition for BossRatScript class

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#include "prpch.h"
#include "BossRatScript.h"
#include "Logic/LogicSystem.h"
#include "BossRatExecuteState.h"
#include "BossRatPlanningState.h"
#include "Logic/Cat/CatController_v2_0.h"
#include "Layers/LayerManager.h"
namespace PE
{
	// ---------- FUNCTION DEFINITIONS ---------- //

	BossRatScript::~BossRatScript()
	{
	}

	void BossRatScript::Init(EntityID id)
	{
		CreateCheckStateManager(id);
		FindAllObstacles();
	}


	void BossRatScript::Update(EntityID id, float deltaTime)
	{
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		if (p_gsc->currentState == GameStates_v2_0::PAUSE) 
		{
			return;
		}

		if (p_gsc->currentState == GameStates_v2_0::WIN || p_gsc->currentState == GameStates_v2_0::LOSE)
		{
			//might want to do something else from pause
			return;
		}

		if (m_scriptData[id].health <= 0)
		{
			//do something
		}

		CreateCheckStateManager(id);

		if(p_gsc->currentState == GameStates_v2_0::PLANNING || p_gsc->currentState == GameStates_v2_0::EXECUTE)
		m_scriptData[id].p_stateManager->Update(id, deltaTime);
	}


	void BossRatScript::OnAttach(EntityID id)
	{
		m_scriptData[id] = BossRatScriptData();
		currentBoss = id;
	}


	void BossRatScript::OnDetach(EntityID id)
	{
		// Delete this instance's script data
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			m_scriptData.erase(id);
		}
	}


	std::map<EntityID, BossRatScriptData>& BossRatScript::GetScriptData()
	{
		return m_scriptData;
	}


	rttr::instance BossRatScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

	void BossRatScript::CreateCheckStateManager(EntityID id)
	{
		if (m_scriptData.count(id))
		{
			if (m_scriptData.at(id).p_stateManager)
				return;

			m_scriptData[id].p_stateManager = new StateMachine{};
			m_scriptData[id].p_stateManager->ChangeState(new BossRatPlanningState{}, id);
		}
	}

	void BossRatScript::FindAllObstacles()
	{
		m_Obstacles.clear();
		for (const auto& layer : LayerView<EntityDescriptor, Collider>(true))
		{
			for (const EntityID& id : InternalView(layer))
			{
				std::string nameTest = EntityManager::GetInstance().Get<EntityDescriptor>(id).name;
				if(EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Obstacle") != std::string::npos)
				m_Obstacles.push_back(id);
			}
		}
	}

	void BossRatScript::TakeDamage(int damage)
	{
		--m_scriptData[currentBoss].health;
	}

	EntityID BossRatScript::FindFurthestCat()
	{
		CatController_v2_0* CatManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
		EntityID FurthestCat{};
		float FurthestDistance{};
		
		for (auto [CatID, CatType] : CatManager->GetCurrentCats(CatManager->mainInstance))
		{
			Transform cattransform = EntityManager::GetInstance().Get<Transform>(CatID);

			if (cattransform.position.Distance(EntityManager::GetInstance().Get<Transform>(currentBoss).position) > FurthestDistance)
			{
				FurthestDistance = cattransform.position.Distance(EntityManager::GetInstance().Get<Transform>(currentBoss).position);
				FurthestCat = CatID;
			}

		}

		//std::cout << "Furthest Cat: " << EntityManager::GetInstance().Get<EntityDescriptor>(FurthestCat).name << "\n";

		return FurthestCat;
	}
	std::vector<EntityID> BossRatScript::GetAllObstacles()
	{
		return m_Obstacles;
	}
} // End of namespace PE