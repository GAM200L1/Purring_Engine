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
		std::cout << "RatReturn_v2_0::StateEnter(" << id << ")\n";
#endif // DEBUG_PRINT 
		p_data = GETSCRIPTDATA(RatScript_v2_0, id);
		gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		previousGameState = gameStateController->currentState;
		m_planningRunOnce = false;

		m_collisionEnterEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionEnter, RatReturn_v2_0::OnCollisionEnterOrStay, this);
		m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionStay, RatReturn_v2_0::OnCollisionEnterOrStay, this);
		m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionExit, RatReturn_v2_0::OnCollisionExit, this);

		m_triggerEnterEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatReturn_v2_0::OnTriggerEnterAndStay, this);
		m_triggerStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatReturn_v2_0::OnTriggerEnterAndStay, this);
		m_triggerExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatReturn_v2_0::OnTriggerExit, this);
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
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ClearCollisionContainers(p_data->myID);
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DisableMovementTelegraphs(p_data->myID);

							// Play animation
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::WALK);
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
					case EnumRatType::SNIPER:
					{
							// Move back to the original position
							if (!m_planningRunOnce)
							{
									m_planningRunOnce = true;
									p_data->finishedExecution = false;

									// Play idle animation
									GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::IDLE);

									// Check if any of the conditions to change states has been met
									if (!(p_data->hasRatStateChanged))
										CheckIfShouldChangeStates();

									// Target our original position
									vec2 targetPosition{ p_data->originalPosition };
									GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, targetPosition, true);

									// ---- Update telegraph
									// Rotate the telegraph to face the target
									GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->EnableMovementTelegraphs(p_data->myID, targetPosition);
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
			p_data = nullptr;
			gameStateController = nullptr;

			// Unsubscribe from events
			REMOVE_KEY_COLLISION_LISTENER(m_collisionEnterEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);

			REMOVE_KEY_COLLISION_LISTENER(m_triggerEnterEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_triggerStayEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_triggerExitEventListener);
	}


	void RatReturn_v2_0::StateExit(EntityID)
	{
			// empty
	}


	void RatReturn_v2_0::CheckIfShouldChangeStates()
	{
			// Clear dead cats from the collision sets
			RatScript_v2_0::ClearDeadCats(p_data->catsInDetectionRadius);
			RatScript_v2_0::ClearDeadCats(p_data->catsExitedDetectionRadius);

			// ----- Picking where to move
			// Check if there are any cats in the detection range
			if (!(p_data->catsInDetectionRadius.empty()))
			{
					// there's a cat in the detection range, move to attack it
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToAttack(p_data->myID);
			}
			// Check if any cats exited the detection range in the last turn
			else if (!(p_data->catsExitedDetectionRadius.empty()))
			{
					// Check for the closest cat
					EntityID closestCat{ RatScript_v2_0::GetCloserTarget(RatScript_v2_0::GetEntityPosition(p_data->myID), p_data->catsExitedDetectionRadius) };

					// a cat just passed by us, hunt it down
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToHunt(p_data->myID, closestCat);
			}
			// Check if we're close enough to our original position
			else if (RatScript_v2_0::GetEntityPosition(p_data->myID).DistanceSquared(p_data->originalPosition) < 
					(p_data->minDistanceToTarget * p_data->minDistanceToTarget))
			{
					// Stop moving back to our original position
					//RatScript_v2_0::PositionEntity(p_data->myID, p_data->originalPosition);

					// a cat just passed by us, hunt it down
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToIdle(p_data->myID);
			}
			else
			{
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ClearCollisionContainers(p_data->myID);
			}
	}


	void RatReturn_v2_0::OnCollisionEnterOrStay(const Event<CollisionEvents>& r_event)
	{
			if (!p_data) { return; }
			else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

			if (r_event.GetType() == CollisionEvents::OnCollisionEnter)
			{
					OnCollisionEnterEvent OCEE = dynamic_cast<OnCollisionEnterEvent const&>(r_event);
					// check if rat and cat have collided
					bool ratCollidedWithCat{ GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckRatTouchingCat(p_data->myID, OCEE.Entity1, OCEE.Entity2) };

					// Check if the rat has been colliding with a wall
					if (!ratCollidedWithCat &&
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckRatTouchingWall(p_data->myID, OCEE.Entity1, OCEE.Entity2) &&
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->GetExecutionPhaseTimeout(p_data->myID))
					{
							// The rat has been touching the wall for too long
							p_data->finishedExecution = true;
							p_data->ratPlayerDistance = 0.f;
					}
			}
			else if (r_event.GetType() == CollisionEvents::OnCollisionStay)
			{
					OnCollisionStayEvent OCSE = dynamic_cast<OnCollisionStayEvent const&>(r_event);

					// Check if the rat has been colliding with a wall
					if (GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckRatTouchingWall(p_data->myID, OCSE.Entity1, OCSE.Entity2) &&
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->GetExecutionPhaseTimeout(p_data->myID))
					{
							// The rat has been touching the wall for too long
							p_data->finishedExecution = true;
							p_data->ratPlayerDistance = 0.f;
					}
			}
	}


	void RatReturn_v2_0::OnCollisionExit(const Event<CollisionEvents>& r_event)
	{
			if (!p_data) { return; }
			else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

			if (r_event.GetType() == CollisionEvents::OnCollisionExit)
			{
					OnCollisionExitEvent OCEE = dynamic_cast<OnCollisionExitEvent const&>(r_event);
					// check if rat has stopped colliding with wall
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckRatStopTouchingWall(p_data->myID, OCEE.Entity1, OCEE.Entity2);
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