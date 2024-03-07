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
#include "ECS/Entity.h"

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

	std::optional<LSColResult> DoRayCast(const LineSegment& ls, const size_t& tgt)
	{
		std::optional<LSColResult> ret;

		if (EntityManager::GetInstance().Has<Collider>(tgt))
		{
			if (ret = CheckLineCollision(ls, EntityManager::GetInstance().Get<Collider>(tgt)))
			{
				ret.value().contactedEntity = tgt;
			}
		}

		return ret;
	}

	std::optional<LSColResult> CheckLineCollision(const LineSegment& ls, const Collider& tgt)
	{
		std::optional<LSColResult> ret;
		std::visit([&](auto& target)
			{
				ret = CheckLSCollision(ls, target);
			}, tgt.colliderVariant);
		// just incase the std::visit vails, return default constructed std::optional
		return ret;
	}

	std::optional<LSColResult> CheckLSCollision(const LineSegment& ls, const CircleCollider& tgt)
	{
		std::optional<LSColResult> ret;
		vec2 v(ls.point1.x - ls.point0.x, ls.point1.y - ls.point0.y);
		float a = v.x * v.x + v.y * v.y;
		float b = 2 * (v.x * tgt.center.x + v.y * tgt.center.y);
		float c = (tgt.center.x * tgt.center.x) + (tgt.center.y * tgt.center.y) - (tgt.radius * tgt.radius);
		
		// if a > 0, there is a solution to the equation
		if (a > 0)
		{
			float det = b * b - 4 * a * c;
			if (!det) // if det == 0, ls is tangent to the circle
			{
				ret = LSColResult();
				float t = -b / (2 * a);
				ret.value().contactPoint.x = ls.point0.x + t * v.x;
				ret.value().contactPoint.y = ls.point0.y + t * v.y;
			}
			else 
			{
				ret = LSColResult();
				det = sqrtf(det);
				float t1 = (-b - det) / (2 * a);
				// float t2 = (-b + det) / (2 * a);
				ret.value().contactPoint = vec2(ls.point0.x + t1 * v.x, ls.point0.y + t1 * v.y);
			}
		}
		return ret;
	}

	std::optional<LSColResult> CheckLSCollision(const LineSegment& ls, const AABBCollider& tgt)
	{
		std::optional<LSColResult> ret;

		std::vector<LineSegment> lineSegments{ LineSegment(vec2(tgt.min.x, tgt.min.y), vec2(tgt.max.x, tgt.min.y)),		// btm left to btm right
											   LineSegment(vec2(tgt.min.x, tgt.min.y), vec2(tgt.min.x, tgt.max.y)),		// btm left to top left
											   LineSegment(vec2(tgt.max.x, tgt.min.y), vec2(tgt.max.x, tgt.max.y)),		// btm right to top right
											   LineSegment(vec2(tgt.min.x, tgt.max.y), vec2(tgt.max.x, tgt.max.y)) };	// top left to top right

		std::array<std::optional<LSColResult>, 4> results;
		// utilizing the sweep line algo
		int cnt{ 0 };

		// line segment ls as a1x + b1y = c1;
		const float a1 = ls.point1.y - ls.point0.y;
		const float b1 = ls.point0.x - ls.point1.x;
		const float c1 = a1 * (ls.point0.x) + b1 * (ls.point0.y);

		for (const auto& seg : lineSegments)
		{
			int i{ cnt++ };
			// if either of the lines have zero lenght (sqrd vs sqr doesnt matter)
			if (!ls.lineVec.LengthSquared() || !seg.lineVec.LengthSquared())
				continue;

			// line segment seg as a2x + b2y = c2;
			const float a2 = seg.point1.y - seg.point0.y;
			const float b2 = seg.point0.x - seg.point1.x;
			const float c2 = a2 * (seg.point0.x) + b2 * (seg.point0.y);

			float det = a1 * b2 - a2 * b1;
			if (det) // lines are not parallel
			{
				vec2 contactPoint(b2 * c1 - b1 * c2, a1 * c2 - a2 * c1);
				contactPoint /= det;

				if (contactPoint.x >= tgt.min.x && contactPoint.x <= tgt.max.x && 
					contactPoint.y >= tgt.min.y && contactPoint.y <= tgt.max.y)
				{
					results[i] = LSColResult();
					results[i].value().contactPoint = contactPoint;
					results[i].value().angle = acos(Dot(seg.lineVec.GetNormalized(), ls.lineVec.GetNormalized()));
				}
			}
		}
		float prevDist{ FLT_MAX };
		for (const auto& res : results)
		{
			if (res)
			{
				float newDistSqr = (ls.point0 - res.value().contactPoint).LengthSquared();
				prevDist = (newDistSqr < prevDist)? newDistSqr : prevDist;
				if (prevDist == newDistSqr)
				{
					ret = res;
				}
			}
		}


		return ret;
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