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

		// Decides which attack to use
		//will add the other attacks later, fixed on bash for now
		if (p_data->p_currentAttack)
			delete p_data->p_currentAttack;

		if (p_data->currentAttackInSet == 3)
		{
			p_data->currentAttackInSet = 0;
			p_data-> currentAttackSet = rand() % 3;
		}

		switch (p_script->BossRatAttackSets[p_data->currentAttackSet][p_data->currentAttackInSet])
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

} // End of namespace PE