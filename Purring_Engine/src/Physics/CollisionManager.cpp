/*!*****************************************************************************
	@file       CollisionManager.cpp
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/

#include "prpch.h"
#include "CollisionManager.h"
#include "ECS/Entity.h"
#include "ECS/SceneView.h"

std::vector<PE::Manifold> PE::CollisionManager::m_manifolds;
PE::CollisionManager* PE::CollisionManager::m_ptrInstance;

namespace PE
{
	// ----- Public Getters ----- //
	CollisionManager* CollisionManager::GetInstance()
	{
		if (!m_ptrInstance)
		{
			m_ptrInstance = new CollisionManager();
		}
		return m_ptrInstance;
	}

	void CollisionManager::UpdateColliders()
	{
		for (EntityID ColliderID : SceneView<Collider, Transform>())
		{
			Transform const& transform = g_entityManager->Get<Transform>(ColliderID);
			Collider& collider = g_entityManager->Get<Collider>(ColliderID);
			
			// remove objects that were checked for collision from previous frame
			collider.objectsCollided.clear();
			
			std::visit([&](auto& col)
			{
				Update(col, transform.position, transform.scale);
			
			}, collider.colliderVariant);
		}
	}

	void CollisionManager::TestColliders()
	{
		for (EntityID ColliderID_1 : SceneView<Collider, Transform>())
		{

			Collider& collider1 = g_entityManager->Get<Collider>(ColliderID_1);
			
			for (EntityID ColliderID_2 : SceneView<Collider>())
			{

				Collider& collider2 = g_entityManager->Get<Collider>(ColliderID_2);
				
				// if its the same don't check
				if (ColliderID_1 == ColliderID_2) { continue; }
				// if they have been checked before don't check again
				if (collider1.objectsCollided.count(ColliderID_2)) { continue; }

				std::visit([&](auto& col1)
				{
					std::visit([&](auto& col2)
					{
						Contact contactPt;
						if (CollisionIntersection(col1, col2, ColliderID_1, ColliderID_2, contactPt))
						{						
							std::cout << "Collided!\n";

							// adds collided objects so that it won't be checked again
							collider1.objectsCollided.emplace(ColliderID_2);
							collider2.objectsCollided.emplace(ColliderID_1);

							m_manifolds.emplace_back
							(Manifold{ contactPt,
									   g_entityManager->Get<Transform>(ColliderID_1),
									   g_entityManager->Get<Transform>(ColliderID_2),
									   g_entityManager->GetPointer<RigidBody>(ColliderID_1),
									   g_entityManager->GetPointer<RigidBody>(ColliderID_2) });
						}

					}, collider2.colliderVariant);

				}, collider1.colliderVariant);

			}
		}
	}

	void CollisionManager::ResolveCollision()
	{
		/*for (Manifold& r_manifold : m_manifolds)
		{

		}
		m_manifolds.clear();*/
	}

	// Rect + Rect
	bool CollisionIntersection(AABBCollider const& r_AABB1, AABBCollider const& r_AABB2, EntityID const& r_entity1, EntityID const& r_entity2, Contact& r_contactPt)
	{
		if (r_AABB1.max.x < r_AABB2.min.x || r_AABB1.min.x > r_AABB2.max.x)
		{ return false; }
		if (r_AABB1.max.y < r_AABB2.min.y || r_AABB1.min.y > r_AABB2.max.y)
		{ return false; }
		
		// to do dynamic detection
		return true;
	}
	
	// Circle + Circle
	bool CollisionIntersection(CircleCollider const& r_circle1, CircleCollider const& r_circle2, EntityID const& r_entity1, EntityID const& r_entity2, Contact& r_contactPt)
	{

		if ((r_circle1.center - r_circle2.center).LengthSquared() < r_circle1.radius * r_circle1.radius)
		{ return false; }
		if ((r_circle1.center - r_circle2.center).LengthSquared() < r_circle2.radius * r_circle2.radius)
		{ return false; }

		// to do dynamic detection
		return true;
	}

	// Rect + Circle
	bool CollisionIntersection(AABBCollider const& r_AABB, CircleCollider const& r_circle, EntityID const& r_entity1, EntityID const& r_entity2, Contact& r_contactPt)
	{
		return CollisionIntersection(r_circle, r_AABB, r_entity2, r_entity1, r_contactPt);
	}

	// Circle + Rect
	bool CollisionIntersection(CircleCollider const& r_circle, AABBCollider const& r_AABB, EntityID const& r_entity1, EntityID const& r_entity2, Contact& r_contactPt)
	{
		float interTime{ 1.f };
		if (r_circle.center.x >= r_AABB.min.x && r_circle.center.x <= r_AABB.max.x && r_circle.center.y >= r_AABB.min.y && r_circle.center.y <= r_AABB.max.y)
		{ return true; }

		if (r_circle.center.x < r_AABB.min.x) // left side
		{
			LineSegment lineSeg{ r_AABB.min, vec2{r_AABB.min.x, r_AABB.max.y} };
			CircleLineIntersection(r_circle, lineSeg, r_entity1, interTime, r_contactPt);
			std::cout << "Left\n";
		}
		else if (r_circle.center.x > r_AABB.max.x) // right side
		{
			LineSegment lineSeg{ r_AABB.max, vec2{r_AABB.max.x, r_AABB.min.y} };
			CircleLineIntersection(r_circle, lineSeg, r_entity1, interTime, r_contactPt);
			std::cout << "Right\n";
		}
		if (r_circle.center.y < r_AABB.min.y) // bottom side
		{
			LineSegment lineSeg{ vec2{r_AABB.max.x, r_AABB.min.y}, r_AABB.min };
			CircleLineIntersection(r_circle, lineSeg, r_entity1, interTime, r_contactPt);
			std::cout << "bottom\n";
		}
		else if (r_circle.center.y > r_AABB.max.y) // top side
		{
			LineSegment lineSeg{ vec2{r_AABB.min.x, r_AABB.max.y}, r_AABB.max };
			CircleLineIntersection(r_circle, lineSeg, r_entity1, interTime, r_contactPt);
			std::cout << "top\n";
		}

		return (interTime != 1.f)? true : false;
	}

	// Circle + Line
	bool CircleLineIntersection(CircleCollider const& r_circle, LineSegment const& r_lineSeg, EntityID const& r_entity1, float& r_interTime, Contact& r_contactPt)
	{
		float const check = Dot(r_lineSeg.normal, r_circle.center) - Dot(r_lineSeg.normal, r_lineSeg.point0);
		if (check <= -r_circle.radius || check >= r_circle.radius)
		{
			vec2 normalScaleRadius = (check <= -r_circle.radius) ? -(r_lineSeg.normal * r_circle.radius) : (r_lineSeg.normal * r_circle.radius);

			vec2 _p0 = r_lineSeg.point0 + normalScaleRadius;
			vec2 _p1 = r_lineSeg.point1 + normalScaleRadius;

			vec2 v = g_entityManager->Get<Transform>(r_entity1).position - r_circle.center;
			vec2 M{ v.y, -v.x };

			vec2 Bs_p0 = _p0 - r_circle.center;
			vec2 Bs_p1 = _p1 - r_circle.center;

			float interTime{ 0.f };

			if (Dot(M, Bs_p0) * Dot(M, Bs_p1) < 0.f)
			{
				interTime = Dot(r_lineSeg.normal, r_lineSeg.point0)
							- Dot(r_lineSeg.normal, r_circle.center);

				interTime += (check <= -r_circle.radius)? - r_circle.radius : r_circle.radius;
				interTime /= Dot(r_lineSeg.normal, v);
				
				if (0.f <= interTime && interTime <= 1.f)
				{
					if (r_interTime > interTime)
					{
						r_interTime = interTime;
						r_contactPt.position = r_circle.center + (v * interTime);
						r_contactPt.normal = (check <= -r_circle.radius) ? -r_lineSeg.normal : r_lineSeg.normal;
					}
					return true;
				}
			}
			else
			{
				return false; // call check line edges function here with false
			}
		}
		else
		{
			return false; // call check line edges function here with true
		}
		return false;
	}
}

/* 
Physics + Collision Tentative Loop
Update Dynamics
TestColliders
ResolveCollision / Update Position
Update Colliders
*/
