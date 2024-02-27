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

namespace PE
{
	void BossRatExecuteState::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(BossRatScript, id);
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
	}


	void BossRatExecuteState::StateUpdate(EntityID id, float deltaTime)
	{
		// Can conduct state checks and call for state changes here
				// Can conduct state checks and call for state changes here
		if (p_gsc->currentState == GameStates_v2_0::EXECUTE)
		{
			p_data->p_stateManager->ChangeState(new BossRatPlanningState(), id);
		}
	}


	void BossRatExecuteState::StateExit(EntityID)
	{

	}

} // End of namespace PE