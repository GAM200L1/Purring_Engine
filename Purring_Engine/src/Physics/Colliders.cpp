/*!*****************************************************************************
	@file       Colliders.cpp
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       15/9/2023

	@brief		This file contains definitions for functions that updates colliders,
				constructs line segments and manifolds and resolves each manifolds collision

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#include "prpch.h"
#include "Colliders.h"

namespace PE
{
	void Update(AABBCollider& r_AABB, vec2 const& r_position, vec2 const& r_scale)
	{
		r_AABB.center = r_position + r_AABB.positionOffset;
		r_AABB.scale = vec2{ r_scale.x * r_AABB.scaleOffset.x, r_scale.y * r_AABB.scaleOffset.y };
		r_AABB.min = r_AABB.center - (r_AABB.scale * 0.5f);
		r_AABB.max = r_AABB.center + (r_AABB.scale * 0.5f);
	}


	// ---- Circle Collider ----- //

	void Update(CircleCollider& r_circle, vec2 const& r_position, vec2 const& r_scale)
	{
		r_circle.center = r_position + r_circle.positionOffset;
		r_circle.radius = (r_scale.x > r_scale.y)? r_scale.x : r_scale.y;
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

	Manifold::Manifold(Contact const& r_contData,
		Transform& r_transA, Transform& r_transB,
		RigidBody* p_rbA, RigidBody* p_rbB)
		: contactData{ r_contData },
		r_transformA{ r_transA }, r_transformB{ r_transB },
		p_rigidBodyA{ p_rbA }, p_rigidBodyB{ p_rbB } {}

	void Manifold::ResolveCollision()
	{
		ResolveVelocity();
		ResolvePosition();
	}

	// set the objects to where they would be when they just collide
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

	// set the post trajectory
	void Manifold::ResolveVelocity()
	{
		float p = (2.f * (Dot(p_rigidBodyA->m_velocity, contactData.normal) - Dot(p_rigidBodyB->m_velocity, contactData.normal))) / (p_rigidBodyA->GetMass() + p_rigidBodyB->GetMass());
		//std::cout << p << '\n';
		if (p_rigidBodyA->GetType() == EnumRigidBodyType::DYNAMIC)
		{
			p_rigidBodyA->m_velocity = p_rigidBodyA->m_velocity - (contactData.normal * p_rigidBodyA->GetMass() * p);
		}
		if (p_rigidBodyB->GetType() == EnumRigidBodyType::DYNAMIC)
		{
			p_rigidBodyB->m_velocity = p_rigidBodyB->m_velocity + (contactData.normal * p_rigidBodyB->GetMass() * p);
		}
	}
}

// do not do this for m1
/*struct OBB
{

};*/