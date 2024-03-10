/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatPlanningState.cpp
\date     25 - 02 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Definitioon for Boss Rat Execute State

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#include "prpch.h"
#include "BossRatPlanningState.h"
#include "BossRatExecuteState.h"
#include "Logic/LogicSystem.h"
#include "Logic/Boss/BossRatAttacks/BossRatBashAttack.h"
#include "Logic/Boss/BossRatAttacks/BossRatSlamAttack.h"
#include "Logic/Boss/BossRatAttacks/BossRatChargeAttack.h"

namespace PE
{
	void BossRatPlanningState::StateEnter(EntityID id)
	{
		p_script = GETSCRIPTINSTANCEPOINTER(BossRatScript);
		p_data = GETSCRIPTDATA(BossRatScript, id);
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

		DecideAttack();
		p_script->FindAllObstacles();
		if(p_data->p_currentAttack)
		p_data->p_currentAttack->DrawTelegraphs(id);
		PoisonPuddleUpdate();
	}


	void BossRatPlanningState::StateUpdate(EntityID id, float deltaTime)
	{
		// Can conduct state checks and call for state changes here
		if (p_gsc->currentState == GameStates_v2_0::EXECUTE)
		{
			p_data->finishExecution = false;
			p_data->p_stateManager->ChangeState(new BossRatExecuteState(),id);
		}
	}


	void BossRatPlanningState::StateExit(EntityID)
	{
		if (p_script->currentSlamTurnCounter > 0)
		--p_script->currentSlamTurnCounter;
	}

	void BossRatPlanningState::DecideAttack()
	{
		if (p_script->currentSlamTurnCounter > 0 && p_data->p_currentAttack)
			return;

		if (p_data->p_currentAttack)
			delete p_data->p_currentAttack;

		if (p_data->currentAttackInSet == 3)
		{
			p_data->currentAttackInSet = 0;
			p_data-> currentAttackSet = rand() % 3;
		}

		switch (p_script->bossRatAttackSets[p_data->currentAttackSet][p_data->currentAttackInSet])
		{
		case BossRatAttacks::BASH:
			p_data->p_currentAttack = new BossRatBashAttack(p_script->FindClosestCat());
			p_script->currentSlamTurnCounter = 0;
			break;
		case BossRatAttacks::SLAM:
			p_data->p_currentAttack = new BossRatSlamAttack();
			p_script->currentSlamTurnCounter = 3;
			break;
		case BossRatAttacks::CHARGE:
			p_data->p_currentAttack = new BossRatChargeAttack(p_script->FindFurthestCat());
			p_script->currentSlamTurnCounter = 0;
			break;
		}

		p_data->currentAttackInSet++;
	}

	void BossRatPlanningState::PoisonPuddleUpdate()
	{
		for (auto& [id, timer] : p_script->poisonPuddles)
		{
			--timer;
		}

		for (auto it = p_script->poisonPuddles.cbegin(); it != p_script->poisonPuddles.cend() /* not hoisted */; /* no increment */)
		{
			if (it->second <= 0)
			{
				EntityManager::GetInstance().RemoveEntity(it->first);
				p_script->poisonPuddles.erase(it++);    // or "it = m.erase(it)" since C++11
			}
			else
			{
				++it;
			}
		}
	}

} // End of namespace PE