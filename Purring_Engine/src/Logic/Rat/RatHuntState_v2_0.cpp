/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatHuntState_v2_0.cpp
 \date     24-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions for the rat hunting state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "RatHuntState_v2_0.h"
#include "../GameStateController_v2_0.h"

namespace PE
{
	void RatHunt_v2_0::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(RatScript_v2_0, id);
		gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		previousGameState = gameStateController->currentState;

		huntingTurnsLeft = p_data->maxHuntTurns;
	}


	void RatHunt_v2_0::StateUpdate(EntityID id, float deltaTime)
	{
		if (gameStateController->currentState == GameStates_v2_0::PAUSE)
		{
				previousGameState = gameStateController->currentState;
				return;
		}

		// If the game state is execute, keep track of any cats that pass through the rat's radius
		if (gameStateController->currentState == GameStates_v2_0::EXECUTE)
		{
				// Update the number of turns left
				if (StateJustChanged())
				{
						--huntingTurnsLeft;
				}

				switch (p_data->ratType) 
				{
				case EnumRatType::GUTTER:
				case EnumRatType::BRAWLER:
				{
						// Move towards the target position until we've reached or we've run out of turns
						GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CalculateMovement(p_data->myID, deltaTime);
						break;
				}
				default: break;
				}
		}
		// If the game state is planning, choose the next position to move to
		else if(gameStateController->currentState == GameStates_v2_0::PLANNING)
		{
			switch (p_data->ratType)
			{
			case EnumRatType::GUTTER:
			case EnumRatType::BRAWLER:
			{
				// Check if we should switch to the returning state
				if (huntingTurnsLeft == 0)
				{
					--huntingTurnsLeft;
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->TriggerStateChange(p_data->myID, 0.f);
					break; // Exit the switch case
				}

				// Choose where to move
				if (StateJustChanged())
				{
						// ----- Picking where to move
						PickTargetPosition();

						// ---- Update telegraph
						// Rotate the telegraph to face the target

				}
				break;
			}
			default: break;
			}
		}

		// Store frame
		previousGameState = gameStateController->currentState;
	}


	void RatHunt_v2_0::StateExit(EntityID)
	{

	}


	void RatHunt_v2_0::PickTargetPosition()
	{
			// Check what cats have exited the collider
			EntityID closestCat{ RatScript_v2_0::GetCloserTarget(RatScript_v2_0::GetEntityPosition(p_data->myID), p_data->catsExitedDetectionRadius) };

			vec2 ratPosition{ RatScript_v2_0::GetEntityPosition(p_data->myID) };
			vec2 finalTarget{ RatScript_v2_0::GetEntityPosition(closestCat) };
			float targetDistance{ (ratPosition - finalTarget).LengthSquared() };

			// If the cat is farther than the rat's max dist
			if (targetDistance > (p_data->maxMovementRange * p_data->maxMovementRange))
			{
					vec2 closestTarget{ }; float squaredDistance{ };

					// Choose the two closest waypoints in the direction of the cat
					if (gameStateController->GetCurrentLevel() == 0)
					{
							// We're in level 1
							bool first{ true };

							// Compare the distances of all the targets from the postion passed in
							for (vec2 const& targetPosition : waypointsLevel1)
							{
									float currentSquaredDistance{ (ratPosition - targetPosition).LengthSquared() };

									if (first || currentSquaredDistance < squaredDistance)
									{
											first = false;
											closestTarget = targetPosition;
											squaredDistance = currentSquaredDistance;
									}
							}
					} 
					else
					{
							// We're in level 2
							bool first{ true };

							// Compare the distances of all the waypoints from the postion passed in
							for (vec2 const& targetPosition : waypointsLevel2)
							{
									float currentSquaredDistance{ (ratPosition - targetPosition).LengthSquared() };

									if (first || currentSquaredDistance < squaredDistance)
									{
											first = false;
											closestTarget = targetPosition;
											squaredDistance = currentSquaredDistance;
									}
							}
					}

					// If the waypoint is closer than the cat, then target the waypoint
					if (squaredDistance < targetDistance)
					{
							finalTarget = closestTarget;
					}
			}

			// Set the rat target
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, finalTarget);
	}

} // End of namespace PE