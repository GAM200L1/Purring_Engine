/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatAttackScript.h
 \date     15-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu
 \par      code %:     50%
 \par      changes:    Defined the cat script class and added functions for the 
											 player cat's attack states.

 \co-author            Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 \par      code %:     50%
 \par      changes:    21-11-2023
											 Added functions for the player cat's movement states.

 \brief
	This file contains declarations for functions used for a grey player cat's states.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

#include "prpch.h"
#include "CatScript.h"
#include "CatAttackScript.h"
#include "CatMovementScript.h"
#include "Data/SerializationManager.h"
#include "ResourceManager/ResourceManager.h"

namespace PE
{
	int CatScript::catEnergy{};  // Current energy level of the cat
	int CatScript::catMaxEnergy{ 21 }; // Maximum energy of the cat


	// ----- Destructor ----- //
	CatScript::~CatScript()
	{
			for (auto& [key, value] : m_scriptData)
			{
					delete value.p_stateManager;
			}
	}

	// ----- Public Functions ----- //
	void CatScript::Init(EntityID id)
	{
		m_scriptData[id].catID = id;
		// Make a statemanager and set the starting state
		MakeStateManager(id);

		//! Creates entities for the telegraph boxes
		CreateAttackTelegraphs(id, true, false); // east box
		CreateAttackTelegraphs(id, true, true); // west box
		CreateAttackTelegraphs(id, false, false); // north box
		CreateAttackTelegraphs(id, false, true); // south box

		//! Creates an entity for the projectile
		SerializationManager serializationManager;
		ResourceManager::GetInstance().LoadTextureFromFile("../Assets/Textures/Cat_Hairball_512px.png", "../Assets/Textures/Cat_Hairball_512px.png");
		m_scriptData[id].projectileID = serializationManager.LoadFromFile("../Assets/Prefabs/Projectile_Prefab.json");
		/*EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].projectileID).parent = id;*/
		EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].projectileID).isActive = false;
		// Create as many entities to visualise the player path nodes  
		// such that there is one node per energy level
		m_scriptData[id].pathPositions.reserve(CatScript::GetMaximumEnergyLevel());
		m_scriptData[id].pathQuads.reserve(CatScript::GetMaximumEnergyLevel());
		for (std::size_t i{}; i < CatScript::GetMaximumEnergyLevel(); ++i)
		{
			CreatePathNode(id);
		}
	}

	void CatScript::Update(EntityID id, float deltaTime)
	{
		if (GameStateManager::GetInstance().GetGameState() == GameStates::SPLASHSCREEN) { return; }

		/*if (GameStateManager::GetInstance().GetGameState() == GameStates::WIN) { return; }

		if (GameStateManager::GetInstance().GetGameState() == GameStates::LOSE) { return; }*/


		if (!m_scriptData[id].p_stateManager) 
		{
			m_scriptData[id].catID = id;

			// Make a statemanager and set the starting state
			MakeStateManager(id);
		}

		m_scriptData[id].p_stateManager->Update(id, deltaTime);


		if (m_scriptData[id].p_stateManager->GetStateName() == "MovementPLAN")
		{
			// if player is planning movement, set animation to idle(?)
			// Check if the state should be changed
			if (GameStateManager::GetInstance().GetGameState() == GameStates::ATTACK)
			{
				TriggerStateChange(id); // immediate state change
				if (CheckShouldStateChange(id, deltaTime))
				{
					m_scriptData[id].p_stateManager->ChangeState(new CatAttackPLAN{}, id);
				}
			}
		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "AttackPLAN")
		{
			// if player is planning attack, set animation to idle

			// Check if the state should be changed
			if (GameStateManager::GetInstance().GetGameState() == GameStates::EXECUTE)
			{
				TriggerStateChange(id); // immediate state change
				if (CheckShouldStateChange(id, deltaTime)) 
				{
					m_scriptData[id].p_stateManager->ChangeState(new CatMovementEXECUTE{}, id);
				}
			}
		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "MovementEXECUTE")
		{
			//if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<AnimationComponent>()))
			//{
			//	//EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationIndex("playerWalk");
			//}

			// Check if the state should be changed
			if (CheckShouldStateChange(id, deltaTime))
			{
				//m_scriptData[id].p_stateManager->ChangeState(new CatAttackPLAN{}, id); // --------------- @TODO KRYSTAL uncomment this
				//GameStateManager::GetInstance().IncrementGameState();

				// trigger state change called in MovementEXECUTE state update
				m_scriptData[id].p_stateManager->ChangeState(new CatAttackEXECUTE{}, id);
			}
		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "AttackEXECUTE")
		{
				//if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<AnimationComponent>()) && m_scriptData[id].attackDirection != 0)
				//{
				//	EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationIndex("playerAttack");
				//}

				// Check if the state should be changed
				if (CheckShouldStateChange(id, deltaTime))
				{
					//m_scriptData[id].p_stateManager->ChangeState(new CatAttackEXECUTE{}, id); --------------- @TODO KRYSTAL uncomment this
						
					// trigger state change called in AttackEXECUTE state update
					m_scriptData[id].p_stateManager->ChangeState(new CatMovementPLAN{}, id);
					GameStateManager::GetInstance().IncrementGameState();
				}
		}
	}

	void CatScript::OnAttach(EntityID id)
	{
		// check if the given entity has transform, rigidbody, and collider. if it does not, assign it one
		if (!EntityManager::GetInstance().Has<Transform>(id))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<Transform>() });
		}
		if (!EntityManager::GetInstance().Has<RigidBody>(id))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<RigidBody>() });
			EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
		}
		if (!EntityManager::GetInstance().Has<Collider>(id))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<Collider>() });
			EntityManager::GetInstance().Get<Collider>(id).colliderVariant = CircleCollider(); // cat default colliders is circle
		}

		if (m_scriptData.find(id) == m_scriptData.end())
		{
			m_scriptData[id] = CatScriptData{};
		}
		else
		{
			delete m_scriptData[id].p_stateManager;
			m_scriptData[id] = CatScriptData{};
		}

		m_scriptData[id].shouldChangeState = false;
		m_scriptData[id].timeBeforeChangingState = 0.f;

		// Set the cat max energy to the value set in the editor
		catMaxEnergy = m_scriptData[id].catMaxEnergy;
	}

	void CatScript::OnDetach(EntityID id)
	{
		if (m_scriptData.find(id) != m_scriptData.end())
		{
			delete m_scriptData[id].p_stateManager;
			m_scriptData.erase(id);
		}
	}		
		
		
	void CatScript::MakeStateManager(EntityID id)
	{
		if (m_scriptData[id].p_stateManager) { return; }

		m_scriptData[id].p_stateManager = new StateMachine{};
		m_scriptData[id].p_stateManager->ChangeState(new CatMovementPLAN{}, id);
	}


	void CatScript::TriggerStateChange(EntityID id, float const stateChangeDelay)
	{
		if (m_scriptData[id].delaySet) { return; }

		m_scriptData[id].shouldChangeState = true;
		m_scriptData[id].timeBeforeChangingState = stateChangeDelay;
		m_scriptData[id].delaySet = true;
	}	


	bool CatScript::CheckShouldStateChange(EntityID id, float const deltaTime)
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


	void CatScript::ToggleEntity(EntityID id, bool setToActive)
	{
			// Exit if the entity is not valid
			if (!EntityManager::GetInstance().IsEntityValid(id)) { return; }

		// Toggle the entity
		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = setToActive;
	}


	void CatScript::PositionEntity(EntityID const transformId, vec2 const& r_position)
	{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				r_transform.position = r_position;
			}
			catch (...) { return; }
	}	


	void CatScript::ScaleEntity(EntityID const transformId, float const width, float const height)
	{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				r_transform.width = width;
				r_transform.height = height;
			}
			catch (...) { return; }
	}	


	vec2 CatScript::GetEntityPosition(EntityID const transformId)
	{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				return r_transform.position;
			}
			catch (...) { return vec2{}; }
	}


	vec2 CatScript::GetEntityScale(EntityID const transformId)
	{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				return vec2{ r_transform.width, r_transform.height };
			}
			catch (...) { return vec2{}; }
	}


	void CatScript::CreateAttackTelegraphs(EntityID id, bool isXAxis, bool isNegative)
	{
		Transform const& catTransform = EntityManager::GetInstance().Get<Transform>(id);

		EntityID telegraphID = EntityFactory::GetInstance().CreateEntity<Transform, Collider, Graphics::Renderer>();
		Transform& telegraphTransform = EntityManager::GetInstance().Get<Transform>(telegraphID);

		EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).parent = id; // telegraph follows the cat entity
		EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).isActive = false; // telegraph to not show until attack planning

		// set size of telegraph
		telegraphTransform.height = catTransform.height * 0.75f;
		telegraphTransform.width = catTransform.width * m_scriptData[id].bulletRange;
		EnumCatAttackDirection dir;
		AABBCollider telegraphCollider;

		if (isXAxis)
		{
			telegraphTransform.relPosition.x = ((isNegative) ? -1.f : 1.f) * ((telegraphTransform.width * 0.5f) + (catTransform.width * 0.5f) + 10.f);
			dir = (isNegative) ? EnumCatAttackDirection::WEST : EnumCatAttackDirection::EAST;
		}
		else
		{
			telegraphTransform.relOrientation = PE_PI * 0.5f;
			telegraphTransform.relPosition.y = ((isNegative) ? -1.f : 1.f) * ((telegraphTransform.width * 0.5f) + (catTransform.width * 0.5f) + 10.f);

			telegraphCollider.scaleOffset.x = telegraphTransform.height / telegraphTransform.width;
			telegraphCollider.scaleOffset.y = telegraphTransform.width / telegraphTransform.height;

			dir = (isNegative) ? EnumCatAttackDirection::SOUTH : EnumCatAttackDirection::NORTH;
		}

		EntityManager::GetInstance().Get<Collider>(telegraphID).colliderVariant = telegraphCollider;
		EntityManager::GetInstance().Get<Collider>(telegraphID).isTrigger = true;

		// Load and Set the texture for the telegraphs
		std::string telegraphTextureName = "../Assets/Textures/Telegraphs/Telegraph_Long_512x128.png";
		ResourceManager::GetInstance().LoadTextureFromFile(telegraphTextureName, telegraphTextureName);
		EntityManager::GetInstance().Get<Graphics::Renderer>(telegraphID).SetTextureKey(telegraphTextureName);


		m_scriptData[id].telegraphIDs.emplace(dir, telegraphID);
	}

	void CatScript::CreatePathNode(EntityID id)
	{
		// create the east direction entity
		EntityID nodeId{ EntityFactory::GetInstance().CreateEntity<Transform, Graphics::Renderer>() };

		EntityManager::GetInstance().Get<Graphics::Renderer>(nodeId).SetColor(1.f, 1.f, 1.f, 1.f); // sets the color of the got to white

		EntityManager::GetInstance().Get<Transform>(nodeId).width = m_scriptData[id].nodeSize;
		EntityManager::GetInstance().Get<Transform>(nodeId).height = m_scriptData[id].nodeSize;
				
		EntityManager::GetInstance().Get<EntityDescriptor>(nodeId).isActive = false;

		m_scriptData[id].pathQuads.emplace_back(nodeId);

		std::cout << "CreatePathNode(" << id << ") created " << nodeId << "\n";
	}

}