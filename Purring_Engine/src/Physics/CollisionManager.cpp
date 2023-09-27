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
#include "Logging/Logger.h"

extern Logger engine_logger;
std::vector<PE::Manifold> PE::CollisionManager::m_manifolds;
PE::CollisionManager* PE::CollisionManager::p_instance;

namespace PE
{
	// ----- Public Getters ----- //
	CollisionManager* CollisionManager::GetInstance()
	{
		if (!p_instance)
		{
			p_instance = new CollisionManager();
		}
		return p_instance;
	}

	void CollisionManager::UpdateColliders()
	{
		for (EntityID ColliderID : SceneView<Collider, Transform>())
		{
			Transform const& transform = EntityManager::GetInstance().Get<Transform>(ColliderID);
			Collider& collider = EntityManager::GetInstance().Get<Collider>(ColliderID);
			
			// remove objects that were checked for collision from previous frame
			collider.objectsCollided.clear();
			
			std::visit([&](auto& col)
			{
					Update(col, transform.position, vec2{ transform.width, transform.height });
			
			}, collider.colliderVariant);
		}
	}

	void CollisionManager::TestColliders()
	{
		for (EntityID ColliderID_1 : SceneView<Collider, Transform>())
		{

			Collider& collider1 = EntityManager::GetInstance().Get<Collider>(ColliderID_1);
			
			for (EntityID ColliderID_2 : SceneView<Collider>())
			{

				Collider& collider2 = EntityManager::GetInstance().Get<Collider>(ColliderID_2);
				
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
							//engine_logger.AddLog(false, "Collided!\n", __FUNCTION__);

							// adds collided objects so that it won't be checked again
							collider1.objectsCollided.emplace(ColliderID_2);
							collider2.objectsCollided.emplace(ColliderID_1);

							m_manifolds.emplace_back
							(Manifold{ contactPt,
									   EntityManager::GetInstance().Get<Transform>(ColliderID_1),
									   EntityManager::GetInstance().Get<Transform>(ColliderID_2),
									   EntityManager::GetInstance().GetPointer<RigidBody>(ColliderID_1),
									   EntityManager::GetInstance().GetPointer<RigidBody>(ColliderID_2) });
						}
						//else
							//engine_logger.AddLog(false, "Not Collided!\n", __FUNCTION__);

					}, collider2.colliderVariant);

				}, collider1.colliderVariant);

			}
		}
	}

	void CollisionManager::ResolveCollision(float deltaTime)
	{
		for (Manifold& r_manifold : m_manifolds)
		{
			r_manifold.Resolve(deltaTime);
		}
		m_manifolds.clear();
	}


	void CollisionManager::DeleteInstance()
	{
		delete p_instance;
		p_instance = nullptr;
	}


	// Rect + Rect
	bool CollisionIntersection(AABBCollider const& r_AABB1, AABBCollider const& r_AABB2, EntityID const& r_entity1, EntityID const& r_entity2, Contact& r_contactPt)
	{
		// Static Collision
		if (r_AABB1.max.x < r_AABB2.min.x || r_AABB1.min.x > r_AABB2.max.x)
		{ return false; }
		if (r_AABB1.max.y < r_AABB2.min.y || r_AABB1.min.y > r_AABB2.max.y)
		{ return false; }

		// Dynamic Collision
		
		
		return true;
	}
	
	// Circle + Circle
	bool CollisionIntersection(CircleCollider const& r_circle1, CircleCollider const& r_circle2, EntityID const& r_entity1, EntityID const& r_entity2, Contact& r_contactPt)
	{
		vec2 const deltaPosition{ r_circle1.center - r_circle2.center };
		float const deltaLengthSquared = deltaPosition.LengthSquared();
		float const totalRadius{ (r_circle1.radius + r_circle2.radius) };
		// Static Collision
		if (deltaLengthSquared < totalRadius * totalRadius)
		{
			// get contact point data etc.
			if (deltaLengthSquared <= 0.f)
			{
				r_contactPt.intersectionPoint = (r_contactPt.normal * r_circle2.radius) + r_circle2.center;
				r_contactPt.normal = vec2{ 0.f, 1.f };
				r_contactPt.penetrationDepth = r_circle1.radius;
			}
			else
			{
				r_contactPt.normal = deltaPosition.GetNormalized();
				r_contactPt.intersectionPoint = (r_contactPt.normal * r_circle2.radius) + r_circle2.center;
				r_contactPt.penetrationDepth = totalRadius - sqrtf(deltaLengthSquared);
			}
			return true; 
		}
		//else
		//{
		//	// Dynamic Collision Check

		//	vec2 const& startPos_e1 = EntityManager::GetInstance().Get<RigidBody>(r_entity1).m_prevPosition;
		//	vec2 const& endPos_e1 = r_circle1.center;
		//	//vec2 const& v_e1 = endPos_e1 - startPos_e1;

		//	vec2 const& startPos_e2 = EntityManager::GetInstance().Get<RigidBody>(r_entity2).m_prevPosition;
		//	vec2 const& endPos_e2 = r_circle2.center;
		//	//vec2 const& v_e2 = endPos_e2 - startPos_e2;

		//	vec2 const v = (endPos_e1 - startPos_e1) - (endPos_e2 - startPos_e2); // v = v1 - v2
		//	vec2 const s = startPos_e1 - startPos_e2;
		//	
		//	float eqnC = Dot(s, s) - (totalRadius * totalRadius);
		//	if (eqnC < 0.f)
		//	{
		//		
		//	}

		//	// quadratic equation to solve for t
		//	float eqnA = Dot(v, v);
		//	if (eqnA < std::numeric_limits<float>::epsilon()) 
		//	{ return false; } // not moving relative to each other
		//	
		//	float eqnB = Dot(v, s);
		//	if (eqnB >= 0.f)
		//	{ return false; } // not moving towards each other

		//	
		//	
		//	// checks if it has root values;
		//	float discriminant = 
		//}
		

		return false;
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
		int collided = 0;

		if (r_circle.center.x < r_AABB.min.x) // left side
		{
			LineSegment lineSeg{ r_AABB.min, vec2{r_AABB.min.x, r_AABB.max.y} };
			collided += CircleLineIntersection(r_circle, lineSeg, r_entity1, interTime, r_contactPt);
			//std::cout << "Left\n";
		}
		else if (r_circle.center.x > r_AABB.max.x) // right side
		{
			LineSegment lineSeg{ r_AABB.max, vec2{r_AABB.max.x, r_AABB.min.y} };
			collided += CircleLineIntersection(r_circle, lineSeg, r_entity1, interTime, r_contactPt);
			//std::cout << "Right\n";
		}
		else
		{
			collided += 1;
		}
		if (r_circle.center.y < r_AABB.min.y) // bottom side
		{
			LineSegment lineSeg{ vec2{r_AABB.max.x, r_AABB.min.y}, r_AABB.min };
			collided += CircleLineIntersection(r_circle, lineSeg, r_entity1, interTime, r_contactPt);
			//std::cout << "bottom\n";
		}
		else if (r_circle.center.y > r_AABB.max.y) // top side
		{
			LineSegment lineSeg{ vec2{r_AABB.min.x, r_AABB.max.y}, r_AABB.max };
			collided += CircleLineIntersection(r_circle, lineSeg, r_entity1, interTime, r_contactPt);
			//std::cout << "top\n";
		}
		else
		{
			collided += 1;
		}

		return (collided >= 2);
	}

	// Circle + Line
	int CircleLineIntersection(CircleCollider const& r_circle, LineSegment const& r_lineSeg, EntityID const& r_entity1, float& r_interTime, Contact& r_contactPt)
	{
		// Static Collision
		float const check = Dot(r_lineSeg.normal, r_circle.center - r_lineSeg.point0);
		if (check <= r_circle.radius)
		{
			float p0centerLength = (r_circle.center - r_lineSeg.point0).LengthSquared();
			float p1centerLength = (r_circle.center - r_lineSeg.point1).LengthSquared();
			float radiusSquare = r_circle.radius * r_circle.radius;
			return (p0centerLength <= radiusSquare) + (p1centerLength <= radiusSquare);

			/*vec2 normalScaleRadius = (check <= -r_circle.radius) ? -(r_lineSeg.normal * r_circle.radius) : (r_lineSeg.normal * r_circle.radius);

			vec2 _p0 = r_lineSeg.point0 + normalScaleRadius;
			vec2 _p1 = r_lineSeg.point1 + normalScaleRadius;

			vec2 v = EntityManager::GetInstance().Get<Transform>(r_entity1).position - r_circle.center;
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
						r_contactPt.intersectionPoint = r_circle.center + (v * interTime);
						r_contactPt.normal = (check <= -r_circle.radius) ? -r_lineSeg.normal : r_lineSeg.normal;
					}
					return true;
				}
			}
			else
			{
				return false; // call check line edges function here with false
			}*/
		}
		else
		{
			return false; // call check line edges function here with true
		}
		//return false;
	}
}

/* 
Physics + Collision Tentative Loop
Update Dynamics
TestColliders
ResolveCollision / Update Position
Update Colliders
*/
