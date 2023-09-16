/*!*****************************************************************************
	@file       Physics.cpp
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/

#include "Physics.h"
#include "../FrameRateTargetControl.h"
#include <sstream>

float frameTime{ 60.f }; 
// just putting a variable here to code without errors - to be replaced with actual fps

const vec2 UP_VECTOR{ 0.f, 1.f };

// ----- Getters/Setters ----- //
vec2 RigidBody::GetPosition() const
{
	return m_position;
}
void RigidBody::SetPosition(vec2 const& r_pos)
{
	m_position = r_pos;
}

float RigidBody::GetAngle() const
{
	return m_angle;
}
void RigidBody::SetAngleRad(float radAngle)
{
	m_angle = radAngle;
}
void RigidBody::SetAngleDeg(float degAngle)
{
	m_angle = ConvertDegToRad(degAngle);
}

// Calculates and saves the direction vector/angle based on the other
void RigidBody::ConvertAngleToDirection()
{
	mat3x3 rotMat{};
	rotMat.RotateRad(m_angle);
	m_direction = rotMat * m_direction;
}
void RigidBody::ConvertDirectionToAngle()
{
	m_angle = atan2(-UP_VECTOR.Dot(m_direction), -UP_VECTOR.Cross(m_direction)) + 180.f;
}

float RigidBody::GetMass() const
{
	return m_mass;
}
void RigidBody::SetMass(float mass)
{
	m_mass = mass;
}

vec2 RigidBody::GetLinearVelocity() const
{
	return m_linearVelocity;
}
void RigidBody::SetLinearVelocity(vec2 const& r_linearVelocity)
{
	if (m_type == EnumRigidBodyType::STATIC)
		return;

	if (r_linearVelocity.Dot(r_linearVelocity) > 0.f)
		SetAwake(true);

	m_linearVelocity = r_linearVelocity;
}

float RigidBody::GetAngularVelocity() const
{
	return m_angularVelocity;
}
void RigidBody::SetAngularVelocity(float angularVelocity)
{
	if (m_type == EnumRigidBodyType::STATIC)
		return;

	if (angularVelocity*angularVelocity > 0.f)
		SetAwake(true);

	m_angularVelocity = angularVelocity;
}

float RigidBody::GetLinearDamping() const
{
	return m_linearDamping;
}
void RigidBody::SetLinearDamping(float linearDamping)
{
	m_linearDamping = linearDamping;
}

float RigidBody::GetAngularDamping() const
{
	return m_angularDamping;
}
void RigidBody::SetAngularDamping(float angularDamping)
{
	m_angularDamping = angularDamping;
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
// Changes position of the object based on its velocity
void RigidBody::CalculatePosition()
{
	if (m_type == EnumRigidBodyType::STATIC || !m_awake)
		return;
	m_position += m_linearVelocity * frameTime;
}

// Changes the rotation of the object based on its velocity
void RigidBody::CalculateAngle()
{
	if (m_type == EnumRigidBodyType::STATIC || !m_awake)
		return;
	m_angle += m_angularVelocity * frameTime;
}

// Calculates the linear velocity of the objest from its force and mass
void RigidBody::CalculateLinearVelocity()
{
	if (m_type != EnumRigidBodyType::DYNAMIC || !m_awake)
		return;

	m_linearVelocity += (m_force / m_mass) * frameTime;
}

// Calculates the angular velocity from its torque and mass
void RigidBody::CalculateAngularVelocity()
{
	if (m_type != EnumRigidBodyType::DYNAMIC || !m_awake)
		return;

	m_angularVelocity += (m_torque / m_mass) * frameTime;
}

// Applies forces acting against RigidBody changing its m_force
void RigidBody::ApplyLinearDamping()
{
	if (m_type != EnumRigidBodyType::DYNAMIC || !m_awake)
		return;
	
	if (m_force.LengthSquared() <= 0.001f)
	{
		m_force.Zero();
		m_awake = false;
	}

	m_force *= m_linearDamping;
}

// Applies forces acting against RigidBody changing its m_torque
void RigidBody::ApplyRotationalDamping()
{
	if (m_type != EnumRigidBodyType::DYNAMIC || !m_awake)
		return;

	if (abs(m_torque) < 0.001f) // negligible
	{
		m_torque = 0.f;
		m_awake = false;
	}

	m_torque *= m_linearDamping;
}

// Adds on to existing force, ultimately affects position
void RigidBody::AddForce(vec2 const& r_addOnForce)
{
	if (m_type != EnumRigidBodyType::DYNAMIC)
		return;

	m_awake = true;
	m_force += r_addOnForce;
}

// Adds on to existing torque, ultimately affects rotation
void RigidBody::AddTorque(float r_addOnTorque)
{
	if (m_type != EnumRigidBodyType::DYNAMIC)
		return;

	m_awake = true;
	m_torque += r_addOnTorque;
}

// affects velocity directly
void RigidBody::ApplyLinearImpulse(vec2 const& r_impulseForce)
{
	if (m_type != EnumRigidBodyType::DYNAMIC)
		return;

	m_awake = true;
	vec2 acceleration = r_impulseForce / m_mass;
	m_linearVelocity += acceleration * m_mass;
}



std::string RigidBody::PrintDebug() const
{
	std::ostringstream oss;
	oss << "Position: " << m_position.x << ' ' << m_position.y << '\n';
	oss << "Direction: " << m_direction.x << ' ' << m_direction.x << '\n';
	oss << "Angle: " << m_angle << '\n';
	oss << "Mass: " << m_mass << '\n';
	oss << "Linear Velocity: " << m_linearVelocity.x << ' ' << m_linearVelocity.y << '\n';
	oss << "Angular Velocity: " << m_angularVelocity << '\n';
	oss << "Linear Damping: " << m_linearDamping << '\n';
	oss << "Angular Damping: " << m_angularDamping << '\n';
	oss << "Force: " << m_force.x << ' ' << m_force.y << '\n';
	oss << "Torque: " << m_torque << '\n';
	oss << "Awake: " << m_awake << '\n';
	return oss.str();
}

//void RigidBody::ApplyAngularImpulse(float impulse)
//{
//	if (m_type != EnumRigidBodyType::DYNAMIC)
//		return;
//
//	m_angularVelocity += impulse * m_mass;
//}