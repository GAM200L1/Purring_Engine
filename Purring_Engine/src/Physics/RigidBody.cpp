/*!*****************************************************************************
	@file       RigidBody.cpp
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       10/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#include "prpch.h"
#include "RigidBody.h"
#include "Logging/Logger.h"
#include <sstream>

namespace PE
{
	RigidBody::RigidBody() :
		m_velocity{ vec2{ 0.f, 0.f } }, m_rotationVelocity{ 0.f },
		m_force{ vec2{ 0.f, 0.f } }, m_torque{ 0.f }, m_awake{ false },
		m_mass{ 0.f }, m_inverseMass{ 0.f }, m_drag{ 0.f }, m_rotationDrag{ 0.f } {}

	RigidBody::RigidBody(RigidBody const& r_cpy) :
		m_velocity{ r_cpy.m_velocity }, m_rotationVelocity{ r_cpy.m_rotationVelocity },
		m_force{ r_cpy.m_force }, m_torque{ r_cpy.m_torque }, m_awake{ r_cpy.m_awake },
		m_mass{ r_cpy.m_mass }, m_inverseMass{ r_cpy.m_inverseMass }, m_drag{ r_cpy.m_drag }, m_rotationDrag{ r_cpy.m_rotationDrag } {}

	RigidBody& RigidBody::operator=(RigidBody const& r_cpy)
	{
		m_velocity = r_cpy.m_velocity;
		m_rotationVelocity = r_cpy.m_rotationVelocity;
		m_force = r_cpy.m_force;
		m_torque = r_cpy.m_torque;
		m_awake = r_cpy.m_awake;
		m_mass = r_cpy.m_mass;
		m_inverseMass = r_cpy.m_inverseMass;

		m_drag = r_cpy.m_drag;
		m_rotationDrag = r_cpy.m_rotationDrag;
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
		m_inverseMass = 1.f / mass;
	}

	vec2 RigidBody::GetVelocity() const
	{
		if (m_type == EnumRigidBodyType::STATIC)
			return vec2{};

		return m_velocity;
	}
	void RigidBody::SetVelocity(vec2 const& r_linearVelocity)
	{
		if (m_type == EnumRigidBodyType::STATIC)
			return;

		m_velocity = r_linearVelocity;
	}

	float RigidBody::GetRotationVelocity() const
	{
		if (m_type == EnumRigidBodyType::STATIC)
			return 0.f;

		return m_rotationVelocity;
	}
	void RigidBody::SetRotationVelocity(float angularVelocity)
	{
		if (m_type == EnumRigidBodyType::STATIC)
			return;

		m_rotationVelocity = angularVelocity;
	}

	vec2 RigidBody::GetForce() const
	{
		if (m_type != EnumRigidBodyType::DYNAMIC)
			return vec2{};

		return m_force;
	}
	void RigidBody::SetForce(vec2 const& r_force)
	{
		if (m_type != EnumRigidBodyType::DYNAMIC)
			return; // should change to throw

		m_force = r_force;
	}
	void RigidBody::ZeroForce()
	{
		if (m_type != EnumRigidBodyType::DYNAMIC)
			return;

		m_force = vec2{ 0.f, 0.f };
	}

	EnumRigidBodyType RigidBody::GetType() const
	{
		return m_type;
	}
	void RigidBody::SetType(EnumRigidBodyType flag)
	{
		m_type = flag;
	}

	bool RigidBody::IsAwake() const
	{
		return (m_awake) ? true : false;
	}
	void RigidBody::SetAwake(bool flag)
	{
		m_awake = flag;
	}

	// ----- Public Methods ----- //
	// Adds on to existing force, ultimately affects position
	void RigidBody::ApplyForce(vec2 const& r_addOnForce)
	{
		if (m_type != EnumRigidBodyType::DYNAMIC)
			return;

		m_force += r_addOnForce;
	}

	// Adds on to existing torque, ultimately affects rotation
	void RigidBody::ApplyTorque(float r_addOnTorque)
	{
		if (m_type != EnumRigidBodyType::DYNAMIC)
			return;

		m_torque += r_addOnTorque;
	}

	void RigidBody::ApplyLinearImpulse(vec2 const& r_impulseForce)
	{
		if (m_type != EnumRigidBodyType::DYNAMIC)
			return;
		m_velocity += r_impulseForce * m_inverseMass;
	}
}
