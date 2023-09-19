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
	PhysicsManager* PhysicsManager::m_ptrInstance;
	float PhysicsManager::m_linearDragCoefficient = -2.f;

	// ----- Public Getters ----- //
	PhysicsManager* PhysicsManager::GetInstance()
	{
		if (!m_ptrInstance)
		{
			m_ptrInstance = new PhysicsManager();
		}
		return m_ptrInstance;
	}

	float PhysicsManager::GetLinearDragCoefficient()
	{
		return m_linearDragCoefficient;
	}

	void PhysicsManager::SetLinearDragCoefficient(float newCoefficient)
	{
		m_linearDragCoefficient = (newCoefficient < 0.f) ? newCoefficient : -newCoefficient;
	}

	// ----- Public Methods ----- //
	void PhysicsManager::UpdateDynamics(float deltaTime) // update forces, acceleration and velocity here
	{
		for (EntityID RigidBodyID : SceneView<RigidBody, Transform>())
		{
			if (RigidBodyID == 1)
				continue;
			RigidBody& rb = g_entityManager->Get<RigidBody>(RigidBodyID);
			Transform& transform = g_entityManager->Get<Transform>(RigidBodyID);

			if (rb.GetType() == EnumRigidBodyType::DYNAMIC)
			{
				// Quick dirty implementation of drag force
				rb.ApplyForce(rb.m_velocity * rb.GetMass() * m_linearDragCoefficient);
				rb.m_velocity += rb.m_force * rb.GetInverseMass() * deltaTime;

				// at negligible velocity, velocity will set to 0.f
				rb.m_velocity.x = (rb.m_velocity.x < 2.f && rb.m_velocity.x > -2.f) ? 0.f : rb.m_velocity.x;
				rb.m_velocity.y = (rb.m_velocity.y < 2.f && rb.m_velocity.y > -2.f) ? 0.f : rb.m_velocity.y;
			}
			//std::cout << rb.GetMass() << '\n';
			//std::cout << rb.m_velocity.x << ' ' << rb.m_velocity.y << '\n';
			if (rb.GetType() != EnumRigidBodyType::STATIC)
			{
				transform.position += rb.m_velocity * deltaTime;
				transform.angle += rb.m_rotationVelocity * deltaTime;
			}
			rb.ZeroForce();
			rb.m_rotationVelocity = 0.f;
			if (rb.GetType() == EnumRigidBodyType::KINEMATIC)
				rb.m_velocity *= 0.5f;
		}
	}
}