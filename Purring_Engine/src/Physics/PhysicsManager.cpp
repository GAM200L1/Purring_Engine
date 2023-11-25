/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     PhysicsManager.cpp
 \date     10-09-2023
 
 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu
 
 \brief    Contains definition of PhysicsManager's member functions
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "PhysicsManager.h"
#include "ECS/Entity.h"
#include "ECS/SceneView.h"
#include "Math/Transform.h"
#include "RigidBody.h"
#include "Logging/Logger.h"

#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif

extern Logger engine_logger;

namespace PE
{
	// ----- Static Declarations ----- //

	bool PhysicsManager::m_applyStepPhysics{ false };
	bool PhysicsManager::m_advanceStep{ false };

	// ----- Constructor ----- //

	PhysicsManager::PhysicsManager() :
		m_velocityNegligence{ 2.f } {} 
	
	// ----- Public Getters ----- //

	float PhysicsManager::GetVelocityNegligence()
	{
		return m_velocityNegligence;
	}
	void PhysicsManager::SetVelocityNegligence(float negligence)
	{
		m_velocityNegligence = negligence;
	}

	bool& PhysicsManager::GetStepPhysics()
	{
		return m_applyStepPhysics;
	}

	bool& PhysicsManager::GetAdvanceStep()
	{
		return m_advanceStep;
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
#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
		{
#endif
			// In normal physics simulation mode
			if (!m_applyStepPhysics)
			{
				UpdateDynamics(deltaTime);
			}
#ifndef GAMERELEASE
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
#endif

#ifndef GAMERELEASE
		if (!Editor::GetInstance().IsRunTime())
		{
			for (EntityID RigidBodyID : SceneView<RigidBody, Transform>())
			{
				// if the entity is not active, do not update physics
				if (!EntityManager::GetInstance().Get<EntityDescriptor>(RigidBodyID).isActive) { continue; }

				RigidBody& rb = EntityManager::GetInstance().Get<RigidBody>(RigidBodyID);
				rb.ZeroForce();
				rb.velocity.Zero();
				rb.rotationVelocity = 0.f;
			}
		}
#endif
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
			// if the entity is not active, do not check for collision
			if (!EntityManager::GetInstance().Get<EntityDescriptor>(RigidBodyID).isActive) { continue; }
			
			RigidBody& rb = EntityManager::GetInstance().Get<RigidBody>(RigidBodyID);
			Transform& transform = EntityManager::GetInstance().Get<Transform>(RigidBodyID);

			if (rb.GetType() == EnumRigidBodyType::DYNAMIC)
			{
				// Applies drag force
				rb.ApplyForce(rb.velocity * rb.GetMass() * rb.GetLinearDrag() * -1.f);
				
				// Update Speed based on total forces
				rb.velocity += rb.force * rb.GetInverseMass() * deltaTime;
			}

			if (rb.GetType() != EnumRigidBodyType::STATIC)
			{
				// at negligible velocity, velocity will set to 0.f
				rb.velocity.x = (rb.velocity.x < m_velocityNegligence && rb.velocity.x > -m_velocityNegligence) ? 0.f : rb.velocity.x;
				rb.velocity.y = (rb.velocity.y < m_velocityNegligence && rb.velocity.y > -m_velocityNegligence) ? 0.f : rb.velocity.y;
				rb.prevPosition = transform.position;
				transform.position += rb.velocity * deltaTime;
				transform.orientation += rb.rotationVelocity * deltaTime;
				Wrap(transform.orientation, 0.f, 2.f * PE_PI);
			}
			rb.ZeroForce();
			rb.rotationVelocity = 0.f;
		}
	}
}