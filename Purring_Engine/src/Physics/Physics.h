/*!*****************************************************************************
	@file       Physics.h
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

	@brief
	
All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#pragma once
#include "../MathCustom.h"
#include "../Entity/Components.h"
#include <string>

enum class EnumRigidBodyType
{
	STATIC = 0, // Zero mass, zero velocity, not affected by force but position can be set -> during simulation, it won't move and it wont collide with anything
	DYNAMIC, // Has mass, velocity can be set, affected by forces
	KINEMATIC // Zero mass, velocity can be set, not affected by forces, can move by setting velocity
};


// singleton class that will manage/update the physics of all entities with a physics component
//class PhysicsManager
//{
//public:
//	PhysicsManager();
//	~PhysicsManager();
//
//	PhysicsManager(PhysicsManager const& r_cpy) = delete;
//	PhysicsManager& operator=(PhysicsManager const& r_cpy) = delete;
//
//	PhysicsManager* GetInstance();
//
//	// placeholder - these are to be run in the engine
//	void Load();
//	void Init();
//	void Update();
//	void Unload();
//
//private:
//	PhysicsManager* m_instancePtr;
//};

class RigidBody : public Component // dynamic stuff
{
public:

	RigidBody() : m_position{ 0.f , 0.f }, m_direction{ 0.f, 0.f }, 
				  m_angle{ 0.f }, m_mass{ 0.f }, 
				  m_linearVelocity{ 0.f, 0.f }, m_angularVelocity{ 0.f }, 
				  m_linearDamping{ 0.f }, m_angularDamping{ 0.f }, 
				  m_force{ 0.f, 0.f }, m_torque{ 0.f }, m_awake{ false } {}
	~RigidBody() = default;
	RigidBody(RigidBody const& r_cpy) : m_position{ r_cpy.m_position }, m_direction{ r_cpy.m_direction },
										m_angle{ r_cpy.m_angle }, m_mass{ r_cpy.m_mass },
										m_linearVelocity{ r_cpy.m_linearVelocity }, m_angularVelocity{ r_cpy.m_angularVelocity },
										m_linearDamping{ r_cpy.m_linearDamping }, m_angularDamping{ r_cpy.m_angularDamping },
										m_force{ r_cpy.m_force }, m_torque{ r_cpy.m_torque }, m_awake{ r_cpy.m_awake } {}

	RigidBody& operator=(RigidBody const& r_cpy) = default;
	
	// ----- Getters/Setters ----- //
	vec2 GetPosition() const;
	void SetPosition(vec2 const& r_pos);

	float GetAngle() const;
	void SetAngleRad(float radAngle);
	void SetAngleDeg(float degAngle);

	// Calculates and saves the direction vector/angle based on the other
	void ConvertAngleToDirection();
	void ConvertDirectionToAngle();
	
	float GetMass() const;
	void SetMass(float mass);
	
	vec2 GetLinearVelocity() const;
	void SetLinearVelocity(vec2 const& r_linearVelocity);

	float GetAngularVelocity() const;
	void SetAngularVelocity(float angularVelocity);

	float GetLinearDamping() const;
	void SetLinearDamping(float linearDamping);

	float GetAngularDamping() const;
	void SetAngularDamping(float angularDamping);

	vec2 GetForce() const;
	void SetForce(vec2 const& r_force);
	void ZeroForce();

	float GetTorque() const;
	void SetTorque(float torque);
	void ZeroTorque();

	EnumRigidBodyType GetType() const;
	void SetType(EnumRigidBodyType flag);

	bool IsAwake() const;
	void SetAwake(bool flag);

	// ----- Public Methods ----- //
	// Changes position of the object based on its velocity
	void CalculatePosition();

	// Changes the rotation of the object based on its velocity
	void CalculateAngle();

	// Calculates the linear velocity of the objest from its force and mass
	void CalculateLinearVelocity();

	// Calculates the angular velocity from its torque and mass
	void CalculateAngularVelocity();

	// Applies forces acting against RigidBody changing its m_force
	void ApplyLinearDamping();

	// Applies forces acting against RigidBody changing its m_torque
	void ApplyRotationalDamping();

	// Adds on to existing force, ultimately affects position
	void AddForce(vec2 const& r_addOnForce);

	// Adds on to existing torque, ultimately affects rotation
	void AddTorque(float r_addOnTorque);

	// Applies impulse - directly adds to linear velocity
	void ApplyLinearImpulse(vec2 const& r_impulseForce);

	// Applies impulse - directly adds to angular velocity
	// void ApplyAngularImpulse(float impulse);

	std::string PrintDebug() const;

private:
	
	EnumRigidBodyType m_type = EnumRigidBodyType::STATIC;

	vec2 m_position;
	vec2 m_direction;
	float m_angle; // rad

	float m_mass; // eg. kg
	
	vec2 m_linearVelocity; // eg. m/s, m/s
	float m_angularVelocity; // eg. rad/s
	
	float m_linearDamping; // 0 - 1 value => 0 will completely stop the object
	float m_angularDamping; // 0 - 1 value => 0 will completely stop the object
	
	vec2 m_force; // eg. kg * m/s^2
	float m_torque;
	
	bool m_awake; // enables collision of object, true for awake

	// consider bitwise int to check variables that are active
};

