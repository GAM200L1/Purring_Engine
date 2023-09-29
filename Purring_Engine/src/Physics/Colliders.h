/*!*****************************************************************************
	@file       Colliders.h
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       15/9/2023

	@brief		This file contains struct implementation for each type of collider,
				line segment, manifolds, and declaration of each collider's update functions

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#pragma once

#include "Math/MathCustom.h"
#include "Math/Transform.h"
#include "RigidBody.h"
#include <variant>
#include "Data/json.hpp"
#include <set>

namespace PE
{
	// may change this to box collider to combine obb and aabb in the future
	
	// ----- AABB Collider ----- //
	
	//! Struct containing AABBCollider parameters
	struct AABBCollider
	{
		// ----- Public Variables ----- //
		vec2 positionOffset{ 0.f, 0.f }; // Position offset from the collider's entity's center. Will add on to entity position.
		vec2 scaleOffset{ 1.f, 1.f }; // Scale offset from the collider's entity's height and width. Will multiply with entity scale.
		vec2 center{};
		vec2 scale{};
		vec2 min{}; // bottom left point of AABB
		vec2 max{}; // top right point of AABB


		// ----- Public Methods ----- //
		
		// Serialization
		nlohmann::json ToJson() const
		{
			nlohmann::json j;
			j["min"]["x"] = min.x;
			j["min"]["y"] = min.y;
			j["max"]["x"] = max.x;
			j["max"]["y"] = max.y;
			return j;
		}

		// Deserialization
		static AABBCollider FromJson(const nlohmann::json& r_j)
		{
			AABBCollider aabb;
			aabb.min.x = r_j["min"]["x"];
			aabb.min.y = r_j["min"]["y"];
			aabb.max.x = r_j["max"]["x"];
			aabb.max.y = r_j["max"]["y"];
			return aabb;
		}
	};

	/*!***********************************************************************************
	 \brief Updates the position and scale of the AABBCollider according to its entity's
			current scale and position

	 \param[in] r_AABB		reference to the AABBCollider which parameters are to be updated
	 \param[in] r_position 	position to update r_AABB parameters off of
	 \param[in] r_scale		scale to update r_AABB parameters off of
	*************************************************************************************/
	void Update(AABBCollider& r_AABB, vec2 const& r_position, vec2 const& r_scale);

	// ----- Circle Collider ----- //

	//! Struct containing CircleCollider parameters
	struct CircleCollider
	{
		// ----- Public Variables ----- //
		vec2 positionOffset{ 0.f, 0.f }; // Position offset from the collider's entity's center. Will add on to entity position.
		float scaleOffset{ 1.f }; // Scale offset from the collider's entity's height or width. Will multiply with entity scale.
		vec2 center{};
		float radius{}; // takes the larger axis of the entity's scale


		// ----- Public Methods ----- //
		
		// Serialization
		nlohmann::json ToJson() const
		{
			nlohmann::json j;
			j["center"]["x"] = center.x;
			j["center"]["y"] = center.y;
			j["radius"] = radius;
			return j;
		}

		// Deserialization
		static CircleCollider FromJson(const nlohmann::json& r_j)
		{
			CircleCollider circle;
			circle.center.x = r_j["center"]["x"];
			circle.center.y = r_j["center"]["y"];
			circle.radius = r_j["radius"];
			return circle;
		}

	};

	/*!***********************************************************************************
	 \brief Updates the position and radius of the r_circle according to the assigned entity's
			current scale and position
	
	 \param[in] r_circle	reference to the CircleCollider which parameters are to be updated
	 \param[in] r_position 	position to update r_circle parameters off of
	 \param[in] r_scale		scale to update r_circle parameters off of
	*************************************************************************************/
	void Update(CircleCollider& r_circle, vec2 const& r_position, vec2 const& r_scale);
	

	// ----- Collider ----- //

	//! General Collider each type of collider is loosely based off
	struct Collider
	{
		// ----- Public Variables ----- //
		public:

		std::variant<AABBCollider, CircleCollider> colliderVariant; // contains the different types of colliders
		std::set<size_t> objectsCollided; // contains the IDs of the entities that each collider has collided with
		bool isTrigger{ false }; // determines whether the collider will need to resolve its collision

		// ----- Public Methods ----- //
		public:

		// Serialization
		nlohmann::json ToJson() const
		{
			nlohmann::json j;
			if (std::holds_alternative<AABBCollider>(colliderVariant))
			{
				j["type"] = "AABBCollider";
				//j["data"] = std::get<AABBCollider>(colliderVariant).ToJson();			// for future -hans
			}
			else if (std::holds_alternative<CircleCollider>(colliderVariant))
			{
				j["type"] = "CircleCollider";
				//j["data"] = std::get<CircleCollider>(colliderVariant).ToJson();		// for future -hans
			}
			return j;
		}

		// Deserialization
		static Collider FromJson(const nlohmann::json& r_j)
		{
			Collider collider;
			std::string type = r_j["type"];
			if (type == "AABBCollider")
			{
				collider.colliderVariant = AABBCollider::FromJson(r_j["data"]);
			}
			else if (type == "CircleCollider")
			{
				collider.colliderVariant = CircleCollider::FromJson(r_j["data"]);
			}
			return collider;
		}
	};

	// ----- Line Segment ----- //

	//! Struct containing LineSegment parameters
	struct LineSegment
	{
		// ----- Public Variables ----- //
		public:

		vec2 point0; // start point of the line
		vec2 point1; // end point of the line
		vec2 lineVec; // the vector of the line segment
		vec2 normal; // normalized normal of line segment

		// ----- Constructor ----- //
		public:
		
		/*!***********************************************************************************
		 \brief Alternative constructor for LineSegment. Sets point0 and point1 of the LineSegment
				using input parameters and calculates the other parameters based off it.
		
		 \param[in] r_startPt	The coordinates for the starting point of the line segment
		 \param[in] r_endPt 	The coordinates for the end point of the line segment
		*************************************************************************************/
		LineSegment(vec2 const& r_startPt, vec2 const& r_endPt);
		
	};

	// ----- Collision Info ----- //

	//! Struct containing parameters which denote the the point where two objects have collided
	struct Contact
	{
		// ----- Public Variables ------ //
		public:

		vec2 intersectionPoint{ 0.f, 0.f };
		vec2 normal{ 0.f, 0.f };		// outward normal from intersectionPoint, towards Entity1
		float penetrationDepth{ 0.f }; // amount of overlap between the two objects
	};

	//! Struct containing contact point data and reference to the transform and pointer to the RigidBody of the respective entity
	struct Manifold
	{
		// ----- Public Variables ----- //
		public:

		Transform& r_transformA;
		Transform& r_transformB;

		RigidBody* p_rigidBodyA = nullptr; // this could be nonexistent as trigger/static
		RigidBody* p_rigidBodyB = nullptr; // collision objects do not always need a rigidbody

		Contact contactData;

		// ----- Constructors ------ //
		public:
		
		// Manifold cannot be constructed with no basis entity therefore constructor with no input parameters is deleted
		Manifold() = delete;

		/*!***********************************************************************************
		 \brief Constructor for Manifold

		 \param[in] r_contData		Contact info of the two colliding entities
		 \param[in] r_transA 		Transform of the first involved entity
		 \param[in] r_transB 		Transform of the second involved entity
		 \param[in] p_rigidBodyA 	RigidBody of the first involved entity
		 \param[in] p_rigidBodyB 	RigidBody of the second involved entity
		*************************************************************************************/
		Manifold(Contact const& r_contData,
				 Transform& r_transA, Transform& r_transB,
				 RigidBody* p_rbA, RigidBody* p_rbB);

		// ------ Public Methods ------ //
		public:
		
		/*!***********************************************************************************
		 \brief Resolves both velocity and position of the entities by calling ResolveVelocity
				and Resolve Position

		*************************************************************************************/
		void ResolveCollision();

		/*!***********************************************************************************
		 \brief Resolves velocity of both Manifold entities where applicable. Determines the
				next direction dynamic object will bounce to

		*************************************************************************************/
		void ResolveVelocity();

		/*!***********************************************************************************
		 \brief Resolves position of both Manifold entities where applicable. Preventing them
				from appearing like they were overlapping

		*************************************************************************************/
		void ResolvePosition();
	};
}
