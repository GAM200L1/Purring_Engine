/*!*****************************************************************************
	@file       RigidBody.h
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#pragma once

#include "Math/MathCustom.h"
#include "ECS/Components.h"


enum class EnumRigidBodyType
{
	STATIC = 0, // Zero mass, zero velocity, not affected by force but position can be set -> during simulation, it won't move and it wont collide with anything
	DYNAMIC, // Has mass, velocity can be set, affected by forces
	KINEMATIC // Zero mass, velocity can be set, not affected by forces, can move by setting velocity
};

namespace PE
{
	class RigidBody // dynamic stuff
	{
	public:
		// ----- Public Variables ----- //

		vec2 m_velocity;
		float m_rotationVelocity;

		vec2 m_force;
		float m_torque;

		bool m_awake; // enables collision of object, true for awake

	public:
		// ----- Constructors ----- //

		RigidBody();
		~RigidBody() = default;
		
		RigidBody(RigidBody const& r_cpy);
		RigidBody& operator=(RigidBody const& r_cpy);

		// ----- Getters/Setters ----- //

		float GetMass() const;
		float GetInverseMass() const;
		void SetMass(float mass);

		vec2 GetVelocity() const;
		void SetVelocity(vec2 const& r_linearVelocity);

		float GetRotationVelocity() const;
		void SetRotationVelocity(float angularVelocity);

		vec2 GetForce() const;
		void SetForce(vec2 const& r_force);
		void ZeroForce();

		EnumRigidBodyType GetType() const;
		void SetType(EnumRigidBodyType flag);

		bool IsAwake() const;
		void SetAwake(bool flag);

		// ----- Public Methods ----- //

		// Adds on to existing force, + drag + gravity etc
		void ApplyForce(vec2 const& r_addOnForce);

		// Adds on to existing torque, ultimately affects rotation
		void ApplyTorque(float r_addOnTorque);

		// Applies impulse - directly adds to linear velocity
		void ApplyLinearImpulse(vec2 const& r_impulseForce);

	private:
		// ----- Private Variables ----- //
		EnumRigidBodyType m_type = EnumRigidBodyType::STATIC;

		float m_mass;
		float m_inverseMass;

		float m_drag;
		float m_rotationDrag;
	};

}


// ignore! remove before pulling!! //
/*
float GetLinearDamping() const;
void SetLinearDamping(float linearDamping);

float GetAngularDamping() const;
void SetAngularDamping(float angularDamping);

float GetTorque() const;
void SetTorque(float torque);
void ZeroTorque();

// Applies impulse - directly adds to angular velocity
void ApplyAngularImpulse(float impulse);

void PrintDebug() const;
*/