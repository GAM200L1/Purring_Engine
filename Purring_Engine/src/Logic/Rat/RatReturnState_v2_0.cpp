/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatReturnState_v2_0.cpp
 \date     24-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions for a temp rat state for testing purposes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "RatReturnState_v2_0.h"

namespace PE
{
	void RatReturn_v2_0::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(RatScript_v2_0, id);
		gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		previousGameState = gameStateController->currentState;
	}


	void RatReturn_v2_0::StateUpdate(EntityID id, float deltaTime)
	{
			if (gameStateController->currentState == GameStates_v2_0::PAUSE)
			{
					previousGameState = gameStateController->currentState;
					return;
			}

			// If the game state is execute, keep track of any cats that pass through the rat's radius
			if (gameStateController->currentState == GameStates_v2_0::EXECUTE)
			{
					// Move the rat back towards its original position
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CalculateMovement(p_data->myID, deltaTime);
			}
			// If the game state is planning, set the target 
			else if (gameStateController->currentState == GameStates_v2_0::PLANNING)
			{
					switch (p_data->ratType)
					{
					case EnumRatType::GUTTER:
					case EnumRatType::BRAWLER:
					{
							// Move back to the original position
							if (StateJustChanged())
							{
									GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, p_data->originalPosition);

									// ---- Update telegraph
									// Rotate the telegraph to face the target

							}
							break;
					}
					default: break;
					}
			}

			// Store the current game state
			previousGameState = gameStateController->currentState;
	}


	void RatReturn_v2_0::StateExit(EntityID)
	{

	}

} // End of namespace PE