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
namespace PE
{
	void BossRatPlanningState::StateEnter(EntityID id)
	{
		p_script = GETSCRIPTINSTANCEPOINTER(BossRatScript);
		p_data = GETSCRIPTDATA(BossRatScript, id);
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

		DecideAttack();
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

	}

	void BossRatPlanningState::DecideAttack()
	{
		// Decides which attack to use
		//will add the other attacks later, fixed on bash for now
		p_data->p_currentAttack = new BossRatBashAttack(p_script->FindFurthestCat());
	}

} // End of namespace PE