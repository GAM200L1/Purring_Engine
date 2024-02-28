/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatExecuteState.h
\date     25 - 02 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Declaration for Boss Rat Execute State

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/

#pragma once
#include "Logic/StateManager.h"
#include "BossRatScript.h"
#include <string_view>
#include "Logic/GameStateController_v2_0.h"

namespace PE
{
	class BossRatExecuteState : public State
	{
		// ----- Destructor ----- //
	public:
		virtual ~BossRatExecuteState() override { p_data = nullptr; }

		// ----- Public Functions ----- //
	public:
		/*!***********************************************************************************
			\brief Play the idle animation and start the timer

			\param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;

		/*!***********************************************************************************
			\brief Checks if its state should change

			\param[in,out] id - ID of instance of script
			\param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;

		/*!***********************************************************************************
			\brief does nothing
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;

		/*!***********************************************************************************
			\brief Get the name of the state

			\return std::string_view name of state
		*************************************************************************************/
		virtual std::string_view GetName() override { return "BossRatExecuteState"; }

	private:
		BossRatScriptData* p_data; // pointer to script instance data
		GameStateController_v2_0* p_gsc; // pointer to game state controller
	};
} // End of namespace PE
