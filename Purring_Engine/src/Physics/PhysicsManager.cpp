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
#include "Logging/Logger.h"

extern Logger engine_logger;

namespace PE
{
	// ----- Constructor ----- //
	PhysicsManager::PhysicsManager() :
		m_linearDragCoefficient{ -2.f }, m_velocityNegligence{ 2.f },
		m_applyStepPhysics{ false }, m_advanceStep{ false }, m_fixedDt{ 1.f / 60.f } {}

	// ----- Public Getters ----- //
	float PhysicsManager::GetLinearDragCoefficient()
	{
		return m_linearDragCoefficient;
	}

	void PhysicsManager::SetLinearDragCoefficient(float newCoefficient)
	{
		m_linearDragCoefficient = (newCoefficient < 0.f) ? newCoefficient : -newCoefficient;
	}

	float PhysicsManager::GetVelocityNegligence()
	{
		return m_velocityNegligence;
	}
	void PhysicsManager::SetVelocityNegligence(float negligence)
	{
		m_velocityNegligence = negligence;
	}

	float PhysicsManager::GetFixedDt()
	{
		return m_fixedDt;
	}
	void PhysicsManager::SetFixedDt(float fixDt)
	{
		m_fixedDt = fixDt;
	}

	bool PhysicsManager::GetStepPhysics()
	{
		return m_applyStepPhysics;
	}
	void PhysicsManager::SetStepPhysics(bool isStepState)
	{
		m_applyStepPhysics = isStepState;
	}

	bool PhysicsManager::GetAdvanceStep()
	{
		return m_advanceStep;
	}
	void PhysicsManager::SetAdvanceStep(bool advance)
	{
		m_advanceStep = m_applyStepPhysics;
	}

	// ----- System Methods ----- //
	void PhysicsManager::InitializeSystem()
	{
		// Check if Initialization was successful
		engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
		engine_logger.SetTime();
		engine_logger.AddLog(false, "PhysicsManager initialized!", __FUNCTION__);
	}

	void PhysicsManager::UpdateSystem(float deltaTime)
	{
		if (!m_applyStepPhysics)
		{
			UpdateDynamics(deltaTime);
		}
		else
		{
			// Applies Step Physics
			if (m_advanceStep)
			{
				UpdateDynamics(deltaTime);
				m_advanceStep = false;
			}
		}
	}

	void PhysicsManager::DestroySystem()
	{
		// empty by design
	}

	// ----- Physics Methods ----- //

	void PhysicsManager::UpdateDynamics(float deltaTime)
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
				//Wrap(transform.orientation, -PE_PI, PE_PI);
			}
			rb.ZeroForce();
			rb.m_rotationVelocity = 0.f;
		}
	}
}