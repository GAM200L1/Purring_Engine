/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatReturnState_v2_0.cpp
 \date     24-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions for the rat's return state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "RatReturnState_v2_0.h"

//#define DEBUG_PRINT

namespace PE
{
	void RatReturn_v2_0::StateEnter(EntityID id)
	{
#ifdef DEBUG_PRINT
		std::cout << "RatReturn_v2_0::StateEnter()\n";
#endif // DEBUG_PRINT 
		p_data = GETSCRIPTDATA(RatScript_v2_0, id);
		gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		previousGameState = gameStateController->currentState;
		m_planningRunOnce = false;

		m_collisionEnterEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionEnter, RatReturn_v2_0::OnCollisionEnter, this);
		m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatReturn_v2_0::OnTriggerEnterAndStay, this);
		m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatReturn_v2_0::OnTriggerEnterAndStay, this);
		m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatReturn_v2_0::OnTriggerExit, this);
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
					m_planningRunOnce = false;

					// Disable telegraphs when the execution phase starts
					if (StateJustChanged())
					{
							DisableTelegraphs();
					}

					// Move the rat back towards its original position
					if(!(p_data->finishedExecution))
						p_data->finishedExecution = GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CalculateMovement(p_data->myID, deltaTime);
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
							if (!m_planningRunOnce)
							{
									m_planningRunOnce = true;
									p_data->finishedExecution = false;

									// Check if any of the conditions to change states has been met
									if (!(p_data->hasRatStateChanged))
										CheckIfShouldChangeStates();

									// Target our original position
									vec2 targetPosition{ p_data->originalPosition };
									GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, targetPosition, true);

									// ---- Update telegraph
									// Rotate the telegraph to face the target
									EnableTelegraphs(targetPosition);
							}
							break;
					}
					default: break;
					}
			}

			// Store the current game state
			previousGameState = gameStateController->currentState;
	}


	void RatReturn_v2_0::StateCleanUp()
	{
			DisableTelegraphs();
			p_data = nullptr;
			gameStateController = nullptr;

			// Unsubscribe from events
			REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_collisionEnterEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);
	}


	void RatReturn_v2_0::StateExit(EntityID)
	{
			// empty
	}


	void RatReturn_v2_0::EnableTelegraphs(vec2 const& targetPosition)
	{
			vec2 directionOfTarget{ targetPosition - RatScript_v2_0::GetEntityPosition(p_data->myID) };
			float orientation = atan2(directionOfTarget.y, directionOfTarget.x);

			// Rotate the telegraph
			RatScript_v2_0::RotateEntityRelative(p_data->pivotEntityID, orientation);
			RatScript_v2_0::ToggleEntity(p_data->pivotEntityID, true); // enable the telegraph parent
			RatScript_v2_0::ToggleEntity(p_data->telegraphArrowEntityID, true); // enable the telegraph
	}


	void RatReturn_v2_0::DisableTelegraphs()
	{
			RatScript_v2_0::ToggleEntity(p_data->telegraphArrowEntityID, false); // disable the telegraph
	}


	void RatReturn_v2_0::CheckIfShouldChangeStates()
	{
			// ----- Picking where to move
			// Check if there are any cats in the detection range
			if (!(p_data->catsInDetectionRadius.empty()))
			{
#ifdef DEBUG_PRINT
					std::cout << "RatReturn_v2_0::CheckIfShouldChangeStates(" << p_data->myID << "): cat in range\n";
#endif // DEBUG_PRINT
					// there's a cat in the detection range, move to attack it
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToAttack(p_data->myID);
			}
			// Check if any cats exited the detection range in the last turn
			else if (!(p_data->catsExitedDetectionRadius.empty()))
			{
#ifdef DEBUG_PRINT
					std::cout << "RatReturn_v2_0::CheckIfShouldChangeStates(" << p_data->myID << "): cat exited range\n";
#endif // DEBUG_PRINT
					// Check for the closest cat
					EntityID closestCat{ RatScript_v2_0::GetCloserTarget(RatScript_v2_0::GetEntityPosition(p_data->myID), p_data->catsExitedDetectionRadius) };

					// a cat just passed by us, hunt it down
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToHunt(p_data->myID, closestCat);
			}
			// Check if we're close enough to our original position
			else if (RatScript_v2_0::GetEntityPosition(p_data->myID).DistanceSquared(p_data->originalPosition) < 
					(p_data->minDistanceToTarget * p_data->minDistanceToTarget))
			{
#ifdef DEBUG_PRINT
					std::cout << "RatReturn_v2_0::CheckIfShouldChangeStates(" << p_data->myID << "): cat reached\n";
#endif // DEBUG_PRINT
					// Stop moving back to our original position
					RatScript_v2_0::PositionEntity(p_data->myID, p_data->originalPosition);

					// a cat just passed by us, hunt it down
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToIdle(p_data->myID);
			}
			else
			{
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ClearCollisionContainers(p_data->myID);
			}
	}


	void RatReturn_v2_0::OnCollisionEnter(const Event<CollisionEvents>& r_event)
	{
			if (!p_data) { return; }
			else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

			if (r_event.GetType() == CollisionEvents::OnCollisionEnter)
			{
					OnCollisionEnterEvent OCEE = dynamic_cast<OnCollisionEnterEvent const&>(r_event);
					// check if rat and cat have collided
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckRatTouchingCat(p_data->myID, OCEE.Entity1, OCEE.Entity2);
			}
	}

	
	void RatReturn_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
	{
			if (!p_data) { return; }
			else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

			if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
			{
					OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
					// check if a cat has entered the rat's detection collider
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckDetectionTriggerEntered(p_data->myID, OTEE.Entity1, OTEE.Entity2);
			}
			else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
			{
					OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
					// check if a cat has entered the rat's detection collider
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckDetectionTriggerEntered(p_data->myID, OTSE.Entity1, OTSE.Entity2);
			}
	}


	void RatReturn_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
	{
			if (!p_data) { return; }
			else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

			OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
			// check if a cat has exited the rat's detection collider
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckDetectionTriggerExited(p_data->myID, OTEE.Entity1, OTEE.Entity2);
	}

} // End of namespace PE