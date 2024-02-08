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
		std::cout << "RatReturn_v2_0::StateEnter()\n";
		p_data = GETSCRIPTDATA(RatScript_v2_0, id);
		gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		previousGameState = gameStateController->currentState;
		m_planningRunOnce = false;
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
							if (!m_planningRunOnce)
							{
									m_planningRunOnce = true;

									// Check if any of the conditions to change states has been met
									if (!(p_data->hasRatStateChanged))
										CheckIfShouldChangeStates();

									// Target our original position
									vec2 targetPosition{ p_data->originalPosition };
									GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, targetPosition);

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


	void RatReturn_v2_0::StateExit(EntityID)
	{
			DisableTelegraphs();
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
					std::cout << "RatIdle_v2_0::CheckIfShouldChangeStates(" << p_data->myID << "): cat in range\n";
					// there's a cat in the detection range, move to attack it
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToAttack(p_data->myID);
			}
			// Check if any cats exited the detection range in the last turn
			else if (!(p_data->catsExitedDetectionRadius.empty()))
			{
					std::cout << "RatIdle_v2_0::CheckIfShouldChangeStates(" << p_data->myID << "): cat exited range\n";
					// Check for the closest cat
					EntityID closestCat{ RatScript_v2_0::GetCloserTarget(RatScript_v2_0::GetEntityPosition(p_data->myID), p_data->catsExitedDetectionRadius) };

					// a cat just passed by us, hunt it down
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToHunt(p_data->myID, closestCat);
			}
			// Check if we're close enough to our original position
			else if (RatScript_v2_0::GetEntityPosition(p_data->myID).DistanceSquared(p_data->originalPosition) > 
					(p_data->minDistanceToTarget * p_data->minDistanceToTarget))
			{
					std::cout << "RatIdle_v2_0::CheckIfShouldChangeStates(" << p_data->myID << "): cat reached\n";
					// Stop moving back to our original position
					RatScript_v2_0::PositionEntity(p_data->myID, p_data->originalPosition);

					// a cat just passed by us, hunt it down
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToIdle(p_data->myID);
			}
	}

	void RatReturn_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
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


	void RatReturn_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
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