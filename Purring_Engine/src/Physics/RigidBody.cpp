/*!*****************************************************************************
	@file       RigidBody.cpp
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/

#include "RigidBody.h"
#include "Logging/Logger.h"
#include <sstream>

namespace PE
{
	const vec2 UP_VECTOR{ 0.f, 1.f };

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
		return m_velocity;
	}
	void RigidBody::SetVelocity(vec2 const& r_linearVelocity)
	{
		if (m_type == EnumRigidBodyType::STATIC)
			return;

		if (r_linearVelocity.Dot(r_linearVelocity) > 0.f)
			SetAwake(true);

		m_velocity = r_linearVelocity;
	}

	float RigidBody::GetRotationVelocity() const
	{
		return m_rotationVelocity;
	}
	void RigidBody::SetRotationVelocity(float angularVelocity)
	{
		if (m_type == EnumRigidBodyType::STATIC)
			return;

		if (angularVelocity * angularVelocity > 0.f)
			SetAwake(true);

		m_rotationVelocity = angularVelocity;
	}

	vec2 RigidBody::GetForce() const
	{
		return m_force;
	}
	void RigidBody::SetForce(vec2 const& r_force)
	{
		m_force += r_force;
	}
	void RigidBody::ZeroForce()
	{
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

		m_awake = true;
		m_force += r_addOnForce;
		m_force *= m_drag;
	}

	// Adds on to existing torque, ultimately affects rotation
	void RigidBody::ApplyTorque(float r_addOnTorque)
	{
		if (m_type != EnumRigidBodyType::DYNAMIC)
			return;

		m_awake = true;
		m_torque += r_addOnTorque;
		m_torque *= m_rotationDrag;
	}

	// affects velocity directly
	void RigidBody::ApplyLinearImpulse(vec2 const& r_impulseForce)
	{
		if (m_type != EnumRigidBodyType::DYNAMIC)
			return;

		m_awake = true;
		vec2 acceleration = r_impulseForce / m_mass;
		m_velocity += acceleration * m_mass;
	}
}

// ignore! remove before pulling!! //
/*
float RigidBody::GetTorque() const
{
	return m_torque;
}
void RigidBody::SetTorque(float torque)
{
	m_torque += torque;
}
void RigidBody::ZeroTorque()
{
	m_torque = 0.f;
}
void RigidBody::PrintDebug() const
{
	std::ostringstream oss;
	oss << "Position: " << m_position.x << ' ' << m_position.y << '\n';
	oss << "Direction: " << m_direction.x << ' ' << m_direction.x << '\n';
	oss << "Angle: " << m_angle << '\n';
	oss << "Mass: " << m_mass << '\n';
	oss << "Linear Velocity: " << m_linearVelocity.x << ' ' << m_linearVelocity.y << '\n';
	oss << "Angular Velocity: " << m_angularVelocity << '\n';
	oss << "Force: " << m_force.x << ' ' << m_force.y << '\n';
	oss << "Torque: " << m_torque << '\n';
	oss << "Awake: " << m_awake << '\n';

	engine_logger.AddLog(false, oss.str(), __FUNCTION__);

	oss.clear();
}
// Applies forces acting against RigidBody changing its m_force
void RigidBody::ApplyLinearDrag()
{
	if (m_type != EnumRigidBodyType::DYNAMIC || !m_awake)
		return;

	if (m_force.LengthSquared() <= 0.001f)
	{
		m_force.Zero();
		m_awake = false;
	}

	m_force *= m_linearDrag;
}

// Applies forces acting against RigidBody changing its m_torque
void RigidBody::ApplyRotationalDrag()
{
	if (m_type != EnumRigidBodyType::DYNAMIC || !m_awake)
		return;

	if (abs(m_torque) < 0.001f) // negligible
	{
		m_torque = 0.f;
		m_awake = false;
	}

	m_torque *= m_linearDrag;
}*/
