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
#include "../Rat/RatIdle_v2_0.h"

#include "../Physics/RigidBody.h"
#include "../Physics/Colliders.h"

namespace PE
{

		// ---------- FUNCTION DEFINITIONS ---------- //

		void RatScript_v2_0::Init(EntityID id)
		{
				CreateCheckStateManager(id);
		}


		void RatScript_v2_0::Update(EntityID id, float deltaTime)
		{
				CreateCheckStateManager(id);
				m_scriptData[id].p_stateManager->Update(id, deltaTime);
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
} // End of namespace PE