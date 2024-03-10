/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatDetectionScript_v2_0.cpp
 \date     17-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains definitions for functions to detect objects within 
	the detection range of the rat.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "RatDetectionScript_v2_0.h"
#include "RatScript_v2_0.h"

#include "../Math/MathCustom.h"
#include "../Physics/RigidBody.h"
#include "../Physics/Colliders.h"

namespace PE
{
	// ---------- FUNCTION DEFINITIONS ---------- //

	void RatDetectionScript_v2_0::SetParentRat(EntityID id, EntityID ratId)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end()){
			it->second.storedParentRat = true;
			it->second.mainRatID = ratId;
		} 
		else 
		{
			std::cout << "RatDetectionScript_v2_0::SetParentRat(): Failed to store parent rat\n";
		}
	}


	void RatDetectionScript_v2_0::SetDetectionRadius(EntityID id, float const newRadius)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			it->second.detectionRadius = newRadius;
			
			// Configure collider
			try
			{
				Collider& detectionCollider {EntityManager::GetInstance().Get<Collider>(id)};
				CircleCollider& circleCollider{ std::get<CircleCollider>(detectionCollider.colliderVariant) };
				circleCollider.scaleOffset = it->second.detectionRadius;
			} catch(...){ /* Empty */ }
		} 
		else 
		{
			std::cout << "RatDetectionScript_v2_0::SetDetectionRadius(): Failed to set rat detection radius\n";
		}
	}


	void RatDetectionScript_v2_0::Init(EntityID id)
	{	
		CreateCheckStateManager(id);
	}


	void RatDetectionScript_v2_0::Update(EntityID id, float deltaTime)
	{		
		/* Empty */
	}


	void RatDetectionScript_v2_0::OnAttach(EntityID id)
	{
		// Create script instance data
		m_scriptData[id] = RatDetectionScript_v2_0_Data();

		// Add dynamic rigidbody
		if (!EntityManager::GetInstance().Has<RigidBody>(id))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<RigidBody>() });
		}

		// Configure rigidbody
		try
		{
			RigidBody& detectionRigidbody {EntityManager::GetInstance().Get<RigidBody>(id)};
			detectionRigidbody.SetType(EnumRigidBodyType::DYNAMIC);
		} catch(...){ /* Empty */ }
			
		// Add circle collider
		if (!EntityManager::GetInstance().Has<Collider>(id))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<Collider>() });
		}
		
		// Configure collider
		try
		{
			Collider& detectionCollider {EntityManager::GetInstance().Get<Collider>(id)};
			detectionCollider.colliderVariant = CircleCollider();
			detectionCollider.isTrigger = true;
			detectionCollider.collisionLayerIndex = detectionColliderLayer; // @TODO To configure the collision matrix of the game scene

			CircleCollider& circleCollider{ std::get<CircleCollider>(detectionCollider.colliderVariant) };
			circleCollider.scaleOffset = m_scriptData[id].detectionRadius;
		} catch(...){ /* Empty */ }
	}


	void RatDetectionScript_v2_0::OnDetach(EntityID id)
	{
		// Delete this instance's script data
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			m_scriptData.erase(id);
		}
	}

	std::map<EntityID, RatDetectionScript_v2_0_Data>& RatDetectionScript_v2_0::GetScriptData()
	{
		return m_scriptData;
	}


	rttr::instance RatDetectionScript_v2_0::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}


	void RatDetectionScript_v2_0::CreateCheckStateManager(EntityID id)
	{
		if (m_scriptData.count(id))
		{
			if (m_scriptData.at(id).p_stateManager) { return; }

			m_scriptData[id].p_stateManager = new StateMachine{};
			m_scriptData[id].p_stateManager->ChangeState(new RatCollision_v2_0(), id);
		}
	}


	void RatCollision_v2_0::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(RatDetectionScript_v2_0, id);

		// Subscribe to events
		m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatCollision_v2_0::OnTriggerEnterAndStay, this);
		m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatCollision_v2_0::OnTriggerEnterAndStay, this);
		m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatCollision_v2_0::OnTriggerExit, this);
	}


	void RatCollision_v2_0::StateUpdate(EntityID id, float deltaTime)
	{
		// Can conduct state checks and call for state changes here
	}


	void RatCollision_v2_0::StateCleanUp()
	{
		// Unsubscribe from events
		REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
		REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
		REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);
	}


	void RatCollision_v2_0::StateExit(EntityID)
	{

	}


	void RatCollision_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
	{
		if (!p_data) { return; }
		else if (!(p_data->storedParentRat)) { return; }

		if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
		{
			OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
			// check if entity1 is the rat's detection collider and entity2 is cat
			if ((OTEE.Entity1 == p_data->myID) && RatScript_v2_0::GetIsCat(OTEE.Entity2))
			{
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->mainRatID, OTEE.Entity2);
			}
			// check if entity2 is the rat's detection collider and entity1 is cat
			else if ((OTEE.Entity2 == p_data->myID) && RatScript_v2_0::GetIsCat(OTEE.Entity1))
			{
					GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->mainRatID, OTEE.Entity1);
			}
		}
		else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
		{
			OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
			// check if entity1 is the rat's detection collider and entity2 is cat
			if ((OTSE.Entity1 == p_data->myID) && RatScript_v2_0::GetIsCat(OTSE.Entity2))
			{
				GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->mainRatID, OTSE.Entity2);
			}
			// check if entity2 is the rat's detection collider and entity1 is cat
			else if ((OTSE.Entity2 == p_data->myID) && RatScript_v2_0::GetIsCat(OTSE.Entity1))
			{
				GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->mainRatID, OTSE.Entity1);
			}
		}
	}

	void RatCollision_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
	{
		if (!p_data) { return; }
		else if (!(p_data->storedParentRat)) { return; }

		OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
		// check if entity1 is the rat's detection collider and entity2 is cat
		if ((OTEE.Entity1 == p_data->myID) && RatScript_v2_0::GetIsCat(OTEE.Entity2))
		{
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->mainRatID, OTEE.Entity2);
		}
		// check if entity2 is the rat's detection collider and entity1 is cat
		else if ((OTEE.Entity2 == p_data->myID) && RatScript_v2_0::GetIsCat(OTEE.Entity1))
		{
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->mainRatID, OTEE.Entity1);
		}
	}

} // End of namespace PE