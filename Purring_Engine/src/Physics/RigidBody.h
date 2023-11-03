/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     RigidBody.h
 \date     10-09-2023
 
 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu
 \par      code %:     95%
 
 \co-author            Hans Ong You Yang
 \par      email:      youyang.o/@digipen.edu
 \par      code %:     5%
 \par      changes:    Added Serialization and Deserialization functions for RigidBody Component
 
 \brief This function contains the declarations for the RigidBody class and EnumRigidBodyType enum class.
		Changes: Added in Serialization and Deserialization functions for RigidBody Component
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include "Math/MathCustom.h"
#include "ECS/Components.h"
#include "Data/json.hpp"

// Used to denote whether RigidBody should be allowed to move with forces/velocity or not
enum class EnumRigidBodyType
{
	STATIC = 0, //! Zero mass, zero velocity, not affected by force but position can be set -> during simulation, it won't move and it wont collide with anything
	DYNAMIC,	//! Has mass, velocity can be set, affected by forces
};

namespace PE
{
	/*!***********************************************************************************
	 \brief This class contains functions that apply physics to an entity.
	
	*************************************************************************************/
	class RigidBody
	{
		// ----- Public Variables ----- //
		public:

		vec2 prevPosition{}; // the position of the object with RigidBody in the previous frame is stored here
		vec2 velocity{};
		float rotationVelocity{};
		vec2 force{};

		// ----- Constructors ----- //
		public:
		
		/*!***********************************************************************************
		 \brief Default constructor for a new Rigid Body object. All variables are set to 0.f
		 		except for m_mass and m_inverseMass which are set as 10.f and 1.f/10.f respectively.
		 
		*************************************************************************************/
		RigidBody();

		/*!***********************************************************************************
		 \brief Copy construct a new Rigid Body object
		 
		 \param[in,out] r_cpy - RigidBody which variables are to be copied to the newly constructed
		 						RigidBody
		*************************************************************************************/
		RigidBody(RigidBody const& r_cpy);

		/*!***********************************************************************************
		 \brief Copy Assignment operator for RigidBody
		 
		 \param[in,out] r_cpy - RigidBody which variables are to be copied into the existing RigidBody.
		 \return RigidBody& - Reference to the RigidBody which is being modified to copy r_cpy.
		*************************************************************************************/
		RigidBody& operator=(RigidBody const& r_cpy);

	public:
		// ----- Getters/Setters ----- //

		/*!***********************************************************************************
		 \brief Get the Mass of object
		 
		 \return float - m_mass variable of the RigidBody
		*************************************************************************************/
		float GetMass() const;

		/*!***********************************************************************************
		 \brief Get the Inverse Mass of object
		 
		 \return float - m_inverseMass variable of the RigidBody
		*************************************************************************************/
		float GetInverseMass() const;

		float GetLinearDrag() const;

		/*!***********************************************************************************
		 \brief Set the Mass and Inverse Mass of object
		 
		 \param[in,out] mass - new mass of the object
		*************************************************************************************/
		void SetMass(float mass);

		void SetLinearDrag(float drag);

		/*!***********************************************************************************
		 \brief Set 'force' of RigidBody to zero vec2
		 
		*************************************************************************************/
		void ZeroForce();

		/*!***********************************************************************************
		 \brief Get the RigidBodyType of the object
		 
		 \return EnumRigidBodyType - The RigidBodyType of the object
		*************************************************************************************/
		EnumRigidBodyType GetType() const;

		/*!***********************************************************************************
		 \brief Set the RigidBodyType of the object
		 
		 \param[in,out] newType - The RigidBodyType of the object
		*************************************************************************************/
		void SetType(EnumRigidBodyType newType);

	public:
		// ----- Public Methods ----- //

		/*!***********************************************************************************
		 \brief Adds on to objects existing force. Force will be used to calculate objects
		 		acceleration.
		 
		 \param[in,out] r_addOnForce - force to add on to the objects 'force'
		*************************************************************************************/
		void ApplyForce(vec2 const& r_addOnForce);

		/*!***********************************************************************************
		 \brief Applies impulse to the object, directly adding on to 'velocity', allowing
		 		burst movement
		 
		 \param[in,out] r_impulseForce 
		*************************************************************************************/
		void ApplyLinearImpulse(vec2 const& r_impulseForce);

		// ----- Serialization ----- //


		/*!***********************************************************************************
		\brief Serializes the RigidBody object to a JSON object.

		\return The JSON representation of the RigidBody object.
		*************************************************************************************/
		nlohmann::json ToJson(size_t id) const
		{
			id;
			nlohmann::json j;
			j["type"] = static_cast<int>(GetType()); // right now its an enum
			j["mass"] = m_mass;
			j["linearDrag"] = m_linearDrag;

			return j;
		}

		/*!***********************************************************************************
		\brief Deserializes the RigidBody object from a JSON object.

		\param[in] j JSON object containing the values to load into the RigidBody object.
		\return The deserialized RigidBody object.
		*************************************************************************************/
		static RigidBody FromJson(const nlohmann::json& j)
		{
			RigidBody rb;
			rb.SetType(static_cast<EnumRigidBodyType>(j["type"].get<int>())); // right now its an enum
			rb.m_mass = j["mass"];
			rb.m_linearDrag = j["linearDrag"];


			return rb;
		}

	private:
		// ----- Private Variables ----- //
		EnumRigidBodyType m_type = EnumRigidBodyType::STATIC;

		float m_mass{10.f};
		float m_inverseMass{1.f/10.f};
		float m_linearDrag{};
	};

}