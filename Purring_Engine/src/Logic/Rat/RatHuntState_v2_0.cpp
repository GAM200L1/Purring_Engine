/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatHuntState_v2_0.cpp
 \date     24-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions for the rat's hunting state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "RatHuntState_v2_0.h"
#include "RatReturnState_v2_0.h"
#include "../Events/EventHandler.h"
#include "../GameStateController_v2_0.h"
#include "Layers/LayerManager.h"

//#define DEBUG_PRINT

namespace PE
{
	void RatHunt_v2_0::StateEnter(EntityID id)
	{
#ifdef DEBUG_PRINT
			//std::cout << "RatHunt_v2_0::StateEnter(" << id << ")" << std::endl;
#endif // DEBUG_PRINT
		p_data = GETSCRIPTDATA(RatScript_v2_0, id);
		gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		m_previousGameState = gameStateController->currentState;
		m_planningRunOnce = false;

		// Subscribe to events
		m_collisionEnterEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionEnter, RatHunt_v2_0::OnCollisionEnter, this);
		m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionExit, RatHunt_v2_0::OnCollisionExit, this);
		m_triggerEnterEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatHunt_v2_0::OnTriggerEnterAndStay, this);
		m_triggerStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatHunt_v2_0::OnTriggerEnterAndStay, this);
		m_triggerExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatHunt_v2_0::OnTriggerExit, this);

		// Store the ID of the cat being targeted and reset the hunting state
		SetHuntTarget(targetId);
	}


	void RatHunt_v2_0::StateUpdate(EntityID id, float deltaTime)
	{
		if (gameStateController->currentState == GameStates_v2_0::PAUSE)
		{
				m_previousGameState = gameStateController->currentState;
				return;
		}

		// If the game state is execute, keep track of any cats that pass through the rat's radius
		if (gameStateController->currentState == GameStates_v2_0::EXECUTE)
		{
				m_planningRunOnce = false;

				// Update the number of turns left
				if (StateJustChanged() && m_previousGameState != GameStates_v2_0::PAUSE)
				{
						--huntingTurnsLeft;
						GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DisableMovementTelegraphs(id);

						// Play animation
						GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::WALK);

#ifdef DEBUG_PRINT
						std::cout << "RatHunt_v2_0::StateUpdate(" << p_data->myID << "): in execute\n";
#endif // DEBUG_PRINT
				}

				switch (p_data->ratType) 
				{
				case EnumRatType::GUTTER:
				case EnumRatType::BRAWLER:
				case EnumRatType::SNIPER:
				{
						// Move towards the target position until we've reached or we've run out of turns
						if (!(p_data->finishedExecution)) {
								p_data->finishedExecution = GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CalculateMovement(p_data->myID, deltaTime);
#ifdef DEBUG_PRINT
								std::cout << "RatHunt_v2_0::StateUpdate(" << p_data->myID << "): in execute - moving\n";
#endif // DEBUG_PRINT
						}
						else 
						{
								// Play animation
								GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::IDLE);
#ifdef DEBUG_PRINT
								std::cout << "RatHunt_v2_0::StateUpdate(" << p_data->myID << "): in execute - finished moving\n";
#endif // DEBUG_PRINT
						}
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
			case EnumRatType::SNIPER:
			{
				// Choose where to move when the planning state has just started
				if (!m_planningRunOnce)
				{
#ifdef DEBUG_PRINT
						std::cout << "RatHunt_v2_0::StateUpdate(" << p_data->myID << "): in planning\n";
#endif // DEBUG_PRINT
					m_planningRunOnce = true;
					p_data->finishedExecution = false;

					// Play animation
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::IDLE);

					// Check if any of the conditions to change states has been met
					if (!(p_data->hasRatStateChanged))
						CheckIfShouldChangeStates();
					
					// Continue hunting
					vec2 targetPosition{ PickTargetPosition() };

					// ---- Update telegraph
					// Rotate the telegraph to face the target
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->EnableMovementTelegraphs(id, targetPosition);
				}

				break; // end of BRAWLER rat type
			}
			default: break;
			}
		}

		// Store frame
		m_previousGameState = gameStateController->currentState;
	}


	void RatHunt_v2_0::StateExit(EntityID id)
	{
			// empty
	}


	void RatHunt_v2_0::StateCleanUp()
	{
			p_data = nullptr;
			gameStateController = nullptr;

			// Unsubscribe from events
			REMOVE_KEY_COLLISION_LISTENER(m_collisionEnterEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_triggerEnterEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_triggerStayEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_triggerExitEventListener);
	}


	void RatHunt_v2_0::SetHuntTarget(EntityID const _targetId)
	{
			targetId = _targetId;

			// Reset number of hunting turns
			huntingTurnsLeft = p_data->maxHuntTurns;
	}


	vec2 RatHunt_v2_0::PickTargetPosition()
	{
		vec2 ratPosition{ RatScript_v2_0::GetEntityPosition(p_data->myID) };
		vec2 finalTarget{ RatScript_v2_0::GetEntityPosition(targetId) };

		//bool hitSomething{};
		//bool continueLoop{ true };
		//int loopTimer = 360 / 15;


		//while (continueLoop)
		//{
		//	hitSomething = false;
		//	// Check what cats have exited the collider

		//	std::cout<<"help aaaaaaaaaaa: " << finalTarget.x << " " << finalTarget.y << std::endl;
		//	float targetDistance{ (ratPosition - finalTarget).LengthSquared() }; //magnitude

		//	vec2 targetNormalized = (ratPosition - finalTarget) / targetDistance;

		//	float range = std::min(p_data->maxMovementRange, targetDistance);

		//	vec2 endPoint = ratPosition + (targetNormalized * range);

		//	LineSegment ls(ratPosition, endPoint);

		//	for (const auto& layer : LayerView<Collider>())
		//	{
		//		for (EntityID objectID : InternalView(layer))
		//		{
		//			if (objectID == p_data->myID)
		//				continue;

		//			if (EntityManager::GetInstance().Has<EntityDescriptor>(objectID))
		//			{
		//				if (EntityManager::GetInstance().Get<EntityDescriptor>(objectID).name == "Obstacle")
		//				{
		//					if (DoRayCast(ls, objectID).has_value())
		//					{	
		//						std::cout <<"I am: "<<p_data->myID << " I Have Hit Entity: " << objectID << std::endl;
		//						hitSomething = true;
		//					}
		//				}
		//			}
		//		}
		//	}
		//	if (!hitSomething)
		//	{
		//		GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, finalTarget, true);
		//		return finalTarget;
		//	}
		//	--loopTimer;

		//	if (loopTimer == 0)
		//	{
		//		GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, finalTarget, true);
		//		return finalTarget;
		//	}

		//	finalTarget = RotatePoint(ratPosition, finalTarget, 15);
		//}
		// 
		// Set the rat target
		GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, finalTarget, true);
		return finalTarget;
	}

	vec2 RatHunt_v2_0::RotatePoint(vec2 center, vec2 point, float m)
	{
		float angleInRadians = ConvertDegToRad(m);
		vec2 centerToPoint{point - center};

		return Rotate(centerToPoint, angleInRadians) + center;
	}

	void RatHunt_v2_0::CheckIfShouldChangeStates()
	{
      // Clear dead cats from the collision sets
			RatScript_v2_0::ClearDeadCats(p_data->catsInDetectionRadius);
			RatScript_v2_0::ClearDeadCats(p_data->catsExitedDetectionRadius);

			// ----- Picking where to move
			// Check if there are any cats in the detection range
			if (!(p_data->catsInDetectionRadius.empty()))
			{
					// there's a cat in the detection range, move to attack it
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToMovement(p_data->myID);
			}
			// Check if any cats exited the detection range in the last turn
			else if (!(p_data->catsExitedDetectionRadius.empty()))
			{
					// cats just passed by us, hunt the closest one down
					SetHuntTarget( RatScript_v2_0::GetCloserTarget(RatScript_v2_0::GetEntityPosition(p_data->myID), p_data->catsExitedDetectionRadius) );
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ClearCollisionContainers(p_data->myID);
			}
			// Check if the current cat is alive 
			else if (!RatScript_v2_0::GetIsNonCagedCat(targetId))
			{
					// the cat we're chasing is dead, return to the original position
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToReturn(p_data->myID);
			}
			// Check if we should switch to the returning state
			else if (huntingTurnsLeft == 0)
			{
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToReturn(p_data->myID);
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->EnableDetectionTelegraphs(p_data->myID, EnumRatIconAnimations::CONFUSED);
			}
			else 
			{
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ClearCollisionContainers(p_data->myID);
			}
	}

	void RatHunt_v2_0::OnCollisionEnter(const Event<CollisionEvents>& r_event)
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
	}

	void RatHunt_v2_0::OnCollisionExit(const Event<CollisionEvents>& r_event)
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

	void RatHunt_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
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


	void RatHunt_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
	{
			if (!p_data) { return; }
			else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

			OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
			// check if a cat has exited the rat's detection collider
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckDetectionTriggerExited(p_data->myID, OTEE.Entity1, OTEE.Entity2);
	}
} // End of namespace PE