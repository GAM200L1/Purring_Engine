/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Colliders.cpp
 \date     16-09-2023
 
 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu
 
 \brief    This file contains definition of each collider's update functions and
		   definition for resolving collision.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "Colliders.h"

namespace PE
{
	// ----- AABB Collider ----- //
	void Update(AABBCollider& r_AABB, vec2 const& r_position, vec2 const& r_scale)
	{
		r_AABB.center = r_position + r_AABB.positionOffset;
		r_AABB.scale = vec2{ std::abs(r_scale.x * r_AABB.scaleOffset.x), std::abs(r_scale.y * r_AABB.scaleOffset.y) };
		r_AABB.min = r_AABB.center - (r_AABB.scale * 0.5f);
		r_AABB.max = r_AABB.center + (r_AABB.scale * 0.5f);
	}


	// ---- Circle Collider ----- //

	void Update(CircleCollider& r_circle, vec2 const& r_position, vec2 const& r_scale)
	{
		r_circle.center = r_position + r_circle.positionOffset;
		r_circle.radius = std::abs((r_scale.x > r_scale.y) ? r_scale.x : r_scale.y); // Collider radius is based on larger axis
		r_circle.radius *= 0.5f * r_circle.scaleOffset;
	}


	// ----- Line Segment Constructor ----- //
	LineSegment::LineSegment(vec2 const& r_startPt, vec2 const& r_endPt)
	{
		point0 = r_startPt;
		point1 = r_endPt;
		lineVec = point1 - point0;
		normal = vec2{ lineVec.y, -lineVec.x }.GetNormalized();
	}

	// ----- Manifolds ------ //

	Manifold::Manifold(Contact const& r_contData,
		Transform& r_transA, Transform& r_transB,
		RigidBody* p_rbA, RigidBody* p_rbB)
		: contactData{ r_contData },
		r_transformA{ r_transA }, r_transformB{ r_transB },
		p_rigidBodyA{ p_rbA }, p_rigidBodyB{ p_rbB } {}

	// will not be called if one of the collidable objects involved is a trigger
	void Manifold::ResolveCollision()
	{
		ResolveVelocity();
		ResolvePosition();
	}

	// set the objects to where they would be when they collide, i.e. not overlapping - only for dynamic objects
	void Manifold::ResolvePosition()
	{
		float totalInvMass = p_rigidBodyA->GetInverseMass() + p_rigidBodyB->GetInverseMass();

		vec2 penM = contactData.normal * (contactData.penetrationDepth / totalInvMass);

		if (p_rigidBodyA->GetType() == EnumRigidBodyType::DYNAMIC)
		{
			r_transformA.position += (penM * p_rigidBodyA->GetInverseMass());
		}
		if (p_rigidBodyB->GetType() == EnumRigidBodyType::DYNAMIC)
		{
			r_transformB.position += (penM * -p_rigidBodyB->GetInverseMass());
		}
	}

	// set the trajectory post-collision - only valid for dynamic objects
	void Manifold::ResolveVelocity()
	{
		float p = (2.f * (Dot(p_rigidBodyA->velocity, contactData.normal) - Dot(p_rigidBodyB->velocity, contactData.normal))) / (p_rigidBodyA->GetInverseMass() + p_rigidBodyB->GetInverseMass());
		
		if (p_rigidBodyA->GetType() == EnumRigidBodyType::DYNAMIC)
		{
			p_rigidBodyA->velocity = p_rigidBodyA->velocity - (contactData.normal * p_rigidBodyA->GetInverseMass() * p);
		}
		if (p_rigidBodyB->GetType() == EnumRigidBodyType::DYNAMIC)
		{
			p_rigidBodyB->velocity = p_rigidBodyB->velocity + (contactData.normal * p_rigidBodyB->GetInverseMass() * p);
		}
	}
}