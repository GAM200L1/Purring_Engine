/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatHuntState_v2_0.h
 \date     24-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions for a temp rat state for testing purposes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "../StateManager.h"
#include "RatScript_v2_0.h"

namespace PE
{
	class RatHunt_v2_0 : public State
	{
		// ----- Constructors ----- //
	public:
		virtual ~RatHunt_v2_0() override { p_data = nullptr; }

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
		virtual std::string_view GetName() override { return "RatHuntState_v2_0"; }

	private:
	  RatScript_v2_0_Data* p_data; // pointer to script instance data
		GameStateController_v2_0* gameStateController{ nullptr }; // pointer to the game state controller
	};
} // End of namespace PE
