/*!*****************************************************************************
	@file       Colliders.cpp
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#include "prpch.h"
#include "Colliders.h"

namespace PE
{
	// ----- AABB Collider ----- //
	
	void Update(AABBCollider& r_AABB, vec2 const& r_position, vec2 const& r_scale)
	{
		r_AABB.min = r_position - (r_scale * 0.5f);
		r_AABB.max = r_position + (r_scale * 0.5f);
	}
	

	// ---- Circle Collider ----- //
	
	void Update(CircleCollider& r_circle, vec2 const& r_position, vec2 const& r_scale)
	{
		r_circle.center = r_position;
		r_circle.radius = r_scale.x * 0.5f;
	}


	// ----- Line Segment Constructor ----- //
	LineSegment::LineSegment(vec2 const& r_startPt, vec2 const& r_endPt)
	{
		point0 = r_startPt;
		point1 = r_endPt;
		normal = point1 - point0;
		normal = vec2{ normal.y, -normal.x };
		normal.Normalize();
	}

	Manifold::Manifold(Contact const& r_contData,
					   Transform& r_transA, Transform& r_transB,
					   RigidBody* r_rbA, RigidBody* r_rbB)
					   : contactData{ r_contData },
					     r_transformA{ r_transA }, r_transformB{ r_transB },
					     r_rigidBodyA{ r_rbA }, r_rigidBodyB{ r_rbB } {}

	void Manifold::Resolve(float deltaTime)
	{
		ResolveVelocity();
		ResolvePosition(deltaTime);
	}

	// set the objects to where they would be when they just collide
	void Manifold::ResolvePosition(float deltaTime)
	{
		float totalInvMass = r_rigidBodyA->GetInverseMass() + r_rigidBodyB->GetInverseMass();

		vec2 penM = contactData.normal * (contactData.penetrationDepth / totalInvMass);

		if (r_rigidBodyA->GetType() == EnumRigidBodyType::DYNAMIC)
		{
			r_transformA.position += (penM * r_rigidBodyA->GetInverseMass());
		}
		if (r_rigidBodyB->GetType() == EnumRigidBodyType::DYNAMIC)
		{
			r_transformB.position += (penM * -r_rigidBodyB->GetInverseMass());
		}
	}

	// set the post trajectory
	void Manifold::ResolveVelocity()
	{
		float p = (2.f * (Dot(r_rigidBodyA->m_velocity, contactData.normal) - Dot(r_rigidBodyB->m_velocity, contactData.normal))) / (r_rigidBodyA->GetMass() + r_rigidBodyB->GetMass());
		//std::cout << p << '\n';
		if (r_rigidBodyA->GetType() == EnumRigidBodyType::DYNAMIC)
		{
			r_rigidBodyA->m_velocity = r_rigidBodyA->m_velocity - (contactData.normal * r_rigidBodyA->GetMass() * p);
		}
		if (r_rigidBodyB->GetType() == EnumRigidBodyType::DYNAMIC)
		{	
			r_rigidBodyB->m_velocity = r_rigidBodyB->m_velocity + (contactData.normal * r_rigidBodyB->GetMass() * p);
		}
	}
}

// do not do this for m1
/*struct OBB
{

};*/