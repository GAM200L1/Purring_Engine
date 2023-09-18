/*!*****************************************************************************
	@file       PhysicsManager.cpp
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#include "prpch.h"
#include "PhysicsManager.h"
#include "FrameRateTargetControl.h"
#include "ECS/Entity.h"
#include "ECS/SceneView.h"
#include "RigidBody.h"

namespace PE
{
	// ----- Public Getters ----- //
	PhysicsManager* PhysicsManager::GetInstance()
	{
		if (!m_ptrInstance)
		{
			m_ptrInstance = new PhysicsManager();
		}
		return m_ptrInstance;
	}
	
	// ----- Public Methods ----- //
	void PhysicsManager::UpdateDynamics(float deltaTime) // update forces, acceleration and velocity here
	{
		for (EntityID rigidBodyID : SceneView<RigidBody>())
		{
			RigidBody& rb = g_entityManager->Get<RigidBody>(rigidBodyID);

			//rb.ApplyForce(); // probably apply the friction forces here
			//rb.ApplyTorque(); // not sure yet
			//rb.ApplyLinearImpulse() // for dashes etc.

			if (rb.m_awake) // object does not need to be moved
				rb.m_velocity += rb.m_force * rb.GetInverseMass() * deltaTime;
		}
	}

	void PhysicsManager::UpdatePositions(float deltaTime) // update positions here
	{
		for (EntityID objectID : SceneView<RigidBody, Transform>())
		{
			RigidBody const &rb = g_entityManager->Get<RigidBody>(objectID);
			
			if (!rb.m_awake) // object does not need to be moved
			{ continue; }
			
			vec2& objPos = g_entityManager->Get<Transform>(objectID).position;
			objPos += rb.m_velocity * deltaTime;
		}
	}
}