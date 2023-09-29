/*!*****************************************************************************
	@file       RigidBody.h
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       10/9/2023

	@brief		This file contains the declarations for the RigidBody struct and
				EnumRigidBodyStruct.
All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#pragma once

#include "Math/MathCustom.h"
#include "ECS/Components.h"
#include "Data/json.hpp"

enum class EnumRigidBodyType
{
	STATIC = 0, //! Zero mass, zero velocity, not affected by force but position can be set -> during simulation, it won't move and it wont collide with anything
	DYNAMIC,	//! Has mass, velocity can be set, affected by forces
	//KINEMATIC	//! Zero mass, velocity can be set, not affected by forces, can move by setting velocity
};

namespace PE
{
	/*!***********************************************************************************
	 \brief This class contains functions that apply physics to an entity.
	
	*************************************************************************************/
	class RigidBody
	{
	public:

		// ----- Public Variables ----- //

		vec2 prevPosition{}; // the position of the object with RigidBody in the previous frame is stored here
		
		vec2 velocity{};
		float rotationVelocity{};

		vec2 force{};

		//float m_torque{};

		//bool m_awake{}; //! enables collision of object, true for awake

	public:
		// ----- Constructors ----- //
		RigidBody();

		RigidBody(RigidBody const& r_cpy);
		RigidBody& operator=(RigidBody const& r_cpy);

	public:
		// ----- Getters/Setters ----- //

		float GetMass() const;
		float GetInverseMass() const;
		void SetMass(float mass);

		void ZeroForce();

		EnumRigidBodyType GetType() const;
		void SetType(EnumRigidBodyType flag);

		//bool IsAwake() const;
		//void SetAwake(bool flag);

	public:
		// ----- Public Methods ----- //

		// Adds on to existing force, + drag + gravity etc
		void ApplyForce(vec2 const& r_addOnForce);

		// Adds on to existing torque, ultimately affects rotation
		//void ApplyTorque(float r_addOnTorque);

		// Applies impulse - directly adds to linear velocity
		void ApplyLinearImpulse(vec2 const& r_impulseForce);

		// ----- Serialization ----- //

		// Serialization
		nlohmann::json ToJson() const
		{
			nlohmann::json j;
			j["type"] = static_cast<int>(GetType()); // right now its an enum
			j["mass"] = m_mass;

			return j;
		}

		// Deserialization
		static RigidBody FromJson(const nlohmann::json& j)
		{
			RigidBody rb;
			rb.SetType(static_cast<EnumRigidBodyType>(j["type"].get<int>())); // right now its an enum
			rb.m_mass = j["mass"];

			return rb;
		}

	private:
		// ----- Private Variables ----- //
		EnumRigidBodyType m_type = EnumRigidBodyType::STATIC;

		float m_mass{10.f};
		float m_inverseMass{1.f/10.f};

		//float m_drag;
		//float m_rotationDrag;
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