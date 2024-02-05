/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatScript_v2_0.cpp
 \date     17-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains definitions for functions used for a rats mechanics

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "RatScript_v2_0.h"
#include "RatTempSTATE_v2_0.h"

#include "../Physics/RigidBody.h"
#include "../Physics/Colliders.h"
#include "../Data/SerializationManager.h"
#include "../Hierarchy/HierarchyManager.h"

#include "../Rat/RatIdle_v2_0.h"
#include "../Rat/RatMovement_v2_0.h"
#include "../Rat/RatHuntState_v2_0.h"
#include "../Rat/RatReturnState_v2_0.h"


namespace PE
{

		// ---------- FUNCTION DEFINITIONS ---------- //

		void RatScript_v2_0::Init(EntityID id)
		{
			gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
			previousGameState = gameStateController->currentState;

			CreateCheckStateManager(id);

			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }

			// Clear collision containers
			it->second.catsInDetectionRadius.clear();
			it->second.catsExitedDetectionRadius.clear();

			// Create a detection radius and configure
			it->second.detectionRadiusId = CreateDetectionRadius(it->second);
		}


		void RatScript_v2_0::Update(EntityID id, float deltaTime)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }

			if(gameStateController->currentState == GameStates_v2_0::PAUSE){
					previousGameState = gameStateController->currentState;
					return;
			}

			CreateCheckStateManager(id);
			it->second.p_stateManager->Update(id, deltaTime);

			if (StateJustChanged() && gameStateController->currentState == GameStates_v2_0::EXECUTE)
			{
				// Clear cat collision containers
				it->second.catsInDetectionRadius.clear();
				it->second.catsExitedDetectionRadius.clear();
			}

			previousGameState = gameStateController->currentState;
		}


		void RatScript_v2_0::OnAttach(EntityID id)
		{
			// Add dynamic rigidbody
			if (!EntityManager::GetInstance().Has<RigidBody>(id))
			{
					EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<RigidBody>() });
					EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
			}
			else
			{
					EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
			}
			 
			// Add circle collider
			if (!EntityManager::GetInstance().Has<Collider>(id))
			{
					EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<Collider>() });
					EntityManager::GetInstance().Get<Collider>(id).colliderVariant = CircleCollider();
			}
			else
			{
					EntityManager::GetInstance().Get<Collider>(id).colliderVariant = CircleCollider();
			}

			// Create script instance data
			m_scriptData[id] = RatScript_v2_0_Data();
			m_scriptData[id].myID = id;
		}


		void RatScript_v2_0::OnDetach(EntityID id)
		{
			// Delete this instance's script data
			auto it = m_scriptData.find(id);
			if (it != m_scriptData.end())
			{
					m_scriptData.erase(id);
			}
		}

		void RatScript_v2_0::ToggleEntity(EntityID id, bool setToActive)
		{
			// Exit if the entity is not valid
			if (!EntityManager::GetInstance().IsEntityValid(id)) { return; }

			// Toggle the entity
			EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = setToActive;
		}

		void RatScript_v2_0::PositionEntity(EntityID const transformId, vec2 const& r_position)
		{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				r_transform.position = r_position;
			}
			catch (...) { return; }
		}

		void RatScript_v2_0::ScaleEntity(EntityID const transformId, float const width, float const height)
		{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				r_transform.width = width;
				r_transform.height = height;
			}
			catch (...) { return; }
		}

		vec2 RatScript_v2_0::GetEntityPosition(EntityID const transformId)
		{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				return r_transform.position;
			}
			catch (...) { return vec2{}; }
		}

		vec2 RatScript_v2_0::GetEntityScale(EntityID const transformId)
		{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				return vec2{ r_transform.width, r_transform.height };
			}
			catch (...) { return vec2{}; }
		}

		std::map<EntityID, RatScript_v2_0_Data>& RatScript_v2_0::GetScriptData()
		{
			return m_scriptData;
		}


		rttr::instance RatScript_v2_0::GetScriptData(EntityID id)
		{
			return rttr::instance(m_scriptData.at(id));
		}

		void RatScript_v2_0::TriggerStateChange(EntityID id, float const stateChangeDelay)
		{
			if (m_scriptData[id].delaySet) { return; }

			m_scriptData[id].shouldChangeState = true;
			m_scriptData[id].timeBeforeChangingState = stateChangeDelay;
			m_scriptData[id].delaySet = true;
		}



		// ------------ CAT DETECTION ------------ // 

		void RatScript_v2_0::CatEntered(EntityID const id, EntityID const catID)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }

			// Store the cat in the container
			it->second.catsInDetectionRadius.emplace(catID);

			// Remove the cats in the enter container from the exit container
			it->second.catsExitedDetectionRadius.erase(catID);
		}


		void RatScript_v2_0::CatExited(EntityID const id, EntityID const catID)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }

			// Store the cat in the container
			it->second.catsExitedDetectionRadius.emplace(catID);

			// Remove the cats in the exit container from the enter container
			it->second.catsInDetectionRadius.erase(catID);
		}




		// ------------ MOVEMENT HELPER FUNCTIONS ------------ //


		// Loops through all the targets in the container passed in and returns the ID of the target closest to the position passed in
		// Tie breaking: whichever comes last in the container
		// Returns zero if the container passed in is empty
		EntityID RatScript_v2_0::GetCloserTarget(vec2 position, std::set<EntityID> const& potentialTargets)
		{
				if (potentialTargets.empty()) { return 0; }

				bool first{ true };
				EntityID closestTarget{ }; float squaredDistance{ };

				// Compare the distances of all the targets from the postion passed in
				for (EntityID const& id : potentialTargets)
				{
						float currentSquaredDistance{ (position - GetEntityPosition(id)).LengthSquared() };

						if (first || currentSquaredDistance < squaredDistance)
						{
								first = false;
								closestTarget = id;
								squaredDistance = currentSquaredDistance;
						}
				}

				return closestTarget;
		}


		// Loops through all the targets in the container passed in and returns the closest position passed in
		// Tie breaking: whichever comes last in the container
		// Returns zero if the container passed in is empty
		vec2 RatScript_v2_0::GetCloserTarget(vec2 position, std::vector<vec2> const& potentialTargets)
		{
				if (potentialTargets.empty()) { return position; }

				bool first{ true };
				vec2 closestTarget{ }; float squaredDistance{ };

				// Compare the distances of all the targets from the postion passed in
				for (vec2 const& targetPosition : potentialTargets)
				{
						float currentSquaredDistance{ (position - targetPosition).LengthSquared() };

						if (first || currentSquaredDistance < squaredDistance)
						{
								first = false;
								closestTarget = targetPosition;
								squaredDistance = currentSquaredDistance;
						}
				}

				return closestTarget;
		}



		void RatScript_v2_0::SetTarget(EntityID id, EntityID targetId)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }

				it->second.targetedCat = targetId;
				vec2 targetPosition = RatScript_v2_0::GetEntityPosition(it->second.targetedCat);
				vec2 ratPosition = RatScript_v2_0::GetEntityPosition(id);

				// Calculate the distance and direction from the rat to the player cat
				it->second.ratPlayerDistance = (targetPosition - ratPosition).Length();
				it->second.directionFromRatToPlayerCat = (targetPosition - ratPosition).GetNormalized();
		}


		void RatScript_v2_0::SetTarget(EntityID id, vec2 const& r_targetPosition)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }

				vec2 ratPosition = RatScript_v2_0::GetEntityPosition(id);

				// Calculate the distance and direction from the rat to the player cat
				it->second.ratPlayerDistance = (r_targetPosition - ratPosition).Length();
				it->second.directionFromRatToPlayerCat = (r_targetPosition - ratPosition).GetNormalized();
		}


		bool RatScript_v2_0::CalculateMovement(EntityID id, float deltaTime)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return false; }

				if (it->second.ratPlayerDistance > 0.f)
				{
						vec2 newPosition = RatScript_v2_0::GetEntityPosition(id) + (it->second.directionFromRatToPlayerCat * it->second.movementSpeed * deltaTime);
						RatScript_v2_0::PositionEntity(id, newPosition);
						it->second.ratPlayerDistance -= it->second.movementSpeed * deltaTime;

						//std::cout << "RatMovement_v2_0::CalculateMovement - Rat ID: " << id
						//		<< " moved to new position: (" << newPosition.x << ", " << newPosition.y
						//		<< "), Remaining distance: " << it->second.ratPlayerDistance << std::endl;

						return CheckDestinationReached(it->second.minDistanceToTarget, newPosition, RatScript_v2_0::GetEntityPosition(it->second.mainCatID));
				}
				else
				{
						//std::cout << "RatMovement_v2_0::CalculateMovement - Rat ID: " << id << " has no movement or already at destination." << std::endl;
						return false;
				}
		}


		bool RatScript_v2_0::CheckDestinationReached(float const minDistanceToTarget, const vec2& newPosition, const vec2& targetPosition)
		{
				bool reached = (newPosition - targetPosition).Length() <= minDistanceToTarget;
				//std::cout << "RatMovement_v2_0::CheckDestinationReached - Destination " << (reached ? "reached." : "not reached.") << std::endl;
				return reached;
		}


		void RatScript_v2_0::CreateCheckStateManager(EntityID id)
		{
			if (m_scriptData.count(id))
			{
				if (m_scriptData.at(id).p_stateManager)
						return;

				m_scriptData[id].p_stateManager = new StateMachine{};
				m_scriptData[id].p_stateManager->ChangeState(new RatIdle_v2_0(RatType::PATROL), id);
			}
		}

		EntityID RatScript_v2_0::CreateDetectionRadius(RatScript_v2_0_Data const& r_data)
		{
			SerializationManager serializationManager;
			EntityID radiusId{ serializationManager.LoadFromFile("RatDetectionRadius_Prefab.json") };
			Hierarchy::GetInstance().AttachChild(r_data.myID, radiusId);

			// Print parent child relationships
			if (EntityManager::GetInstance().Has<EntityDescriptor>(radiusId))
			{
					EntityDescriptor& radiusIdDesc{ EntityManager::GetInstance().Get<EntityDescriptor>(radiusId) };
					EntityDescriptor& myDesc{ EntityManager::GetInstance().Get<EntityDescriptor>(r_data.myID) };
					std::cout << "RatScript_v2_0::CreateDetectionRadius(): parent: " << (radiusIdDesc.parent.has_value() ? radiusIdDesc.parent.value() : 2567) << ", " << *myDesc.children.cbegin() << "\n";
			}

		//	//GETSCRIPTINSTANCEPOINTER(RatDetectionScript_v2_0)->SetParentRat(radiusId, r_data.myID);
		//	//GETSCRIPTINSTANCEPOINTER(RatDetectionScript_v2_0)->SetDetectionRadius(radiusId, r_data.detectionRadius);

		  // Add dynamic rigidbody
			if (!EntityManager::GetInstance().Has<RigidBody>(radiusId))
			{
					EntityFactory::GetInstance().Assign(radiusId, { EntityManager::GetInstance().GetComponentID<RigidBody>() });
			}

			// Configure rigidbody
			if(EntityManager::GetInstance().Has<RigidBody>(radiusId))
			{
					RigidBody& detectionRigidbody{ EntityManager::GetInstance().Get<RigidBody>(radiusId) };
					detectionRigidbody.SetType(EnumRigidBodyType::DYNAMIC);
			}

			// Add circle collider
			if (!EntityManager::GetInstance().Has<Collider>(radiusId))
			{
					EntityFactory::GetInstance().Assign(radiusId, { EntityManager::GetInstance().GetComponentID<Collider>() });
			}

			// Configure collider
			if(EntityManager::GetInstance().Has<Collider>(radiusId))
			{
					Collider& detectionCollider{ EntityManager::GetInstance().Get<Collider>(radiusId) };
					detectionCollider.colliderVariant = CircleCollider();
					detectionCollider.isTrigger = true;
					detectionCollider.collisionLayerIndex = detectionColliderLayer; // @TODO To configure the collision matrix of the game scene

					CircleCollider& circleCollider{ std::get<CircleCollider>(detectionCollider.colliderVariant) };
					circleCollider.scaleOffset = r_data.detectionRadius;
			}

			return radiusId;
		}

} // End of namespace PE