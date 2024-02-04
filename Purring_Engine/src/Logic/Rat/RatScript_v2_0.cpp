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
						m_scriptData[id].p_stateManager->ChangeState(new RatTempSTATE_v2_0{}, id);
				}
		}
} // End of namespace PE