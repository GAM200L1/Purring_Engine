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
#include "RatReturnState_v2_0.h"
#include "../Events/EventHandler.h"
#include "../GameStateController_v2_0.h"

namespace PE
{
	void RatHunt_v2_0::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(RatScript_v2_0, id);
		gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		previousGameState = gameStateController->currentState;

		// Subscribe to events
		m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatHunt_v2_0::OnTriggerEnterAndStay, this);
		m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatHunt_v2_0::OnTriggerEnterAndStay, this);
		m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatHunt_v2_0::OnTriggerExit, this);

		// Store the current position as the position to return to later
		p_data->originalPosition = RatScript_v2_0::GetEntityPosition(id);

		// Store the ID of the cat being targeted and reset the hunting state
		SetNewTarget(targetId);
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
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToReturn(p_data->myID);
					break; // Exit the switch case
				}

				// Choose where to move
				if (StateJustChanged())
				{
					// ----- Picking where to move
					// Check if there are any cats in the detection range
					if (CheckCatsInDetectionRange())
					{
							// there's a cat in the detection range, move to attack it
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToReturn(id);
					}
					else if (EntityManager::GetInstance().IsEntityValid(id)) // Check if the current cat is alive // @TODO to replace when we have a method for checking cat's alive status
					{
							// the cat we're chasing is dead, return to the original position
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToReturn(id);
					}
					
					PickTargetPosition();

					// ---- Update telegraph
					// Rotate the telegraph to face the target
					// @TODO
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
			targetId = 0;
	}


	void RatHunt_v2_0::StateCleanUp()
	{
			// Unsubscribe from events
			REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);
	}


	void RatHunt_v2_0::SetNewTarget(EntityID const _targetId)
	{
			targetId = _targetId;

			// Reset number of hunting turns
			huntingTurnsLeft = p_data->maxHuntTurns;
	}


	void RatHunt_v2_0::PickTargetPosition()
	{
			// Check what cats have exited the collider
			vec2 ratPosition{ RatScript_v2_0::GetEntityPosition(p_data->myID) };
			vec2 finalTarget{ RatScript_v2_0::GetEntityPosition(targetId) };
			float targetDistance{ (ratPosition - finalTarget).LengthSquared() };

			// If the cat is farther than the rat's max dist
			if (targetDistance > (p_data->maxMovementRange * p_data->maxMovementRange))
			{
					// Try to find a waypoint that's closer
					vec2 closestTarget{ finalTarget }; float squaredDistance{ targetDistance };

					// Choose the two closest waypoints in the direction of the cat
					if (gameStateController->GetCurrentLevel() == 0)
					{
							// We're in level 1
							// Compare the distances of all the targets from the postion passed in
							for (vec2 const& targetPosition : waypointsLevel1)
							{
									float currentSquaredDistance{ (ratPosition - targetPosition).LengthSquared() };

									if ((currentSquaredDistance > p_data->minDistanceToTarget) && // Ensure that the player doesnt target a waypoint that it's already next to
											(currentSquaredDistance < squaredDistance))
									{
											closestTarget = targetPosition;
											squaredDistance = currentSquaredDistance;
									}
							}
					} 
					else
					{
							// We're in level 2
							// Compare the distances of all the waypoints from the postion passed in
							for (vec2 const& targetPosition : waypointsLevel2)
							{
									float currentSquaredDistance{ (ratPosition - targetPosition).LengthSquared() };

									if ((currentSquaredDistance > p_data->minDistanceToTarget) && // Ensure that the player doesnt target a waypoint that it's already next to
											(currentSquaredDistance < squaredDistance))
									{
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


	bool RatHunt_v2_0::CheckCatsInDetectionRange()
	{
			return !(p_data->catsInDetectionRadius.empty());
	}


	void RatHunt_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
	{
			if (!p_data) { return; }

			if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
			{
					OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
					// check if entity1 is the rat's detection collider and entity2 is cat
					if ((OTEE.Entity1 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity2) &&
							EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
					{
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity2);
					}
					// check if entity2 is the rat's detection collider and entity1 is cat
					else if ((OTEE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity1) &&
							EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
					{
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity1);
					}
			}
			else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
			{
					OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
					// check if entity1 is the rat's detection collider and entity2 is cat
					if ((OTSE.Entity1 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTSE.Entity2) &&
							EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity2).name.find("Cat") != std::string::npos)
					{
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTSE.Entity2);
					}
					// check if entity2 is the rat's detection collider and entity1 is cat
					else if ((OTSE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTSE.Entity1) &&
							EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity1).name.find("Cat") != std::string::npos)
					{
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTSE.Entity1);
					}
			}
	}


	void RatHunt_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
	{
			if (!p_data) { return; }

			OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
			// check if entity1 is the rat's detection collider and entity2 is cat
			if ((OTEE.Entity1 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity2) &&
					EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
			{
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity2);
			}
			// check if entity2 is the rat's detection collider and entity1 is cat
			else if ((OTEE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity1) &&
					EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
			{
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity1);
			}
	}
} // End of namespace PE