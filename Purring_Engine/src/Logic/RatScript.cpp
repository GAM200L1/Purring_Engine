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
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "Data/SerializationManager.h"
#include "Logic/LogicSystem.h"
#include "CatScript.h"
#include "FollowScript.h"
#include "Hierarchy/HierarchyManager.h"

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
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		m_scriptData[id].ratID = id;

		// create state manager for the entity
		if (m_scriptData[id].p_stateManager) { return; }

		if (p_gsc->currentState == GameStates_v2_0::PLANNING)
		{
			m_scriptData[id].p_stateManager = new StateMachine{};
			m_scriptData[id].p_stateManager->ChangeState(new RatIDLE{}, id);
		}

		CreateAttackTelegraphs(id);
	}
		 
	void RatScript::Update(EntityID id, float deltaTime)
	{
		if (p_gsc->currentState == GameStates_v2_0::WIN || p_gsc->currentState == GameStates_v2_0::LOSE)
		{
			ToggleEntity(m_scriptData[id].attackTelegraphID, false);
			return;
		}

		if (m_scriptData[id].health <= 0)
		{
			//GameStateManager::GetInstance().noPause = true;

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
					p_gsc->WinGame();
					SerializationManager serializationManager;
					EntityID sound = serializationManager.LoadFromFile("AudioObject/Rat Death SFX_Prefab.json");
					if (EntityManager::GetInstance().Has<AudioComponent>(sound))
						EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
					EntityManager::GetInstance().RemoveEntity(sound);
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

			if (p_gsc->currentState == GameStates_v2_0::PLANNING)
			{
				m_scriptData[id].p_stateManager = new StateMachine{};
				m_scriptData[id].p_stateManager->ChangeState(new RatIDLE{}, id);
			}
		}

		if (m_scriptData[id].p_stateManager)
		{
			m_scriptData[id].p_stateManager->Update(id, deltaTime);

			if (m_scriptData[id].p_stateManager->GetStateName() == "IDLE")
			{
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
				if (p_gsc->currentState == GameStates_v2_0::EXECUTE)
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
				if (p_gsc->currentState == GameStates_v2_0::PLANNING)
				{
					TriggerStateChange(id); // immediate state change
					if (CheckShouldStateChange(id, deltaTime))
					{
						m_scriptData[id].p_stateManager->ChangeState(new RatIDLE{}, id);
						m_scriptData[id].finishedExecution = false;
					}
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
		data.arrowTelegraphID = serializationManager.LoadFromFile("EnemyArrowTelegraph_Prefab.json");
		ToggleEntity(data.arrowTelegraphID, false); // set to inactive, it will only show during planning phase
		ScaleEntity(data.arrowTelegraphID, ratScale.x, ratScale.y);
		Hierarchy::GetInstance().AttachChild(data.psudoRatID, data.arrowTelegraphID); // attach child to parent
		EntityManager::GetInstance().Get<Transform>(data.arrowTelegraphID).relPosition.Zero();	  // zero out the position (attach calculates to stay in the same position in the world)
		EntityManager::GetInstance().Get<Transform>(data.arrowTelegraphID).relPosition.x = ratScale.x * data.detectionRadius * 0.5f;
		
		// create cross attack telegraph
		data.attackTelegraphID = serializationManager.LoadFromFile("EnemyAttackTelegraph_Prefab.json");
		ToggleEntity(data.attackTelegraphID, false); // set to inactive, it will only show during planning phase if the cat is in the area
		ScaleEntity(data.attackTelegraphID, data.attackDiameter, data.attackDiameter);

		// create the detection radius
		data.detectionTelegraphID = serializationManager.LoadFromFile("EnemyDetectionTelegraph_Prefab.json");
		ToggleEntity(data.detectionTelegraphID, false); // set to inactive it will only show during planning phase
		ScaleEntity(data.detectionTelegraphID, ratScale.x * 2.f * data.detectionRadius, ratScale.y * 2.f * data.detectionRadius);
		Hierarchy::GetInstance().AttachChild(id, data.detectionTelegraphID);
		EntityManager::GetInstance().Get<Transform>(data.detectionTelegraphID).relPosition = vec2{ 0.f,0.f }; // detection UI will always show
	}


	// ===== RatIDLE definition ===== //

	// ----- Public Functions ----- //
	void RatIDLE::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(RatScript, id);
		EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0);
		RatScript::PositionEntity(p_data->psudoRatID, EntityManager::GetInstance().Get<PE::Transform>(id).position);
		//RatScript::ToggleEntity(p_data->detectionTelegraphID, true); // show the detection telegraph
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
				//RatScript::ToggleEntity(p_data->arrowTelegraphID, true); // show the arrow of movement
			}
			
			// Ensure the rat is facing the direction of their movement
			vec2 newScale{ RatScript::GetEntityScale(id) };
			newScale.x = std::abs(newScale.x) * (((catObject.position - ratObject.position).Dot(vec2{1.f, 0.f}) >= 0.f) ? 1.f : -1.f); // Set the scale to negative if the rat is facing left
			RatScript::ScaleEntity(id, newScale.x, newScale.y);

			// settings for the attack cross
			RatScript::PositionEntity(p_data->attackTelegraphID, catObject.position);
			//RatScript::ToggleEntity(p_data->attackTelegraphID, true); // show the arrow of movement
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
		 p_data->attacking = (p_data->distanceFromPlayer > 0.f) ? true : false;
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
		 
	 }

	 void RatMovementEXECUTE::RatHitCat(const Event<CollisionEvents>& r_TE)
	 {
		 GETSCRIPTINSTANCEPOINTER(RatScript)->RatHitCat(p_data->ratID, r_TE);
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
		 if (m_delay > 0.f && p_data->attacking)
		 {
			 m_delay -= deltaTime;
		 }
		 else if (p_data->attacking)
		 {
			 RatScript::ToggleEntity(p_data->attackTelegraphID, true);
			 if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetCurrentFrameIndex() == EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationMaxIndex())
			 {
				 RatScript::ToggleEntity(p_data->attackTelegraphID, false);
				 p_data->finishedExecution = true;
			 }

			 if (!attacksoundonce) 
			 {
				 SerializationManager serializationManager;
				 EntityID sound = serializationManager.LoadFromFile("AudioObject/Rat Attack SFX_Prefab.json");
				 if (EntityManager::GetInstance().Has<AudioComponent>(sound))
					 EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
				 EntityManager::GetInstance().RemoveEntity(sound);

				 attacksoundonce = true;
			 }
		 }
		 else
		 {
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
		 p_data->hitCat = false;
		 
	 }

	 void RatAttackEXECUTE::RatHitCat(const Event<CollisionEvents>& r_TE)
	 {
		 GETSCRIPTINSTANCEPOINTER(RatScript)->RatHitCat(p_data->ratID, r_TE);
	 }

	 void RatScript::RatHitCat(EntityID id, const Event<CollisionEvents>& r_TE)
	 {
		// if cat has been checked before check the next event
			if (m_scriptData[id].hitCat) { return; }
		 if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
		 {
			 OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
			 // check if entity1 is rat or rat's attack and entity2 is cat
			 if ((OTEE.Entity1 == m_scriptData[id].ratID || OTEE.Entity1 == m_scriptData[id].attackTelegraphID) && 
			 		EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
			 {
				 GETSCRIPTINSTANCEPOINTER(RatScript)->CheckFollowOrMain(m_scriptData[id].mainCatID, OTEE.Entity2, OTEE.Entity1, id);
			 }
			 // check if entity2 is rat or rat's attack and entity1 is cat
			 else if ((OTEE.Entity2 == m_scriptData[id].ratID || OTEE.Entity2 == m_scriptData[id].attackTelegraphID) && 
			 			EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
			 {
				 // save the id of the cat that has been checked so that it wont be checked again
				 GETSCRIPTINSTANCEPOINTER(RatScript)->CheckFollowOrMain(m_scriptData[id].mainCatID, OTEE.Entity1, OTEE.Entity2, id);
			 }
		 }
		 else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
		 {
			 OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
			 // check if entity1 is rat or rat's attack and entity2 is cat
			 if ((OTSE.Entity1 == m_scriptData[id].ratID || OTSE.Entity1 == m_scriptData[id].attackTelegraphID) && 
			 		EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity2).name.find("Cat") != std::string::npos)
			 {
				 // save the id of the cat that has been checked so that it wont be checked again
				 GETSCRIPTINSTANCEPOINTER(RatScript)->CheckFollowOrMain(m_scriptData[id].mainCatID, OTSE.Entity2, OTSE.Entity1, id);
			 }
			 // check if entity2 is rat or rat's attack and entity1 is cat
			 else if ((OTSE.Entity2 == m_scriptData[id].ratID || OTSE.Entity2 == m_scriptData[id].attackTelegraphID) && 
			 			EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity1).name.find("Cat") != std::string::npos)
			 {
				 // save the id of the cat that has been checked so that it wont be checked again
				 GETSCRIPTINSTANCEPOINTER(RatScript)->CheckFollowOrMain(m_scriptData[id].mainCatID, OTSE.Entity1, OTSE.Entity2, id);
			 }
		 }
	 }

	 void RatScript::CheckFollowOrMain(EntityID mainCat, EntityID collidedCat, EntityID damagingID, EntityID rat)
	 {
		 try
		 {
			 auto& catFollowScript = *GETSCRIPTDATA(FollowScript, mainCat);
			 // checks hit cat is the main cat
			 if (collidedCat == mainCat)
			 {
				 int const& damage = (rat == m_scriptData[rat].ratID) ? m_scriptData[rat].collisionDamage : m_scriptData[rat].attackDamage;
				 try
				 {
					 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(mainCat, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
					 m_scriptData[rat].hitCat = true; // prevents double hits
				 }
				 catch (...) {}
			 }
			 // checks if hit cat is a following cat
			 else if (catFollowScript.NumberOfFollower >= 2)
			 {
				 if (std::find(catFollowScript.FollowingObject.begin(), catFollowScript.FollowingObject.end(), collidedCat) != catFollowScript.FollowingObject.end())
				 {
					 int const& damage = (damagingID == rat) ? m_scriptData[rat].collisionDamage : m_scriptData[rat].attackDamage;
					 try
					 {
						 GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(mainCat, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
						 m_scriptData[rat].hitCat = true; // prevents double hits
					 }
					 catch (...) {}
				 }
			 }		 
		 }
		 catch (...) {}
	 }
}