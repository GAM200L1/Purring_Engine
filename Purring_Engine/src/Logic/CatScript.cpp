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
#include "ResourceManager/ResourceManager.h"

namespace PE
{

		// ----- Destructor ----- //
		CatScript::~CatScript()
		{
				for (auto& [key, value] : m_scriptData)
				{
						delete value.m_stateManager;
				}
		}

		// ----- Public Functions ----- //
		void CatScript::Init(EntityID id)
		{
				m_scriptData[id].m_stateManager = new StateMachine{};
				m_scriptData[id].m_stateManager->ChangeState(new CatAttackPLAN{}, id);

				ResetValues(id);

				//! Creates entities for the telegraph boxes
				CreateAttackTelegraphs(id, true, false); // east box
				CreateAttackTelegraphs(id, true, true); // west box
				CreateAttackTelegraphs(id, false, false); // north box
				CreateAttackTelegraphs(id, false, true); // south box
		}

		void CatScript::Update(EntityID id, float deltaTime)
		{
				m_scriptData[id].m_stateManager->Update(id, deltaTime);
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
						delete m_scriptData[id].m_stateManager;
						m_scriptData[id] = CatScriptData{};
				}

				// Create as many entities to visualise the player path nodes  
				// such that there is one node per energy level
				for (std::size_t i{}; i < m_scriptData[id].catMaxEnergy; ++i)
				{

				}
		}

		void CatScript::OnDetach(EntityID id)
		{

				// Delete the boxes created for attack selection
				for (auto const& telegraphID : m_scriptData[id].telegraphIDs)
				{
						EntityManager::GetInstance().RemoveEntity(telegraphID.second);
				}

				// Delete the entities created to visualise the path nodes
				for (EntityID const& nodeId : m_scriptData[id].pathQuads)
				{
						EntityManager::GetInstance().RemoveEntity(nodeId);
				}

				if (m_scriptData.find(id) != m_scriptData.end())
				{
						delete m_scriptData[id].m_stateManager;
						m_scriptData.erase(id);
				}
		}

		/*!***********************************************************************************
		 \brief Helper function to en/disables an entity.

		 \param[in] id EntityID of the entity to en/disable.
		 \param[in] setToActive Whether this entity should be set to active or inactive.
		*************************************************************************************/
		void CatScript::ToggleEntity(EntityID id, bool setToActive)
		{
				// Exit if the entity is not valid
				if (!EntityManager::GetInstance().IsEntityValid(id)) { return; }

				// Toggle the entity
				EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = setToActive;
		}

		
		/*!***********************************************************************************
		 \brief Helper function for creating entities which are for the cat's attack telegraphs

		 \param[in] id			EntityID of the cat which the telegraphs are for
		 \param[in] isXAxis		bool to check if the box is elongated by its x Axis
		 \param[in] isNegative	bool to check if the box is positioned on the west or south of the cat 
								if it is lengthen by its width or height respectively
		*************************************************************************************/
		void CatScript::CreateAttackTelegraphs(EntityID id, bool isXAxis, bool isNegative)
		{
				// create the east direction entity
				EntityID telegraphID = EntityFactory::GetInstance().CreateEntity<Transform, Collider, Graphics::Renderer>();
				EntityManager::GetInstance().Get<Collider>(telegraphID).colliderVariant = AABBCollider();
				EntityManager::GetInstance().Get<Collider>(telegraphID).isTrigger = true;

				EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).parent = id;
				EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).isActive = false;
				//EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).toSave = false;

				vec2 boxPositionOffset{ 0.f,0.f };
				vec2 boxScaleOffset{ 1.f,1.f };
				EnumCatAttackDirection dir;

				if (isXAxis)
				{
						boxScaleOffset.y = 0.75f;
						boxScaleOffset.x = m_scriptData[id].bulletRange;
						boxPositionOffset.x = (EntityManager::GetInstance().Get<Transform>(id).width * 0.5f) + (m_scriptData[id].bulletRange * 0.5f * EntityManager::GetInstance().Get<Transform>(telegraphID).width);
						boxPositionOffset.x *= (isNegative) ? -1 : 1;
						dir = (isNegative) ? EnumCatAttackDirection::WEST : EnumCatAttackDirection::EAST;
				}
				else
				{
						boxScaleOffset.x = 0.75f;
						boxScaleOffset.y = m_scriptData[id].bulletRange;
						boxPositionOffset.y = (EntityManager::GetInstance().Get<Transform>(id).height * 0.5f) + (m_scriptData[id].bulletRange * 0.5f * EntityManager::GetInstance().Get<Transform>(telegraphID).height);
						boxPositionOffset.y *= (isNegative) ? -1 : 1;
						dir = (isNegative) ? EnumCatAttackDirection::SOUTH : EnumCatAttackDirection::NORTH;
				}

				// set the position of the selection box to be half the range away from the center of the cat
				EntityManager::GetInstance().Get<Transform>(telegraphID).relPosition.x = boxPositionOffset.x;
				EntityManager::GetInstance().Get<Transform>(telegraphID).relPosition.y = boxPositionOffset.y;

				// set the scale of the selection box
				EntityManager::GetInstance().Get<Transform>(telegraphID).width = EntityManager::GetInstance().Get<Transform>(id).width * boxScaleOffset.x;
				EntityManager::GetInstance().Get<Transform>(telegraphID).height = EntityManager::GetInstance().Get<Transform>(id).height * boxScaleOffset.y;

				// Load and Set the texture for the telegraphs
				std::string telegraphTextureName = "../Assets/Textures/Telegraphs/Telegraph_Long_512x128.png";
				ResourceManager::GetInstance().LoadTextureFromFile(telegraphTextureName, telegraphTextureName);
				EntityManager::GetInstance().Get<Graphics::Renderer>(telegraphID).SetTextureKey(telegraphTextureName);


				m_scriptData[id].telegraphIDs.emplace(dir, telegraphID);
		}


		/*!***********************************************************************************
		 \brief Creates a path node to visualise the path drawn by the player.

		 \param[in] id EntityID of the entity that this script is attached to.
		*************************************************************************************/
		void CatScript::CreatePathNode(EntityID id)
		{

		}


		/*!***********************************************************************************
		 \brief Calls the reset function of the movement or attack planning states.

		 \param[in] id EntityID of the entity that this script is attached to.
		*************************************************************************************/
		void CatScript::ResetValues(EntityID id)
		{
				if (m_scriptData[id].m_stateManager->GetStateName() == "MovementPLAN")
				{

				}
				else if (m_scriptData[id].m_stateManager->GetStateName() == "AttackPLAN") 
				{

				}
		}

}