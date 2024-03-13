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

#include "../Physics/RigidBody.h"
#include "../Physics/Colliders.h"
#include "../Data/SerializationManager.h"
#include "../Hierarchy/HierarchyManager.h"
#include "../ResourceManager/ResourceManager.h"

#include "../Rat/RatIdle_v2_0.h"
#include "../Rat/RatMovement_v2_0.h"
#include "../Rat/RatHuntState_v2_0.h"
#include "../Rat/RatReturnState_v2_0.h"
#include "../Rat/RatAttack_v2_0.h"
#include "../Rat/RatDeathState_v2_0.h"

#include "GutterRatAttack_v2_0.h"
#include "SniperRatAttack_v2_0.h"

#include "../Cat/CatController_v2_0.h"
#include "AudioManager/GlobalMusicManager.h"

#include "../Time/TimeManager.h" // GetDeltaTime()

//#define DEBUG_PRINT

namespace PE
{

		// ---------- FUNCTION DEFINITIONS ---------- //

		std::string RatScript_v2_0_Data::GetRatName(EnumRatType ratType)
		{
				switch (ratType)
				{
				case EnumRatType::GUTTER_V1:
				case EnumRatType::GUTTER: { return std::string{ "Gutter Rat" }; }
				case EnumRatType::BRAWLER: { return std::string{ "Brawler Rat" }; }
				case EnumRatType::SNIPER: { return std::string{ "Sniper Rat" }; }
				default: { return std::string{ "Rat" }; }
				}
		}

		std::string RatScript_v2_0_Data::GetRatName(char ratTypeIndex)
		{
				return GetRatName(static_cast<EnumRatType>(ratTypeIndex));
		}

		void RatScript_v2_0::Init(EntityID id)
		{
			gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
			previousGameState = gameStateController->currentState;

			CreateCheckStateManager(id);

			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }

			// Initialize some variables
			it->second.isAlive = true;
			it->second.hasChangedToDeathState = false;
			it->second.ratHealth = it->second.ratMaxHealth;
			it->second.timeBeforeChangingState = 0.f;

			// Clear collision containers
			ClearCollisionContainers(id);

			// Create a detection radius and configure
			it->second.detectionRadiusId = CreateDetectionRadius(it->second);
			CreateRatPathTelegraph(it->second);

			// Create the corresponding attack data object
			InstantiateRatAttack(it->second);

			// Store the position to return to
			it->second.originalPosition = GetEntityPosition(id);

			// Store the animation component
			m_scriptData[id].p_ratAnimationComponent = &EntityManager::GetInstance().Get<AnimationComponent>(id);
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

			if (GetIsPlayingHurtAnim(id) && GetHasAnimEnded(id))
			{
					PlayAnimation(id, it->second.cachedRatAnimation);
					it->second.cachedRatAnimation = EnumRatAnimations::RAT_ANIM_COUNT;
			}

			// If we just changed to the execution state
			if(GameStateJustChanged() && gameStateController->prevState != GameStates_v2_0::PAUSE 
				&& gameStateController->currentState == GameStates_v2_0::EXECUTE)
			{				
					// Clear cat collision containers
					//ClearCollisionContainers(id);
					// NOTE: I'm still not too sure about whether it's a good idea to clear this here. 
					// I'm making the assumption that all decisions the rat makes occurs during the planning phase
					// Update: It was not a good idea. Keeping this here for reference

					// Reset timeout
					it->second.totalTimeCollidingIntoWall = 0.f;
			}

			previousGameState = gameStateController->currentState;
			it->second.hasRatStateChanged = false;

			if (CheckShouldStateChange(id, deltaTime))
			{
					if (it->second.isAlive || !(it->second.hasChangedToDeathState))
						ChangeRatState(id);
			}
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
					it->second.DeleteAttackData();
					m_scriptData.erase(id);
			}
		}

		void RatScript_v2_0::ToggleEntity(EntityID id, bool setToActive)
		{
			// Exit if the entity is not valid
			if (id == 0 || !EntityManager::GetInstance().IsEntityValid(id)) { return; }

			// Toggle the entity
			if(EntityManager::GetInstance().Has<EntityDescriptor>(id))
				EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = setToActive;
		}

		void RatScript_v2_0::PositionEntity(EntityID const transformId, vec2 const& r_position)
		{
			if (transformId != 0 || EntityManager::GetInstance().Has<Transform>(transformId))
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				r_transform.position = r_position;
			}
		}

		void RatScript_v2_0::PositionEntityRelative(EntityID const transformId, vec2 const& r_position)
		{
			if (transformId != 0 || EntityManager::GetInstance().Has<Transform>(transformId))
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				r_transform.relPosition = r_position;
			}
		}

		void RatScript_v2_0::ScaleEntity(EntityID const transformId, float const width, float const height)
		{
			if (transformId != 0 || EntityManager::GetInstance().Has<Transform>(transformId))
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				r_transform.width = width;
				r_transform.height = height;
			}
		}

		void RatScript_v2_0::RotateEntity(EntityID const transformId, float const orientation)
		{
			if (transformId != 0 || EntityManager::GetInstance().Has<Transform>(transformId))
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				r_transform.orientation = orientation;
			}
		}

		void RatScript_v2_0::RotateEntityRelative(EntityID const transformId, float const orientation)
		{
			if (transformId != 0 || EntityManager::GetInstance().Has<Transform>(transformId))
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				r_transform.relOrientation = orientation;
			}
		}

		bool RatScript_v2_0::GetIsPlayingHurtAnim(EntityID const id)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return false; }
				return (it->second.p_ratAnimationComponent->GetAnimationID() == it->second.animationStates.at("Hurt"));
		}

		bool RatScript_v2_0::GetHasAnimEnded(EntityID const id)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return false; }
				return (it->second.p_ratAnimationComponent && it->second.p_ratAnimationComponent->HasAnimationEnded());
		}

		void RatScript_v2_0::PlayAnimation(EntityID const id, EnumRatAnimations const animationState, bool const forceRestart)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }

				// If the rat hasn't died, check if we are trying to play the hurt animation
				if (EnumRatAnimations::DEATH != animationState && EnumRatAnimations::HURT != animationState &&
						GetIsPlayingHurtAnim(id) && !GetHasAnimEnded(id))
				{
						// Don't let the new animation interrupt the hurt animation
						it->second.cachedRatAnimation = animationState;
						return;
				}

				// Get the name of the animation state
				std::string animationStateString{};
				switch (animationState) {
				case EnumRatAnimations::WALK:		animationStateString = "Walk";  break;
				case EnumRatAnimations::ATTACK: animationStateString = "Attack";  break;
				case EnumRatAnimations::HURT:		animationStateString = "Hurt";  break;
				case EnumRatAnimations::DEATH:	animationStateString = "Death";  break;
				default: animationStateString = "Idle";  break;
				}

#ifdef DEBUG_PRINT
				std::cout << "RatScript_v2_0::PlayAnimation(" << id << ") anim state: " << animationStateString << std::endl;
#endif // DEBUG_PRINT

				// Play the animation state
				if (it->second.p_ratAnimationComponent && it->second.animationStates.size())
				{
						try
						{
								if (forceRestart || // bypass the check that the animation being requested is already playing
										it->second.p_ratAnimationComponent->GetAnimationID() != it->second.animationStates.at(animationStateString))
								{
										it->second.p_ratAnimationComponent->SetCurrentAnimationID(it->second.animationStates.at(animationStateString));
										it->second.p_ratAnimationComponent->ResetAnimation();
										it->second.p_ratAnimationComponent->PlayAnimation();
								}
						}
						catch (...) { /* error */ }
				}
		}


		float RatScript_v2_0::GetAnimationDuration(EntityID const id) const
		{
				// Return the total duration of the animation
				return ResourceManager::GetInstance().GetAnimation(m_scriptData.at(id).p_ratAnimationComponent->GetAnimationID())->GetAnimationDuration();
		}


		void RatScript_v2_0::PlayAttackAudio(EntityID id)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }

			std::srand(static_cast<unsigned int>(std::time(nullptr)));
			int randSound = std::rand() % 5 + 1;						// Randomise between 1 and 5

			std::string soundPrefabBasePath = "AudioObject/Rat ";
			std::string ratTypeString;
			switch (it->second.ratType)
			{
			case EnumRatType::GUTTER:
			case EnumRatType::GUTTER_V1:
				ratTypeString = "Gutter ";
				break;
			case EnumRatType::BRAWLER:
				ratTypeString = "Brawler ";
				break;
			case EnumRatType::SNIPER:
				ratTypeString = "Sniper ";
				break;
			default:
				return;
			}

			std::string soundPrefab = soundPrefabBasePath + ratTypeString + "Attack SFX" + std::to_string(randSound) + ".prefab";

		

			PlayAudio(soundPrefab);
		}

		void RatScript_v2_0::PlayDeathAudio(EntityID id)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }

			std::srand(static_cast<unsigned int>(std::time(nullptr)));
			int randSound = std::rand() % 4 + 1;						// Randomize between 1 and 4

			std::string soundPrefabBasePath = "AudioObject/Rat ";
			std::string ratTypeString;
			switch (it->second.ratType)
			{
			case EnumRatType::GUTTER:
			case EnumRatType::GUTTER_V1:
				ratTypeString = "Gutter ";
				break;
			case EnumRatType::BRAWLER:
				ratTypeString = "Brawler ";
				break;
			case EnumRatType::SNIPER:
				ratTypeString = "Sniper ";
				break;
			default:
				return;
			}

			std::string soundPrefab = soundPrefabBasePath + ratTypeString + "Death SFX" + std::to_string(randSound) + ".prefab";

			

			PlayAudio(soundPrefab);
		}


		void RatScript_v2_0::PlayDetectionAudio(EntityID id)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }

			std::srand(static_cast<unsigned int>(std::time(nullptr)));
			int randSound = std::rand() % 5 + 1;					// Randomize between 1 and 5

			std::string soundPrefabBasePath = "AudioObject/Rat ";
			std::string ratTypeString;
			switch (it->second.ratType)
			{
			case EnumRatType::GUTTER:
			case EnumRatType::GUTTER_V1:
				ratTypeString = "Gutter ";
				break;
			case EnumRatType::BRAWLER:
				ratTypeString = "Brawler ";
				break;
			case EnumRatType::SNIPER:
				ratTypeString = "Sniper ";
				break;
			default:
				return;
			}

			std::string soundPrefab = soundPrefabBasePath + ratTypeString + "Detection SFX" + std::to_string(randSound) + ".prefab";

			

			PlayAudio(soundPrefab);
		}


		void RatScript_v2_0::PlayInjuredAudio(EntityID id)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }

			std::srand(static_cast<unsigned int>(std::time(nullptr)));
			int randSound = std::rand() % 5 + 1; // Randomize between 1 and 5 for the new audio files

			std::string soundPrefabBasePath = "AudioObject/Rat ";
			std::string ratTypeString;
			switch (it->second.ratType)
			{
			case EnumRatType::GUTTER:
			case EnumRatType::GUTTER_V1:
				ratTypeString = "Gutter ";
				break;
			case EnumRatType::BRAWLER:
				ratTypeString = "Brawler ";
				break;
			case EnumRatType::SNIPER:
				ratTypeString = "Sniper ";
				break;
			default:
				return;
			}

			std::string soundPrefab = soundPrefabBasePath + ratTypeString + "Injured SFX" + std::to_string(randSound) + ".prefab";

			

			PlayAudio(soundPrefab);
		}


		void RatScript_v2_0::PlayAudio(std::string const& r_soundPrefab)
		{
			PE::GlobalMusicManager::GetInstance().PlaySFX(r_soundPrefab, false);
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


		void RatScript_v2_0::EnableTelegraphs(EntityID id, vec2 const& targetPosition)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }

				vec2 directionOfTarget{ targetPosition - RatScript_v2_0::GetEntityPosition(it->second.myID) };
				float orientation = atan2(directionOfTarget.y, directionOfTarget.x);

				// Rotate the telegraph
				RotateEntityRelative(it->second.pivotEntityID, orientation);
				ToggleEntity(it->second.pivotEntityID, true); // enable the telegraph parent
				ToggleEntity(it->second.telegraphArrowEntityID, true); // enable the telegraph
		}


		void RatScript_v2_0::DisableTelegraphs(EntityID id)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }

				ToggleEntity(it->second.telegraphArrowEntityID, false); // disable the telegraph

				// Disable attack objects
				if (it->second.p_attackData) { it->second.p_attackData->DisableAttackObjects(); }
		}
		

		void RatScript_v2_0::DisableAllSpawnedEntities(EntityID id)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }

				// Disable all telegraphs and invisible objects
				ToggleEntity(it->second.detectionRadiusId, false);
				ToggleEntity(it->second.pivotEntityID, false);
				ToggleEntity(it->second.telegraphArrowEntityID, false);

				// Disable attack objects
				if (it->second.p_attackData) { it->second.p_attackData->DisableAttackObjects(); }
		}


		void RatScript_v2_0::TriggerStateChange(EntityID id, State* p_nextState, float const stateChangeDelay)
		{

			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }
			else if (it->second.delaySet) { return; }

#ifdef DEBUG_PRINT
			std::cout << "RatScript_v2_0::TriggerStateChange(" << id << ", " << (p_nextState ? p_nextState->GetName() : "nullptr") << ", " << stateChangeDelay << "): [START] current queued state before change is" << (it->second.GetQueuedState() ? it->second.GetQueuedState()->GetName() : "nullptr") << "\n";
#endif // DEBUG_PRINT

			it->second.shouldChangeState = true;
			it->second.timeBeforeChangingState = stateChangeDelay;
			it->second.delaySet = true;

			// Set the state that is queued up
			it->second.SetQueuedState(p_nextState, false);
#ifdef DEBUG_PRINT
			//std::cout << "State change requested for Rat ID: " << id << " to state " << p_nextState->GetName() << " with delay: " << stateChangeDelay << " seconds." << std::endl;
#endif // DEBUG_PRINT
		}


		
		bool RatScript_v2_0::CheckShouldStateChange(EntityID id, float const deltaTime)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return false; }
			else if (!it->second.GetQueuedState()) { return false; } // Return if no states have been queued

			// Waits for [timeBeforeChangingState] to pass before changing the state
			if (it->second.shouldChangeState)
			{
				if (it->second.timeBeforeChangingState > 0.f)
				{
					it->second.timeBeforeChangingState -= deltaTime;
					return false;
				}
				else
				{
					return true;
				}
			}

			return false;
		}


		void RatScript_v2_0::ChangeStateToHunt(EntityID const id, EntityID const targetId, float const stateChangeDelay)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }
				else if (!(it->second.isAlive)) { return; }
				TriggerStateChange(id, new RatHunt_v2_0{ targetId }, stateChangeDelay);
		}


		void RatScript_v2_0::ChangeStateToReturn(EntityID const id, float const stateChangeDelay)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }
				else if (!(it->second.isAlive)) { return; }
				TriggerStateChange(id, new RatReturn_v2_0, stateChangeDelay);
		}


		void RatScript_v2_0::ChangeStateToIdle(EntityID const id, float const stateChangeDelay)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }
				else if (!(it->second.isAlive)) { return; }

				// Check the type of idle behaviour based on the type of rat
				RatType idleBehaviour{ RatType::IDLE };
				switch (it->second.ratType)
				{
				case EnumRatType::GUTTER:
				case EnumRatType::BRAWLER:
				case EnumRatType::SNIPER:
				{
						idleBehaviour = RatType::IDLE;
						break;
				}
				default: break;
				}

				TriggerStateChange(id, new RatIdle_v2_0{ idleBehaviour }, stateChangeDelay);
		}

		void RatScript_v2_0::ChangeStateToMovement(EntityID const id, float const stateChangeDelay)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }
			else if (!(it->second.isAlive)) { return; }
			TriggerStateChange(id, new RatMovement_v2_0, stateChangeDelay);
		}


		void RatScript_v2_0::ChangeStateToAttack(EntityID const id, float const stateChangeDelay)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }
			else if (!(it->second.isAlive)) { return; }
			TriggerStateChange(id, new RatAttack_v2_0, stateChangeDelay);
		}


		void RatScript_v2_0::ChangeStateToDeath(EntityID const id, float const stateChangeDelay) 
		{
			TriggerStateChange(id, new RatDeathState_v2_0, stateChangeDelay);
		}

		bool RatScript_v2_0::GetExecutionPhaseTimeout(EntityID const id)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return false; }
				return (it->second.totalTimeCollidingIntoWall >= it->second.maxObstacleCollisionTime);
		}

		// ------------ CAT DETECTION ------------ //

		bool RatScript_v2_0::GetIsObstacle(EntityID const id)
		{
				return (EntityManager::GetInstance().Has<EntityDescriptor>(id) && EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Obstacle") != std::string::npos);
		}


		bool RatScript_v2_0::GetIsNonCagedCat(EntityID const id) 
		{
				return (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCat(id) && !GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCatCaged(id));
		}


		void RatScript_v2_0::ClearCollisionContainers(EntityID const id) 
		{
#ifdef DEBUG_PRINT
				std::cout << "RatScript_v2_0::ClearCollisionContainers(" << id << ")\n";
#endif // DEBUG_PRINT

				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }

				it->second.catsInDetectionRadius.clear();
				it->second.catsExitedDetectionRadius.clear();
				it->second.hitBy.clear();
		}


		void RatScript_v2_0::ClearDeadCats(std::set<EntityID>& catSet)
		{
				// Store the indices of the dead cats 
				std::vector<EntityID> deadCats{};
				for (EntityID const& id : catSet)
				{
						if (!(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCat(id)))
						{
								deadCats.emplace_back(id);
						}
				}

				// Remove the cats from the set
				for (EntityID const& id : deadCats)
				{
						catSet.erase(id);
				}
		}

		void RatScript_v2_0::CatEntered(EntityID const id, EntityID const catID)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }

			// Check if the cat is alive
			if (!(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCat(catID))) { return; }

			if (it->second.catsInDetectionRadius.find(catID) == it->second.catsInDetectionRadius.end())
			{
				PlayDetectionAudio(id);
			}

			// Store the cat in the container
			it->second.catsInDetectionRadius.insert(catID);

			// Remove the cats in the enter container from the exit container
			it->second.catsExitedDetectionRadius.erase(catID);
		}

		void RatScript_v2_0::CatExited(EntityID const id, EntityID const catID)
		{
			auto it = m_scriptData.find(id);
			if (it == m_scriptData.end()) { return; }

			// Check if the cat is alive
			if (!(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCat(catID))) { return; }

			// Store the cat in the container
			it->second.catsExitedDetectionRadius.insert(catID);

			// Remove the cats in the exit container from the enter container
			it->second.catsInDetectionRadius.erase(catID);
		}


		bool RatScript_v2_0::CheckDetectionTriggerEntered(EntityID const ratId, EntityID const entity1, EntityID const entity2)
		{
				auto it = m_scriptData.find(ratId);
				if (it == m_scriptData.end()) { return false; }

				// check if entity1 is the rat's detection collider and entity2 is cat
				if ((entity1 == it->second.detectionRadiusId) && RatScript_v2_0::GetIsNonCagedCat(entity2))
				{
						GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(it->second.myID, entity2);
						return true;
				}
				// check if entity2 is the rat's detection collider and entity1 is cat
				else if ((entity2 == it->second.detectionRadiusId) && RatScript_v2_0::GetIsNonCagedCat(entity1))
				{
						GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(it->second.myID, entity1);
						return true;
				}
				return false;
		}


		bool RatScript_v2_0::CheckDetectionTriggerExited(EntityID const ratId, EntityID const entity1, EntityID const entity2)
		{
				auto it = m_scriptData.find(ratId);
				if (it == m_scriptData.end()) { return false; }

				// check if entity1 is the rat's detection collider and entity2 is cat
				if ((entity1 == it->second.detectionRadiusId) && RatScript_v2_0::GetIsNonCagedCat(entity2))
				{
						GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(ratId, entity2);
						return true;
				}
				// check if entity2 is the rat's detection collider and entity1 is cat
				else if ((entity2 == it->second.detectionRadiusId) && RatScript_v2_0::GetIsNonCagedCat(entity1))
				{
						GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(ratId, entity1);
						return true;
				}
				return false;
		}


		bool RatScript_v2_0::CheckRatTouchingCat(EntityID const ratId, EntityID const entity1, EntityID const entity2)
		{
				auto it = m_scriptData.find(ratId);
				if (it == m_scriptData.end()) { return false; }

#ifdef DEBUG_PRINT
				std::cout << "RatScript_v2_0::CheckRatTouchingCat(" << ratId << "): collision between ent " << entity1 << " and " << entity2 << std::endl;
#endif // DEBUG_PRINT

				// check if entity1 is rat and entity2 is cat
				if ((entity1 == it->second.myID) && RatScript_v2_0::GetIsNonCagedCat(entity2))
				{
						GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DealDamageToCat(entity2, it->second.myID);
						return true;
				}
				// check if entity2 is rat and entity1 is cat
				else if ((entity2 == it->second.myID) && RatScript_v2_0::GetIsNonCagedCat(entity1))
				{
						GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DealDamageToCat(entity1, it->second.myID);
						return true;
				}
				return false;
		}


		bool RatScript_v2_0::CheckRatTouchingWall(EntityID const ratId, EntityID const entity1, EntityID const entity2)
		{
				auto it = m_scriptData.find(ratId);
				if (it == m_scriptData.end()) { return false; }

#ifdef DEBUG_PRINT
				std::cout << "RatScript_v2_0::CheckRatTouchingCat(" << ratId << "): collision between ent " << entity1 << " and " << entity2 << std::endl;
#endif // DEBUG_PRINT

				// check if entity1 is rat and entity2 is cat
				if ((entity1 == it->second.myID) && GetIsObstacle(entity2))
				{
						it->second.totalTimeCollidingIntoWall += TimeManager::GetInstance().GetDeltaTime();
						return true;
				}
				// check if entity2 is rat and entity1 is cat
				else if ((entity2 == it->second.myID) && GetIsObstacle(entity1))
				{
						it->second.totalTimeCollidingIntoWall += TimeManager::GetInstance().GetDeltaTime();
						return true;
				}
				return false;
		}


		bool RatScript_v2_0::CheckRatStopTouchingWall(EntityID const ratId, EntityID const entity1, EntityID const entity2)
		{
				auto it = m_scriptData.find(ratId);
				if (it == m_scriptData.end()) { return false; }

#ifdef DEBUG_PRINT
				std::cout << "RatScript_v2_0::CheckRatTouchingCat(" << ratId << "): collision between ent " << entity1 << " and " << entity2 << std::endl;
#endif // DEBUG_PRINT

				// check if entity1 is rat and entity2 is cat
				if ((entity1 == it->second.myID) && GetIsObstacle(entity2))
				{
						it->second.totalTimeCollidingIntoWall = 0.f;
						return true;
				}
				// check if entity2 is rat and entity1 is cat
				else if ((entity2 == it->second.myID) && GetIsObstacle(entity1))
				{
						it->second.totalTimeCollidingIntoWall = 0.f;
						return true;
				}
				return false;
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



		bool RatScript_v2_0::SetTarget(EntityID id, EntityID targetId, bool const capMaximum)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return false; }

#ifdef DEBUG_PRINT
				std::cout << "RatScript_v2_0::SetTarget(" << id << "): target id: " << targetId << ", ";
#endif // DEBUG_PRINT

				bool targetSet{ SetTarget(id, RatScript_v2_0::GetEntityPosition(targetId), capMaximum) };
				it->second.targetedCat = targetId;
				return targetSet;
		}


		bool RatScript_v2_0::SetTarget(EntityID id, vec2 const& r_targetPosition, bool const capMaximum)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return false; }

#ifdef DEBUG_PRINT
				std::cout << "RatScript_v2_0::SetTarget(" << id << ") r_targetPosition: (" << r_targetPosition.x << ", " << r_targetPosition.y << "), capmax? " << capMaximum << "\n";
#endif // DEBUG_PRINT

				it->second.targetedCat = 0U;
				vec2 ratPosition = RatScript_v2_0::GetEntityPosition(id);

				vec2 ratToTarget{ r_targetPosition - ratPosition };
				if (CompareFloats(ratToTarget.x, 0.f) && CompareFloats(ratToTarget.y, 0.f))
				{
						// If the rat is where the target is, just zero everything out
						it->second.ratPlayerDistance = 0.f;
						it->second.directionFromRatToPlayerCat.Zero();
						it->second.targetPosition = ratPosition;
				}
				else
				{
						it->second.ratPlayerDistance = ratToTarget.Length();
						if (capMaximum && it->second.ratPlayerDistance > it->second.maxMovementRange)
						{
								it->second.ratPlayerDistance = it->second.maxMovementRange;
						}
						it->second.directionFromRatToPlayerCat = ratToTarget.GetNormalized();
						it->second.targetPosition = ratPosition + it->second.directionFromRatToPlayerCat * it->second.ratPlayerDistance;
				}

#ifdef DEBUG_PRINT
				std::cout << "--- RatScript_v2_0::SetTarget(" << id << ") stats: \n"
						<< "ratPosition: (" << ratPosition.x << ", " << ratPosition.y << ")\n"
						<< "ratPlayerDistance: " << it->second.ratPlayerDistance << " (versus max range " << it->second.maxMovementRange << ")\n"
						<< "directionFromRatToPlayerCat: (" << it->second.directionFromRatToPlayerCat.x << ", " << it->second.directionFromRatToPlayerCat.y << ")\n"
						<< "targetPosition: (" << it->second.targetPosition.x << ", " << it->second.targetPosition.y << ")\n"
						<< "--- end stats for SetTarget(" << id << ") ---" << std::endl;
#endif // DEBUG_PRINT

				return (it->second.ratPlayerDistance > it->second.minDistanceToTarget);
		}


		bool RatScript_v2_0::CalculateMovement(EntityID id, float deltaTime)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return false; }
#ifdef DEBUG_PRINT
				//std::cout << "RatScript_v2_0::CalculateMovement(" << id << "): ratPlayerDistance: " << it->second.ratPlayerDistance << ", curr pos: (" << RatScript_v2_0::GetEntityPosition(id).x << ", " << RatScript_v2_0::GetEntityPosition(id).y << ")\n";
				static bool printOnce{false};
#endif // DEBUG_PRINT
				if (it->second.ratPlayerDistance > 0.f)
				{
						float amountToMove{ std::min(it->second.ratPlayerDistance, it->second.movementSpeed * deltaTime) };
						vec2 newPosition = RatScript_v2_0::GetEntityPosition(id) + (it->second.directionFromRatToPlayerCat * amountToMove);
						RatScript_v2_0::PositionEntity(id, newPosition);
						it->second.ratPlayerDistance -= amountToMove;

#ifdef DEBUG_PRINT
						printOnce = false;
						//std::cout << "RatMovement_v2_0::CalculateMovement - Rat ID: " << id
						//		<< " moved to new position: (" << newPosition.x << ", " << newPosition.y
						//		<< "), Remaining distance: " << it->second.ratPlayerDistance << std::endl;
						// 
						//std::cout << "RatScript_v2_0::CalculateMovement(" << id << ") after moving: ratPlayerDistance: " << it->second.ratPlayerDistance << ", new pos: (" << RatScript_v2_0::GetEntityPosition(id).x << ", " << RatScript_v2_0::GetEntityPosition(id).y << ")\n";
#endif // DEBUG_PRINT

						return CheckDestinationReached(it->second.minDistanceToTarget, newPosition, it->second.targetPosition);
				}
				else
				{
#ifdef DEBUG_PRINT
						if (!printOnce) 
						{
								printOnce = true;
								std::cout << "RatScript_v2_0::CalculateMovement(" << id << "): dist to target is zero (" << it->second.targetPosition.x << ", " << it->second.targetPosition.y << ")\n";
						}
#endif // DEBUG_PRINT

						//RatScript_v2_0::PositionEntity(id, it->second.targetPosition);
						it->second.ratPlayerDistance = 0.f;
						return true;
				}
		}


		bool RatScript_v2_0::CheckDestinationReached(EntityID id)
		{
				RatScript_v2_0_Data* data = GETSCRIPTDATA(RatScript_v2_0, id);
				if (!data) { return false; }
				return CheckDestinationReached(data->minDistanceToTarget, GetEntityPosition(data->myID), data->targetPosition);
		}


		bool RatScript_v2_0::CheckDestinationReached(float const minDistanceToTarget, const vec2& newPosition, const vec2& targetPosition)
		{
				float distanceToTarget{ (newPosition - targetPosition).LengthSquared() };
				bool reached = distanceToTarget <= (minDistanceToTarget * minDistanceToTarget);

#ifdef DEBUG_PRINT
				//std::cout << "RatScript_v2_0::CheckDestinationReached(): distanceToTarget: " << distanceToTarget << ", minDistToTarget: " << (minDistanceToTarget * minDistanceToTarget) << ")\n";
#endif // DEBUG_PRINT

				return reached;
		}


		void RatScript_v2_0::DamageRat(EntityID ratId, EntityID damageSourceId, int damage)
		{
			auto it = m_scriptData.find(ratId);
			if (it == m_scriptData.end()) { return; }

			// Check if the rat has hit this entity during this phase
			if (it->second.hitBy.find(damageSourceId) != it->second.hitBy.end())
			{
				// Don't damage the rat twice via the same source
				return;
			}
			it->second.hitBy.emplace(damageSourceId);

			// Subtract the damage from the rat's health
			it->second.ratHealth -= damage;

			// Check if the rat's health drops below or equals zero
			if (it->second.ratHealth <= 0)
			{
				// Kill the rat
				KillRat(ratId);
			}
			else 
			{
				PlayAnimation(ratId, EnumRatAnimations::HURT);
				PlayInjuredAudio(ratId);
			}
		}


		void RatScript_v2_0::KillRat(EntityID id)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }

				// Prevent this function from getting called more than once
				if (!(it->second.isAlive)) { return; }
				it->second.isAlive = false;  // Marking the rat as not alive
				
				ClearCollisionContainers(id); // Clear collision containers

				ChangeStateToDeath(it->second.myID, 0.f); 
				// @TODO Check if this should take the current rat animation duration into account?
		}


		void RatScript_v2_0::CreateCheckStateManager(EntityID id)
		{
			if (m_scriptData.count(id))
			{
				if (m_scriptData.at(id).p_stateManager)
						return;

				m_scriptData[id].p_stateManager = new StateMachine{};
				ChangeStateToIdle(id);
			}
		}


		void RatScript_v2_0::ChangeRatState(EntityID id)
		{
				auto it = m_scriptData.find(id);
				if (it == m_scriptData.end()) { return; }

				// Change the state
				if (it->second.p_stateManager)
				{
#ifdef DEBUG_PRINT
						std::cout << "RatScript_v2_0::ChangeRatState(" << id << ", " << it->second.GetQueuedState()->GetName() << ")\n";
#endif // DEBUG_PRINT
						it->second.p_stateManager->ChangeState(it->second.GetQueuedState(), id);
				}

				// Reset all the values
				it->second.shouldChangeState = false;
				it->second.timeBeforeChangingState = 0.f;
				it->second.delaySet = false;
				it->second.SetQueuedState(nullptr, true);
				it->second.hasRatStateChanged = true;
				DisableTelegraphs(it->second.myID);
				it->second.hasChangedToDeathState = !(it->second.isAlive);
		}

		EntityID RatScript_v2_0::CreateDetectionRadius(RatScript_v2_0_Data const& r_data)
		{
			SerializationManager serializationManager;
			EntityID radiusId{ serializationManager.LoadFromFile("RatDetectionRadius.prefab") };
			Hierarchy::GetInstance().AttachChild(r_data.myID, radiusId);
			PositionEntity(radiusId, GetEntityPosition(r_data.myID));
			PositionEntityRelative(radiusId, vec2{ 0.f, 0.f });

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


		void RatScript_v2_0::CreateRatPathTelegraph(RatScript_v2_0_Data& r_data)
		{
				SerializationManager serializationManager;
				r_data.pivotEntityID = EntityFactory::GetInstance().CreateEntity<Transform>();
				Hierarchy::GetInstance().AttachChild(r_data.myID, r_data.pivotEntityID);
				PositionEntityRelative(r_data.pivotEntityID, vec2{ 0.f, 0.f });

				vec2 ratScale{ GetEntityScale(r_data.myID) };
				r_data.telegraphArrowEntityID = serializationManager.LoadFromFile("PawPrints.prefab");
				ToggleEntity(r_data.telegraphArrowEntityID, false); // set to inactive, it will only show during planning phase
				ScaleEntity(r_data.telegraphArrowEntityID, ratScale.x * 0.5f, ratScale.y * 0.5f);
				Hierarchy::GetInstance().AttachChild(r_data.pivotEntityID, r_data.telegraphArrowEntityID); // attach child to parent
				
				PositionEntityRelative(r_data.telegraphArrowEntityID, vec2{ ratScale.x * 0.7f, 0.f });
		}


		void RatScript_v2_0::InstantiateRatAttack(RatScript_v2_0_Data& r_data)
		{
				// Delete any existing attack data
				r_data.DeleteAttackData();

				switch (r_data.ratType) 
				{
				case EnumRatType::SNIPER:
				{
						r_data.p_attackData = dynamic_cast<AttackDataBase_v2_0*>(new SniperRatAttack_v2_0{ r_data.myID });
						break;
				}
				default:
				{
						r_data.p_attackData = dynamic_cast<AttackDataBase_v2_0*>(new GutterRatAttack_v2_0{ r_data.myID });
						break;
				}
				}

				// Create the objects needed for the attack
				r_data.p_attackData->CreateAttackObjects();
		}

} // End of namespace PE