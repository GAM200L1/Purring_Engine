/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatExecuteState.cpp
\date     25 - 02 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Definitioon for Boss Rat Execute State

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#include "prpch.h"
#include "BossRatExecuteState.h"
#include "Logic/LogicSystem.h"
#include "Logic/GameStateController_v2_0.h"
#include "BossRatPlanningState.h"
#include "Logic/Cat/CatController_v2_0.h"

namespace PE
{
	void BossRatExecuteState::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(BossRatScript, id);
		p_script = GETSCRIPTINSTANCEPOINTER(BossRatScript);
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

		if (p_data->p_currentAttack)
		p_data->p_currentAttack->EnterAttack(id);
	}


	void BossRatExecuteState::StateUpdate(EntityID id, float deltaTime)
	{
		if (p_data->p_currentAttack)
		p_data->p_currentAttack->UpdateAttack(id,deltaTime);

		PoisonPuddleDamageUpdate();

		if (p_gsc->currentState == GameStates_v2_0::PLANNING)
		{
			p_data->p_stateManager->ChangeState(new BossRatPlanningState(), id);
		}
	}


	void BossRatExecuteState::StateExit(EntityID id)
	{
		if (p_data->p_currentAttack)
		p_data->p_currentAttack->ExitAttack(id);
	}

	void BossRatExecuteState::PoisonPuddleDamageUpdate()
	{
		CatController_v2_0* p_catController = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);

		for (auto [CatID, CatType] : p_catController->GetCurrentCats(p_catController->mainInstance))
		{
			for (auto& [id,timer] : p_script->poisonPuddles)
			{
				if (timer == 0)
					continue;
				
				Transform catTransform = EntityManager::GetInstance().Get<Transform>(CatID);
				vec2 difference = catTransform.position - EntityManager::GetInstance().Get<Transform>(id).position;
				float DistanceFromPuddle = EntityManager::GetInstance().Get<Transform>(id).position.Distance(catTransform.position);
				//check distance based on the vec2 differences
				if (difference.y >= 0)
				{
					if (DistanceFromPuddle  < EntityManager::GetInstance().Get<Transform>(id).width / 2)
					{
						GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->KillCat(CatID);
					}
				}
				//x is negative y positive
				else if (difference.y <= 0)
				{
					if (DistanceFromPuddle + catTransform.width / 2 < EntityManager::GetInstance().Get<Transform>(id).width / 2)
					{
						GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->KillCat(CatID);
					}
				}
			}
		}
	}

} // End of namespace PE