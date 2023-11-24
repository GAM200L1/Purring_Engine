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
		m_scriptData[id].pathPositions.reserve(m_scriptData[id].catMaxEnergy);
		m_scriptData[id].pathQuads.reserve(m_scriptData[id].catMaxEnergy);
		for (std::size_t i{}; i < m_scriptData[id].catMaxEnergy; ++i)
		{
			CreatePathNode(id);
		}
	}

	void CatScript::Update(EntityID id, float deltaTime)
	{
		if (!m_scriptData[id].p_stateManager) 
		{
			m_scriptData[id].catID = id;

			// Make a statemanager and set the starting state
			MakeStateManager(id);
		}

		m_scriptData[id].p_stateManager->Update(id, deltaTime);

		if (m_scriptData[id].p_stateManager->GetStateName() == "AttackEXECUTE")
		{
            if (EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<AnimationComponent>()) && m_scriptData[id].attackDirection != 0)
			{
				EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationIndex("playerAttack");
			}
        }
		else if (m_scriptData[id].p_stateManager->GetStateName() == "AttackPLAN")
		{
			// if player is planning attack, set animation to idle
			EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationIndex("playerWalk");
		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "MovementPLAN")
		{

		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "MovementEXECUTE")
		{

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
		m_scriptData[id].p_stateManager->ChangeState(new CatAttackPLAN{}, id);
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


	void CatScript::CreateAttackTelegraphs(EntityID id, bool isXAxis, bool isNegative)
	{
		// create the east direction entity
		EntityID telegraphID = EntityFactory::GetInstance().CreateEntity<Transform, Collider, Graphics::Renderer>();
		EntityManager::GetInstance().Get<Collider>(telegraphID).colliderVariant = AABBCollider();
		EntityManager::GetInstance().Get<Collider>(telegraphID).isTrigger = true;

		EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).parent = id;
		EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).isActive = false;

		vec2 boxPositionOffset{ 0.f,0.f };
		vec2 boxScaleOffset{ 1.f,1.f };
		EnumCatAttackDirection dir;

		if (isXAxis)
		{
			// set the scale of the selection box
				boxScaleOffset.y = 0.75f;
				boxScaleOffset.x = m_scriptData[id].bulletRange;
				EntityManager::GetInstance().Get<Transform>(telegraphID).width = EntityManager::GetInstance().Get<Transform>(id).width * boxScaleOffset.x;
				EntityManager::GetInstance().Get<Transform>(telegraphID).height = EntityManager::GetInstance().Get<Transform>(id).height * boxScaleOffset.y;


				boxPositionOffset.x = (EntityManager::GetInstance().Get<Transform>(id).width * 0.5f) + (EntityManager::GetInstance().Get<Transform>(telegraphID).width * 0.5f);
				boxPositionOffset.x *= (isNegative) ? -1 : 1;
				dir = (isNegative) ? EnumCatAttackDirection::WEST : EnumCatAttackDirection::EAST;
		}
		else
		{
			// set the scale of the selection box
				boxScaleOffset.x = 0.75f;
				boxScaleOffset.y = m_scriptData[id].bulletRange;
				EntityManager::GetInstance().Get<Transform>(telegraphID).width = EntityManager::GetInstance().Get<Transform>(id).width * boxScaleOffset.x;
				EntityManager::GetInstance().Get<Transform>(telegraphID).height = EntityManager::GetInstance().Get<Transform>(id).height * boxScaleOffset.y;


				boxPositionOffset.y = (EntityManager::GetInstance().Get<Transform>(id).width * 0.5f) + (EntityManager::GetInstance().Get<Transform>(telegraphID).height * 0.5f);
				boxPositionOffset.y *= (isNegative) ? -1 : 1;
				dir = (isNegative) ? EnumCatAttackDirection::SOUTH : EnumCatAttackDirection::NORTH;
		}

		// set the position of the selection box to be half the range away from the center of the cat
		EntityManager::GetInstance().Get<Transform>(telegraphID).relPosition.x = boxPositionOffset.x;
		EntityManager::GetInstance().Get<Transform>(telegraphID).relPosition.y = boxPositionOffset.y;

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

	void CatScript::ResetValues(EntityID id)
	{
		if (m_scriptData[id].p_stateManager->GetStateName() == "MovementPLAN")
		{

		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "AttackPLAN") 
		{

		}
	}

}