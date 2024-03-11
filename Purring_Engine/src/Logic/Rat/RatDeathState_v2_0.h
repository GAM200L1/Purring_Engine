/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatDeathState_v2_0.h
 \date     06-03-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions for the rat's death state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include "../StateManager.h"
#include "RatScript_v2_0.h"

namespace PE
{
	class RatDeathState_v2_0 : public State
	{
		// ----- Destructor ----- //
	public:
		virtual ~RatDeathState_v2_0() override { p_data = nullptr; }

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
		virtual std::string_view GetName() override { return "RatDeathState_v2_0"; }

	private:
		RatScript_v2_0_Data *p_data; // pointer to script instance data
		GameStateController_v2_0* p_gameStateController; // pointer to script instance data
		float m_animDurationLeft{}; // Duration of the death animation left before the rat should be deactivated
	};
} // End of namespace PE
