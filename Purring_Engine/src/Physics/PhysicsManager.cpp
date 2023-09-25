/*!*****************************************************************************
	@file       PhysicsManager.cpp
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       10/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#include "prpch.h"
#include "PhysicsManager.h"
#include "ECS/Entity.h"
#include "ECS/SceneView.h"
#include "Math/Transform.h"
#include "RigidBody.h"

namespace PE
{
	PhysicsManager* PhysicsManager::p_instance;
	float PhysicsManager::m_linearDragCoefficient = -2.f;
	float PhysicsManager::m_velocityNegligence = 2.f;
	bool PhysicsManager::applyStepPhysics = false;
	bool PhysicsManager::advanceStep = false;

	// ----- Public Getters ----- //
	PhysicsManager* PhysicsManager::GetInstance()
	{
		if (!p_instance)
		{
			p_instance = new PhysicsManager();
		}
		return p_instance;
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
	void PhysicsManager::Step(float deltaTime)
	{
		if (!applyStepPhysics)
			UpdateDynamics(deltaTime);
		else
		{
			if (advanceStep)
			{
				UpdateDynamics(deltaTime);
				advanceStep = false;
			}
		}
	}

	void PhysicsManager::UpdateDynamics(float deltaTime) // update forces, acceleration and velocity here
	{
		for (EntityID RigidBodyID : SceneView<RigidBody, Transform>())
		{
			RigidBody& rb = g_entityManager->Get<RigidBody>(RigidBodyID);
			Transform& transform = g_entityManager->Get<Transform>(RigidBodyID);

			if (rb.GetType() == EnumRigidBodyType::DYNAMIC)
			{
				// Quick dirty implementation of drag force
				rb.ApplyForce(rb.m_velocity * rb.GetMass() * m_linearDragCoefficient);
				
				// Update Speed based on total forces
				rb.m_velocity += rb.m_force * rb.GetInverseMass() * deltaTime;

				// at negligible velocity, velocity will set to 0.f
				rb.m_velocity.x = (rb.m_velocity.x < m_velocityNegligence && rb.m_velocity.x > -m_velocityNegligence) ? 0.f : rb.m_velocity.x;
				rb.m_velocity.y = (rb.m_velocity.y < m_velocityNegligence && rb.m_velocity.y > -m_velocityNegligence) ? 0.f : rb.m_velocity.y;
			}

			if (rb.GetType() != EnumRigidBodyType::STATIC)
			{
				rb.m_prevPosition = transform.position;
				transform.position += rb.m_velocity * deltaTime;
				transform.orientation += rb.m_rotationVelocity * deltaTime;
				Wrap(transform.orientation, 0.f, 2.f*PE_PI);
			}
			rb.ZeroForce();
			rb.m_rotationVelocity = 0.f;
		}
	}

	void PhysicsManager::DeleteInstance()
	{
		delete p_instance;
		p_instance = nullptr;
	}
}