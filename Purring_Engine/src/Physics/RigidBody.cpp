/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     RigidBody.cpp
 \date     10-09-2023
 
 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu.sg
 
 \brief This function contains the declarations for the RigidBody class and EnumRigidBodyType enum class.
		Changes: Added in Serialization and Deserialization functions for RigidBody Component
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "RigidBody.h"
#include "Physics/PhysicsManager.h"

namespace PE
{
	// ------ RigidBody Class ----- //
	
	// ----- Constructors/Copy Assignment ------ //
	RigidBody::RigidBody() :
		velocity{ vec2{ 0.f, 0.f } }, rotationVelocity{ 0.f },
		force{ vec2{ 0.f, 0.f } }, m_linearDrag{ 2.f },
		m_mass{ 10.f }, m_inverseMass{ 1.f/10.f },
		prevPosition{ vec2{ 0.f, 0.f } }, m_type{ EnumRigidBodyType::STATIC } {}

	RigidBody::RigidBody(RigidBody const& r_cpy) :
		velocity{ r_cpy.velocity }, rotationVelocity{ r_cpy.rotationVelocity },
		force{ r_cpy.force }, m_linearDrag{ r_cpy.m_linearDrag },
		m_mass{ r_cpy.m_mass }, m_inverseMass{ r_cpy.m_inverseMass }, 
		prevPosition{ r_cpy.prevPosition }, m_type{ r_cpy.m_type } {}

	RigidBody& RigidBody::operator=(RigidBody const& r_cpy)
	{
		velocity = r_cpy.velocity;
		rotationVelocity = r_cpy.rotationVelocity;
		force = r_cpy.force;
		m_mass = r_cpy.m_mass;
		m_inverseMass = r_cpy.m_inverseMass;
		prevPosition = r_cpy.prevPosition;
		m_type = r_cpy.m_type;
		return *this;
	}

	// ----- Getters/Setters ----- //

	float RigidBody::GetMass() const
	{
		return m_mass;
	}
	float RigidBody::GetInverseMass() const
	{
		return m_inverseMass;
	}
	void RigidBody::SetMass(float mass)
	{
		m_mass = mass;
		m_inverseMass = 1.f / mass; // inverse mass can only be set through mass
	}

	float RigidBody::GetLinearDrag() const
	{
		return m_linearDrag;
	}

	void RigidBody::SetLinearDrag(float drag)
	{
		m_linearDrag = drag;
	}

	void RigidBody::ZeroForce()
	{
		if (m_type != EnumRigidBodyType::DYNAMIC)
			return;

		force = vec2{ 0.f, 0.f };
	}

	EnumRigidBodyType RigidBody::GetType() const
	{
		return m_type;
	}
	void RigidBody::SetType(EnumRigidBodyType newType)
	{
		m_type = newType;
	}

	// ----- Public Methods ----- //

	// Adds on to existing force, ultimately affects position
	void RigidBody::ApplyForce(vec2 const& r_addOnForce)
	{
		if (m_type != EnumRigidBodyType::DYNAMIC || (!PhysicsManager::GetAdvanceStep() && PhysicsManager::GetStepPhysics()))
			return;

		force += r_addOnForce;
	}

	// Adds on immediately to object's velocity for burst movement
	void RigidBody::ApplyLinearImpulse(vec2 const& r_impulseForce)
	{
		if (m_type != EnumRigidBodyType::DYNAMIC || (!PhysicsManager::GetAdvanceStep() && PhysicsManager::GetStepPhysics()))
			return;
		velocity += r_impulseForce * m_inverseMass;
	}
}
