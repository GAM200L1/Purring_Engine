/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     RatScript.cpp
 \date     20-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains definitions for functions used for a rats mechanics

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "RatScript.h"
#include "GameStateManager.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "Data/SerializationManager.h"
#include "Logic/LogicSystem.h"
#include "CatScript.h"
#include "FollowScript.h"

namespace PE
{

	// ===== Rat Script Definition ===== //
	
	// ----- Destructor ----- //
	RatScript::~RatScript()
	{
		for (auto& [key, value] : m_scriptData)
		{
			delete value.p_stateManager;
		}
	}

	// ----- Public Functions ----- //
	void RatScript::Init(EntityID id)
	{
		m_scriptData[id].ratID = id;

		// create state manager for the entity
		if (m_scriptData[id].p_stateManager) { return; }

		if (GameStateManager::GetInstance().GetGameState() == GameStates::MOVEMENT)
		{
			m_scriptData[id].p_stateManager = new StateMachine{};
			m_scriptData[id].p_stateManager->ChangeState(new RatIDLE{}, id);
		}

		CreateAttackTelegraphs(id);
	}
		 
	void RatScript::Update(EntityID id, float deltaTime)
	{
		if (GameStateManager::GetInstance().GetGameState() == GameStates::SPLASHSCREEN) { return; } // don't allow cat script to update during splashscreen gamestate

		/*if (GameStateManager::GetInstance().GetGameState() == GameStates::WIN) { return; } // do something when they win

		if (GameStateManager::GetInstance().GetGameState() == GameStates::LOSE) { return; } // do something when they lose */

		if (m_scriptData[id].health <= 0)
		{
			// idk if need these

			// TODO ------------------------------------------------------------ //
			// Add function here to get rat to be in DEATH animation when HP reaches 0
			// ----------------------------------------------------------------- //

			// Set game state to win when player HP is 0 (we only have one anyway)
			// probably some DT stuff to let the animation run
			// GameStateManager::GetInstance().SetGameState(GameStates::WIN);
			if (EntityManager::GetInstance().Has<AnimationComponent>(id))
			{
				try
				{
					if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID() != m_scriptData[id].animationStates.at("Death"))
						EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Death"));
				}
				catch (...)
				{
					// error
				}

				// death animation example
				if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetCurrentFrameIndex() == EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationMaxIndex())
				{
					ToggleEntity(id, false);
				}
			}


			return;
		}

		if (!m_scriptData[id].p_stateManager)
		{
			m_scriptData[id].ratID = id;

			// Make a statemanager and set the starting state
			if (m_scriptData[id].p_stateManager) { return; }

			if (GameStateManager::GetInstance().GetGameState() == GameStates::MOVEMENT)
			{
				m_scriptData[id].p_stateManager = new StateMachine{};
				m_scriptData[id].p_stateManager->ChangeState(new RatIDLE{}, id);
			}
		}

		m_scriptData[id].p_stateManager->Update(id, deltaTime);

		if (m_scriptData[id].p_stateManager->GetStateName() == "IDLE")
		{
			// TODO ------------------------------------------------------------ //
			// Add function here to get rat to be in IDLE animation when player is planning Attack and movement
			// ----------------------------------------------------------------- //
			if (EntityManager::GetInstance().Has<AnimationComponent>(id))
			{
				try
				{
					if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID() != m_scriptData[id].animationStates.at("Idle"))
						EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Idle"));
				}
				catch (...)
				{
					// error
				}
			}
			// If current gamestate is set to attack planning, change state to CatAttackPLAN
			if (GameStateManager::GetInstance().GetGameState() == GameStates::EXECUTE)
			{
				TriggerStateChange(id); // immediate state change
				if (CheckShouldStateChange(id, deltaTime))
				{
					m_scriptData[id].p_stateManager->ChangeState(new RatMovementEXECUTE{}, id);
					m_scriptData[id].finishedExecution = false;
				}
			}
		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "MovementEXECUTE")
		{
			// TODO ------------------------------------------------------------ //
			// Add function here to get rat to be in MOVEMENT animation when executing movement
			// ----------------------------------------------------------------- //
			if (EntityManager::GetInstance().Has<AnimationComponent>(id))
			{
				try
				{
					EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Walk"));
				}
				catch (...)
				{
					// error
				}
			}
			if (CheckShouldStateChange(id, deltaTime))
			{
				// trigger state change called in MovementEXECUTE state update
				m_scriptData[id].p_stateManager->ChangeState(new RatAttackEXECUTE{}, id);
			}
		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "AttackEXECUTE")
		{
			// TODO ------------------------------------------------------------ //
			// Add function here to get player to be in ATTACK animation when planning movement
			// ----------------------------------------------------------------- //
			if (EntityManager::GetInstance().Has<AnimationComponent>(id))
			{
				if (m_scriptData[id].finishedExecution)
				{
					try
					{
						if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID() != m_scriptData[id].animationStates.at("Idle"))
							EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Idle"));
					}
					catch (...)
					{
						// error
					}
				}
				else
				{
					try
					{
						EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Attack"));
					}
					catch (...)
					{
						// error
					}
				}


			}
			// Check if the state should be changed
			if (GameStateManager::GetInstance().GetGameState() == GameStates::MOVEMENT)
			{
				TriggerStateChange(id); // immediate state change
				if (CheckShouldStateChange(id, deltaTime))
				{
					m_scriptData[id].p_stateManager->ChangeState(new RatIDLE{}, id);
				}
			}
		}
	}
		
	void RatScript::TriggerStateChange(EntityID id, float const stateChangeDelay)
	{
		if (m_scriptData[id].delaySet) { return; }

		m_scriptData[id].shouldChangeState = true;
		m_scriptData[id].timeBeforeChangingState = stateChangeDelay;
		m_scriptData[id].delaySet = true;
	}


	bool RatScript::CheckShouldStateChange(EntityID id, float const deltaTime)
	{
		if (m_scriptData[id].shouldChangeState)
		{
			if (m_scriptData[id].timeBeforeChangingState > 0.f)
			{
				m_scriptData[id].timeBeforeChangingState -= deltaTime;
				return false;
			}
			else
			{
				m_scriptData[id].shouldChangeState = false;
				m_scriptData[id].timeBeforeChangingState = 0.f;
				m_scriptData[id].delaySet = false;
				return true;
			}
		}

		return false;
	}


	void RatScript::OnAttach(EntityID id)
	{
		if (!EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<RigidBody>()))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<RigidBody>() });
			EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
		}
		else
		{
			EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
		}

		if (!EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<Collider>()))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<Collider>() });
			EntityManager::GetInstance().Get<Collider>(id).colliderVariant = CircleCollider();
		}
		else
		{
			EntityManager::GetInstance().Get<Collider>(id).colliderVariant = CircleCollider();
		}

		if (m_scriptData.find(id) == m_scriptData.end())
		{
			m_scriptData[id] = RatScriptData{};
		}
		else
		{
			delete m_scriptData[id].p_stateManager;
			m_scriptData[id] = RatScriptData{};
		}
	}
		 
	void RatScript::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			delete m_scriptData.at(id).p_stateManager;
			m_scriptData.erase(id);
		}
	}

	void RatScript::ToggleEntity(EntityID id, bool setToActive)
	{
		// Exit if the entity is not valid
		if (!EntityManager::GetInstance().IsEntityValid(id)) { return; }

		// Toggle the entity
		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = setToActive;
	}


	void RatScript::PositionEntity(EntityID const transformId, vec2 const& r_position)
	{
		try
		{
			Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
			r_transform.position = r_position;
		}
		catch (...) { return; }
	}


	void RatScript::ScaleEntity(EntityID const transformId, float const width, float const height)
	{
		try
		{
			Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
			r_transform.width = width;
			r_transform.height = height;
		}
		catch (...) { return; }
	}


	vec2 RatScript::GetEntityPosition(EntityID const transformId)
	{
		try
		{
			Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
			return r_transform.position;
		}
		catch (...) { return vec2{}; }
	}


	vec2 RatScript::GetEntityScale(EntityID const transformId)
	{
		try
		{
			Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
			return vec2{ r_transform.width, r_transform.height };
		}
		catch (...) { return vec2{}; }
	}

	// ----- Private Functions ----- //
	void RatScript::CreateAttackTelegraphs(EntityID id)
	{
		vec2 const& ratScale = GetEntityScale(m_scriptData[id].ratID);
		RatScriptData & data = m_scriptData[id];

		SerializationManager serializationManager;

		data.psudoRatID = EntityFactory::GetInstance().CreateEntity<Transform>();
		EntityManager::GetInstance().Get<Transform>(data.psudoRatID).position = GetEntityScale(id);

		// create the arrow telegraph
		data.arrowTelegraphID = serializationManager.LoadFromFile("../Assets/Prefabs/EnemyArrowTelegraph_Prefab.json");
		ToggleEntity(data.arrowTelegraphID, false); // set to inactive, it will only show during planning phase
		ScaleEntity(data.arrowTelegraphID, ratScale.x, ratScale.y);
		EntityManager::GetInstance().Get<EntityDescriptor>(data.arrowTelegraphID).parent = data.psudoRatID;
		EntityManager::GetInstance().Get<Transform>(data.arrowTelegraphID).relPosition.x = ratScale.x * data.detectionRadius * 0.5f;
		
		// create cross attack telegraph
		data.attackTelegraphID = serializationManager.LoadFromFile("../Assets/Prefabs/EnemyAttackTelegraph_Prefab.json");
		ToggleEntity(data.attackTelegraphID, false); // set to inactive, it will only show during planning phase if the cat is in the area
		ScaleEntity(data.attackTelegraphID, data.attackDiameter, data.attackDiameter);

		// create the detection radius
		data.detectionTelegraphID = serializationManager.LoadFromFile("../Assets/Prefabs/EnemyDetectionTelegraph_Prefab.json");
		ToggleEntity(data.detectionTelegraphID, false); // set to inactive it will only show during planning phase
		ScaleEntity(data.detectionTelegraphID, ratScale.x * 2.f * data.detectionRadius, ratScale.y * 2.f * data.detectionRadius);
		EntityManager::GetInstance().Get<EntityDescriptor>(data.detectionTelegraphID).parent = id; // detection UI will always show
		EntityManager::GetInstance().Get<Transform>(data.detectionTelegraphID).relPosition = vec2{ 0.f,0.f }; // detection UI will always show
	}


	// ===== RatIDLE definition ===== //

	// ----- Public Functions ----- //
	void RatIDLE::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(RatScript, id);
		EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0);
		RatScript::PositionEntity(p_data->psudoRatID, EntityManager::GetInstance().Get<PE::Transform>(id).position);
		RatScript::ToggleEntity(p_data->detectionTelegraphID, true); // show the detection telegraph
		Transform const& ratObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
		Transform const& catObject = PE::EntityManager::GetInstance().Get<PE::Transform>(p_data->mainCatID);
		vec2 absRatScale = vec2{ abs(ratObject.width),abs(ratObject.height) };
		vec2 absCatScale = vec2{ abs(catObject.width),abs(catObject.height) };
		p_data->distanceFromPlayer = RatScript::GetEntityPosition(id).Distance(catObject.position) - (absCatScale.x * 0.5f) - (absRatScale.x * 0.5f);
		p_data->directionToTarget = RatScript::GetEntityPosition(p_data->mainCatID) - RatScript::GetEntityPosition(id);
		p_data->directionToTarget.Normalize();


		// if the cat is within the detection radius
		if (p_data->distanceFromPlayer <= ((RatScript::GetEntityScale(p_data->detectionTelegraphID).x * 0.5f) - (absCatScale.x * 0.5f) - (absRatScale.x * 0.5f)) && EntityManager::GetInstance().Get<EntityDescriptor>(p_data->mainCatID).isActive)
		{			
			if (p_data->distanceFromPlayer <= (absCatScale.x * 0.5f))
			{
				p_data->distanceFromPlayer = 0.f; // cat and rat are directly next to each other so there is no distance to really cover
			}
			else
			{
				// settings for the arrow
				EntityManager::GetInstance().Get<Transform>(p_data->arrowTelegraphID).relPosition.x = catObject.position.Distance(ratObject.position) * 0.5f;
				EntityManager::GetInstance().Get<Transform>(p_data->arrowTelegraphID).width = p_data->distanceFromPlayer;
				// find rotation to rotate the psuedorat entity so the arrow will be rotated accordingly
				float rotation = atan2(p_data->directionToTarget.y, p_data->directionToTarget.x);
				PE::EntityManager::GetInstance().Get<PE::Transform>(p_data->psudoRatID).orientation = rotation;
				RatScript::ToggleEntity(p_data->arrowTelegraphID, true); // show the arrow of movement
			}
			
			// Ensure the rat is facing the direction of their movement
			vec2 newScale{ RatScript::GetEntityScale(id) };
			newScale.x = std::abs(newScale.x) * (((catObject.position - ratObject.position).Dot(vec2{1.f, 0.f}) >= 0.f) ? 1.f : -1.f); // Set the scale to negative if the rat is facing left
			RatScript::ScaleEntity(id, newScale.x, newScale.y);

			// settings for the attack cross
			RatScript::PositionEntity(p_data->attackTelegraphID, catObject.position);
			RatScript::ToggleEntity(p_data->attackTelegraphID, true); // show the arrow of movement
		}
		else
		{
			p_data->distanceFromPlayer = 0.f;
		}
	}
	
	void RatIDLE::StateUpdate(EntityID id, float deltaTime)
	{
		
	}
	
	void RatIDLE::StateCleanUp()
	{
		
	}
	
	void RatIDLE::StateExit(EntityID id)
	{
		// disables all the UI from showing up
		RatScript::ToggleEntity(p_data->arrowTelegraphID, false);
		RatScript::ToggleEntity(p_data->detectionTelegraphID, false);
		RatScript::ToggleEntity(p_data->attackTelegraphID, false);
		EntityManager::GetInstance().Get<Graphics::Renderer>(p_data->attackTelegraphID).SetEnabled(false);
	}

	void RatScript::LoseHP(EntityID id, int damageTaken)
	{
		m_scriptData[id].health -= damageTaken;
		std::cout << "Rat HP: " << m_scriptData[id].health << '\n';
	}

	// ===== RatMovementEXECUTE ===== //


	// ----- Public Functions ----- //
	 void RatMovementEXECUTE::StateEnter(EntityID id)
	 {
		 p_data = GETSCRIPTDATA(RatScript, id);
		 EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0);
		 m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatMovementEXECUTE::RatHitCat, this);
		 m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatMovementEXECUTE::RatHitCat, this);
	 }

	 void RatMovementEXECUTE::StateUpdate(EntityID id, float deltaTime)
	 {
		 if (p_data->distanceFromPlayer > 0.f)
		 {
			 RatScript::PositionEntity(id, RatScript::GetEntityPosition(id) + (p_data->directionToTarget * p_data->movementSpeed * deltaTime));
			 
			 p_data->distanceFromPlayer -= p_data->movementSpeed * deltaTime;
		 }
		 else
		 {
			 GETSCRIPTINSTANCEPOINTER(RatScript)->TriggerStateChange(id, 0.5f);
		 }
	 }

	 void RatMovementEXECUTE::StateCleanUp()
	 {
		 REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
		 REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
	 }

	 void RatMovementEXECUTE::StateExit(EntityID id)
	 {
		 p_data->distanceFromPlayer = 0.f;
		 m_hitCat = false;
	 }

	 void RatMovementEXECUTE::RatHitCat(const Event<CollisionEvents>& r_TE)
	 {
		 if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
		 {
			 OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
			 if ((OTEE.Entity1 == p_data->ratID || OTEE.Entity1 == p_data->attackTelegraphID) && EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
			 {
				 // if cat has been checked before check the next event
				 if (m_hitCat) { return; }

				 // save the id of the cat that has been checked so that it wont be checked again
				 m_hitCat = true;
				 try
				 {
					 auto& catFollowScript = *GETSCRIPTDATA(FollowScript, p_data->mainCatID);
					 // checks if this cat the rat or rat attack collided with is part of the chain
					 if (catFollowScript.NumberOfFollower >= 2)
					 {
						 if (std::find(catFollowScript.FollowingObject.begin(), catFollowScript.FollowingObject.end(), OTEE.Entity2) != catFollowScript.FollowingObject.end())
						 {
							 int const& damage = (OTEE.Entity1 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
							 try
							 {
								 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
							 }
							 catch (...) {}
						 }
					 }
					 // else checks if the cat hit was the main cat
					 else if (OTEE.Entity2 == p_data->mainCatID)
					 {
						 int const& damage = (OTEE.Entity1 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
						 try
						 {
							 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
						 }
						 catch (...) {}
					 }
				 }
				 catch (...) {}
			 }
			 else if ((OTEE.Entity2 == p_data->ratID || OTEE.Entity2 == p_data->attackTelegraphID) && EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
			 {
				 // if cat has been checked before check the next event
				 if (m_hitCat) { return; }

				 // save the id of the cat that has been checked so that it wont be checked again
				 m_hitCat = true;
				 try
				 {
					 auto& catFollowScript = *GETSCRIPTDATA(FollowScript, p_data->mainCatID);
					 // checks if this cat the rat or rat attack collided with is part of the chain
					 if (catFollowScript.NumberOfFollower >= 2)
					 {
						 if (std::find(catFollowScript.FollowingObject.begin(), catFollowScript.FollowingObject.end(), OTEE.Entity1) != catFollowScript.FollowingObject.end())
						 {
							 int const& damage = (OTEE.Entity2 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
							 try
							 {
								 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
							 }
							 catch (...) {}
						 }
					 }
					 // else checks if the cat hit was the main cat
					 else if (OTEE.Entity1 == p_data->mainCatID)
					 {
						 int const& damage = (OTEE.Entity2 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
						 try
						 {
							 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
						 }
						 catch (...) {}
					 }
				 }
				 catch (...) {}
			 }
		 }
		 else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
		 {
			 OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
			 if ((OTSE.Entity1 == p_data->ratID || OTSE.Entity1 == p_data->attackTelegraphID) && EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity2).name.find("Cat") != std::string::npos)
			 {
				 // if cat has been checked before check the next event
				 if (m_hitCat) { return; }

				 // save the id of the cat that has been checked so that it wont be checked again
				 m_hitCat = true;
				 try
				 {
					 auto& catFollowScript = *GETSCRIPTDATA(FollowScript, p_data->mainCatID);
					 // checks if this cat the rat or rat attack collided with is part of the chain
					 if (catFollowScript.NumberOfFollower >= 2)
					 {
						 if (std::find(catFollowScript.FollowingObject.begin(), catFollowScript.FollowingObject.end(), OTSE.Entity2) != catFollowScript.FollowingObject.end())
						 {
							 int const& damage = (OTSE.Entity1 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
							 try
							 {
								 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
							 }
							 catch (...) {}
						 }
					 }
					 // else checks if the cat his was the main cat
					 else if (OTSE.Entity2 == p_data->mainCatID)
					 {
						 int const& damage = (OTSE.Entity1 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
						 try
						 {
							 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
						 }
						 catch (...) {}
					 }
				 }
				 catch (...) {}
			 }
			 else if ((OTSE.Entity2 == p_data->ratID || OTSE.Entity2 == p_data->attackTelegraphID) && EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity1).name.find("Cat") != std::string::npos)
			 {
				 // if cat has been checked before check the next event
				 if (m_hitCat) { return; }

				 // save the id of the cat that has been checked so that it wont be checked again
				 m_hitCat = true;
				 try
				 {
					 auto& catFollowScript = *GETSCRIPTDATA(FollowScript, p_data->mainCatID);
					 // checks if this cat the rat or rat attack collided with is part of the chain
					 if (catFollowScript.NumberOfFollower >= 2)
					 {
						 if (std::find(catFollowScript.FollowingObject.begin(), catFollowScript.FollowingObject.end(), OTSE.Entity1) != catFollowScript.FollowingObject.end())
						 {
							 int const& damage = (OTSE.Entity2 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
							 try
							 {
								 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
							 }
							 catch (...) {}
						 }
					 }
					 // else checks if the cat hit was the main cat
					 else if (OTSE.Entity1 == p_data->mainCatID)
					 {
						 int const& damage = (OTSE.Entity2 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
						 try
						 {
							 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
						 }
						 catch (...) {}
					 }
				 }
				 catch (...) {}
			 }
		 }
	 }


	 // ===== RatAttackEXECUTE ===== //
	 void RatAttackEXECUTE::StateEnter(EntityID id)
	 {
		 p_data = GETSCRIPTDATA(RatScript, id);
		 EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0);
		 m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatAttackEXECUTE::RatHitCat, this);
		 m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatAttackEXECUTE::RatHitCat, this);
		 m_delay = p_data->attackDelay;
	 }

	 void RatAttackEXECUTE::StateUpdate(EntityID id, float deltaTime)
	 {
		 // allows attack to last for attackDuration timing before going back to movement state
		 if (m_delay > 0.f)
		 {
			 m_delay -= deltaTime;
		 }
		 else
		 {
			 RatScript::ToggleEntity(p_data->attackTelegraphID, true);
			 if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetCurrentFrameIndex() == EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationMaxIndex())
				p_data->finishedExecution = true;
		 }

	 }

	 void RatAttackEXECUTE::StateCleanUp()
	 {
		 REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
		 REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
	 }

	 void RatAttackEXECUTE::StateExit(EntityID id)
	 {
		 RatScript::ToggleEntity(p_data->attackTelegraphID, false);
		 EntityManager::GetInstance().Get<Graphics::Renderer>(p_data->attackTelegraphID).SetEnabled(true);
		 m_hitCat = false;
	 }

	 void RatAttackEXECUTE::RatHitCat(const Event<CollisionEvents>& r_TE)
	 {
		 if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
		 {
			 OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
			 if ((OTEE.Entity1 == p_data->ratID || OTEE.Entity1 == p_data->attackTelegraphID) && EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
			 {
				 // if cat has been checked before check the next event
				 if (m_hitCat) { return; }

				 // save the id of the cat that has been checked so that it wont be checked again
				 m_hitCat = true;
				 try
				 {
					 auto& catFollowScript = *GETSCRIPTDATA(FollowScript, p_data->mainCatID);
					 // checks if this cat the rat or rat attack collided with is part of the chain
					 if (catFollowScript.NumberOfFollower >= 2)
					 {
						 if (std::find(catFollowScript.FollowingObject.begin(), catFollowScript.FollowingObject.end(), OTEE.Entity2) != catFollowScript.FollowingObject.end())
						 {
							 int const& damage = (OTEE.Entity1 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
							 try
							 {
								 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
							 } catch(...){}
						 }
					 }
					 // else checks if the cat hit was the main cat
					 else if (OTEE.Entity2 == p_data->mainCatID)
					 {
						 int const& damage = (OTEE.Entity1 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
						 try
						 {
							 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
						 }
						 catch (...) {}
					 }
				 }
				 catch(...){}
			 }
			 else if ((OTEE.Entity2 == p_data->ratID || OTEE.Entity2 == p_data->attackTelegraphID) && EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
			 {
				 // if cat has been checked before check the next event
				 if (m_hitCat) { return; }

				 // save the id of the cat that has been checked so that it wont be checked again
				 m_hitCat = true;
				 try
				 {
					 auto& catFollowScript = *GETSCRIPTDATA(FollowScript, p_data->mainCatID);
					 // checks if this cat the rat or rat attack collided with is part of the chain
					 if (catFollowScript.NumberOfFollower >= 2)
					 {
						 if (std::find(catFollowScript.FollowingObject.begin(), catFollowScript.FollowingObject.end(), OTEE.Entity1) != catFollowScript.FollowingObject.end())
						 {
							 int const& damage = (OTEE.Entity2 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
							 try
							 {
								 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
								 return;
							 } catch(...){}
						 }
					 }
					 // else checks if the cat hit was the main cat
					 if (OTEE.Entity1 == p_data->mainCatID)
					 {
						 int const& damage = (OTEE.Entity2 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
						 try
						 {
							 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
							 return;
						 }
						 catch (...) {}
					 }
				 }
				 catch(...){}
			 }
		 }
		 else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
		 {
			 OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
			 if ((OTSE.Entity1 == p_data->ratID || OTSE.Entity1 == p_data->attackTelegraphID) && EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity2).name.find("Cat") != std::string::npos)
			 {
				 // if cat has been checked before check the next event
				 if (m_hitCat) { return; }
				 
				 // save the id of the cat that has been checked so that it wont be checked again
				 m_hitCat = true;
				 try
				 {
					 auto& catFollowScript = *GETSCRIPTDATA(FollowScript, p_data->mainCatID);
					 // checks if this cat the rat or rat attack collided with is part of the chain
					 if (catFollowScript.NumberOfFollower >= 2)
					 {
						 if (std::find(catFollowScript.FollowingObject.begin(), catFollowScript.FollowingObject.end(), OTSE.Entity2) != catFollowScript.FollowingObject.end())
						 {
							 int const& damage = (OTSE.Entity1 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
							 try
							 {
								 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
							 }
							 catch (...) {}
						 }
					 }
					 // else checks if the cat his was the main cat
					 else if (OTSE.Entity2 == p_data->mainCatID)
					 {
						 int const& damage = (OTSE.Entity1 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
						 try
						 {
							 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
						 }
						 catch (...) {}
					 }
				 }
				 catch (...) {}
			 }
			 else if ((OTSE.Entity2 == p_data->ratID || OTSE.Entity2 == p_data->attackTelegraphID) && EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity1).name.find("Cat") != std::string::npos)
			 {
				 // if cat has been checked before check the next event
				 if (m_hitCat) { return; }

				 // save the id of the cat that has been checked so that it wont be checked again
				 m_hitCat = true;
				 try
				 {
					 auto& catFollowScript = *GETSCRIPTDATA(FollowScript, p_data->mainCatID);
					 // checks if this cat the rat or rat attack collided with is part of the chain
					 if (catFollowScript.NumberOfFollower >= 2)
					 {
						 if (std::find(catFollowScript.FollowingObject.begin(), catFollowScript.FollowingObject.end(), OTSE.Entity1) != catFollowScript.FollowingObject.end())
						 {
							 int const& damage = (OTSE.Entity2 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
							 try
							 {
								 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
							 }
							 catch (...) {}
						 }
					 }
					 // else checks if the cat hit was the main cat
					 else if (OTSE.Entity1 == p_data->mainCatID)
					 {
						 int const& damage = (OTSE.Entity2 == p_data->ratID) ? p_data->collisionDamage : p_data->attackDamage;
						 try
						 {
							 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(p_data->mainCatID, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
						 }
						 catch (...) {}
					 }
				 }
				 catch (...) {}
			 }
		 }
	 }
}