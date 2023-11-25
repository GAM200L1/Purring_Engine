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

namespace PE
{
	// ----- Destructor ----- //
	RatScript::~RatScript()
	{

	}

	// ----- Public Functions ----- //
	void RatScript::Init(EntityID id)
	{
		m_scriptData[id].ratID = id;

		// create state manager for the entity
		if (m_scriptData[id].p_stateManager) { return; }

		m_scriptData[id].p_stateManager = new StateMachine{};
		m_scriptData[id].p_stateManager->ChangeState(new RatIDLE{}, id);

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
			// return;
		}

		if (!m_scriptData[id].p_stateManager)
		{
			m_scriptData[id].ratID = id;

			// Make a statemanager and set the starting state
			if (m_scriptData[id].p_stateManager) { return; }

			m_scriptData[id].p_stateManager = new StateMachine{};
			m_scriptData[id].p_stateManager->ChangeState(new RatIDLE{}, id);
		}

		m_scriptData[id].p_stateManager->Update(id, deltaTime);

		if (m_scriptData[id].p_stateManager->GetStateName() == "IDLE")
		{
			// TODO ------------------------------------------------------------ //
			// Add function here to get rat to be in IDLE animation when player is planning Attack and movement
			// ----------------------------------------------------------------- //

			// If current gamestate is set to attack planning, change state to CatAttackPLAN
			if (GameStateManager::GetInstance().GetGameState() == GameStates::EXECUTE)
			{
				TriggerStateChange(id); // immediate state change
				if (CheckShouldStateChange(id, deltaTime))
				{
					m_scriptData[id].p_stateManager->ChangeState(new RatMovementEXECUTE{}, id);
				}
			}
		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "MovementEXECUTE")
		{
			// TODO ------------------------------------------------------------ //
			// Add function here to get rat to be in MOVEMENT animation when executing movement
			// ----------------------------------------------------------------- //

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

			// Check if the state should be changed
			if (CheckShouldStateChange(id, deltaTime))
			{
				// trigger state change called in AttackEXECUTE state update
				m_scriptData[id].p_stateManager->ChangeState(new RatIDLE{}, id);
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

		// create the arrow telegraph
		data.arrowTelegraphID = serializationManager.LoadFromFile("../Assets/Prefabs/RatArrowTelegraph_Prefab.json");
		ToggleEntity(data.arrowTelegraphID, false); // set to inactive, it will only show during planning phase
		EntityManager::GetInstance().Get<EntityDescriptor>(data.arrowTelegraphID).parent = data.psudoRatID;
		EntityManager::GetInstance().Get<Transform>(data.arrowTelegraphID).relPosition.x = ratScale.x * data.detectionRadius * 0.5f;
		
		// create cross attack telegraph
		data.attackTelegraphID = serializationManager.LoadFromFile("../Assets/Prefabs/RatAttackTelegraph_Prefab.json");
		ScaleEntity(data.attackTelegraphID, ratScale.x, ratScale.y);
		ToggleEntity(data.attackTelegraphID, false); // set to inactive, it will only show during planning phase if the cat is in the area
		EntityManager::GetInstance().Get<EntityDescriptor>(data.attackTelegraphID).parent = id;

		// create the detection radius
		data.detectionTelegraphID = serializationManager.LoadFromFile("../Assets/Prefabs/RatDetectionTelegraph_Prefab.json");
		ToggleEntity(data.attackTelegraphID, false); // set to inactive it will only show during planning phase
		ScaleEntity(data.detectionTelegraphID, ratScale.x * 2.f * data.detectionRadius, ratScale.y * 2.f * data.detectionRadius);
		EntityManager::GetInstance().Get<EntityDescriptor>(data.detectionTelegraphID).parent = id;
	}

}