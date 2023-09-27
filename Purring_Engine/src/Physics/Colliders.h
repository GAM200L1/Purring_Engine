/*!*****************************************************************************
	@file       Colliders.h
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
#include "Math/Transform.h"
#include "RigidBody.h"
#include <variant>


namespace PE
{
	// may change this to box collider to combine obb and aabb in the future
	struct AABBCollider
	{
		// ----- Public Variables ----- //
		//vec2 offsetFromObj{ 0.f, 0.f };
		vec2 center{};
		vec2 min{};
		vec2 max{};
		vec2 scale{};
		bool isTrigger{ false };
	};
	
	void Update(AABBCollider& r_AABB, vec2 const& r_position, vec2 const& r_scale);

	struct CircleCollider
	{
		// ----- Public Variables ----- //
		vec2 center{};
		float radius{};
		bool isTrigger{ false };
	};

	struct Collider
	{
		std::variant<AABBCollider, CircleCollider> colliderVariant;
		std::set<size_t> objectsCollided;
	};

	struct LineSegment
	{
		LineSegment() = default;
		LineSegment(vec2 const& r_startPt, vec2 const& r_endPt);
		vec2 point0;
		vec2 point1;
		vec2 lineVec;
		vec2 normal;
	};


	struct Contact
	{
		vec2 intersectionPoint{ 0.f, 0.f };
		vec2 normal{ 0.f, 0.f };
		float penetrationDepth{ 0.f };
	};

	struct Manifold
	{
		Manifold() = delete;
		Manifold(Contact const& r_contData,
				 Transform& r_transA, Transform& r_transB,
				 RigidBody* r_rbA, RigidBody* r_rbB);

		Transform& r_transformA;
		Transform& r_transformB;

		RigidBody* r_rigidBodyA = nullptr;
		RigidBody* r_rigidBodyB = nullptr;

		Contact contactData;

		void ResolveCollision();
		void ResolveVelocity();
		void ResolvePosition();
	};

	void Update(CircleCollider& r_circle, vec2 const& r_position, vec2 const& r_scale);
}