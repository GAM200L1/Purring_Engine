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
		m_previousGameState = gameStateController->currentState;
		m_planningRunOnce = false;

		// Subscribe to events
		m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatHunt_v2_0::OnTriggerEnterAndStay, this);
		m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatHunt_v2_0::OnTriggerEnterAndStay, this);
		m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatHunt_v2_0::OnTriggerExit, this);

		// Store the current position as the position to return to later
		p_data->originalPosition = RatScript_v2_0::GetEntityPosition(id);

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
						DisableTelegraphs();
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
				// Choose where to move when the planning state has just started
				if (!m_planningRunOnce)
				{
					m_planningRunOnce = true;

					// Check if any of the conditions to change states has been met
					if (!(p_data->hasRatStateChanged))
						CheckIfShouldChangeStates();
					
					// Continue hunting
					vec2 targetPosition{ PickTargetPosition() };

					// ---- Update telegraph
					// Rotate the telegraph to face the target
					EnableTelegraphs(targetPosition);
				}

				break; // end of BRAWLER rat type
			}
			default: break;
			}
		}

		// Store frame
		m_previousGameState = gameStateController->currentState;
	}


	void RatHunt_v2_0::StateExit(EntityID)
	{
			targetId = 0;
			DisableTelegraphs();
	}


	void RatHunt_v2_0::StateCleanUp()
	{
			// Unsubscribe from events
			REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);
	}


	void RatHunt_v2_0::EnableTelegraphs(vec2 const& targetPosition)
	{
			vec2 directionOfTarget{ targetPosition - RatScript_v2_0::GetEntityPosition(p_data->myID) };
			float orientation = atan2(directionOfTarget.y, directionOfTarget.x);

			// Rotate the telegraph
			RatScript_v2_0::RotateEntityRelative(p_data->pivotEntityID, orientation);
			RatScript_v2_0::ToggleEntity(p_data->pivotEntityID, true); // enable the telegraph parent
			RatScript_v2_0::ToggleEntity(p_data->telegraphArrowEntityID, true); // enable the telegraph
	}
	

	void RatHunt_v2_0::DisableTelegraphs()
	{
			RatScript_v2_0::ToggleEntity(p_data->telegraphArrowEntityID, false); // disable the telegraph
	}


	void RatHunt_v2_0::SetHuntTarget(EntityID const _targetId)
	{
			targetId = _targetId;

			// Reset number of hunting turns
			huntingTurnsLeft = p_data->maxHuntTurns;
	}


	vec2 RatHunt_v2_0::PickTargetPosition()
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
			return finalTarget;
	}


	void RatHunt_v2_0::CheckIfShouldChangeStates()
	{
			// Check if we should switch to the returning state
			if (huntingTurnsLeft == 0)
			{
					std::cout << "RatHunt_v2_0::CheckIfShouldChangeStates(" << p_data->myID << "): huntingTurnsLeft == 0\n";
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToReturn(p_data->myID);
			}
			// ----- Picking where to move
			// Check if there are any cats in the detection range
			else if (!(p_data->catsInDetectionRadius.empty()))
			{
					std::cout << "RatHunt_v2_0::CheckIfShouldChangeStates(" << p_data->myID << "): cats in range\n";
					// there's a cat in the detection range, move to attack it
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToAttack(p_data->myID);
			}
			// Check if any cats exited the detection range in the last turn
			else if (!(p_data->catsExitedDetectionRadius.empty()))
			{
					std::cout << "RatHunt_v2_0::CheckIfShouldChangeStates(" << p_data->myID << "): cats exited range\n";
					// cats just passed by us, hunt the closest one down
					SetHuntTarget( RatScript_v2_0::GetCloserTarget(RatScript_v2_0::GetEntityPosition(p_data->myID), p_data->catsExitedDetectionRadius) );
			}
			// Check if the current cat is alive // @TODO to replace when we have a method for checking cat's alive status
			else if (!EntityManager::GetInstance().IsEntityValid(p_data->myID))
			{
					std::cout << "RatHunt_v2_0::CheckIfShouldChangeStates(" << p_data->myID << "): cat is dead\n";
					// the cat we're chasing is dead, return to the original position
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToReturn(p_data->myID);
			}
	}


	void RatHunt_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
	{
			if (!p_data) { return; }
			else if (gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

			if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
			{
					OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
					// check if entity1 is the rat's detection collider and entity2 is cat
					if ((OTEE.Entity1 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity2) &&
							EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
					{
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity2);

							std::cout << "RatHunt_v2_0::OnTriggerEnter() catsInDetectionRadius: ";
							for (auto const& catId : p_data->catsInDetectionRadius)
							{
									std::cout << catId << ' ';
							}
							std::cout << std::endl;
					}
					// check if entity2 is the rat's detection collider and entity1 is cat
					else if ((OTEE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity1) &&
							EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
					{
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity1);

							std::cout << "RatHunt_v2_0::OnTriggerEnter() catsInDetectionRadius: ";
							for (auto const& catId : p_data->catsInDetectionRadius)
							{
									std::cout << catId << ' ';
							}
							std::cout << std::endl;
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
							std::cout << "RatHunt_v2_0::OnTriggerStay() catsInDetectionRadius: ";
							for (auto const& catId : p_data->catsInDetectionRadius)
							{
									std::cout << catId << ' ';
							}
							std::cout << std::endl;
					}
					// check if entity2 is the rat's detection collider and entity1 is cat
					else if ((OTSE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTSE.Entity1) &&
							EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity1).name.find("Cat") != std::string::npos)
					{
							GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTSE.Entity1);
							std::cout << "RatHunt_v2_0::OnTriggerStay() catsInDetectionRadius: ";
							for (auto const& catId : p_data->catsInDetectionRadius)
							{
									std::cout << catId << ' ';
							}
							std::cout << std::endl;
					}
			}
	}


	void RatHunt_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
	{
			if (!p_data) { return; }
			else if (gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

			OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
			// check if entity1 is the rat's detection collider and entity2 is cat
			if ((OTEE.Entity1 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity2) &&
					EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
			{
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity2);
					std::cout << "RatHunt_v2_0::OnTriggerExit() catsExitedDetectionRadius: ";
					for (auto const& catId : p_data->catsExitedDetectionRadius)
					{
							std::cout << catId << ' ';
					}
					std::cout << std::endl;
			}
			// check if entity2 is the rat's detection collider and entity1 is cat
			else if ((OTEE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity1) &&
					EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
			{
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity1);
					std::cout << "RatHunt_v2_0::OnTriggerExit() catsExitedDetectionRadius: ";
					for (auto const& catId : p_data->catsExitedDetectionRadius)
					{
							std::cout << catId << ' ';
					}
					std::cout << std::endl;
			}
	}
} // End of namespace PE