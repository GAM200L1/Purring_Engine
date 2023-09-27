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

namespace PE
{
	// ----- Constructor ----- //
	CollisionManager::CollisionManager() 
	{
		// empty by design
	}

	// ----- Public Getters ----- //
	Manifold* CollisionManager::GetManifoldVector()
	{
		return m_manifolds.data();
	}

	std::string CollisionManager::GetName()
	{
		return m_systemName;
	}

	// ----- System Methods ----- //
	void CollisionManager::InitializeSystem()
	{
		// Check if Initialization was successful
		engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
		engine_logger.SetTime();
		engine_logger.AddLog(false, "CollisionManager initialized!", __FUNCTION__);
	}

	void CollisionManager::UpdateSystem(float deltaTime)
	{
		// prevent warnings
		deltaTime;
		// Update the Collider's specs
		UpdateColliders();
		// Test for Collisions in the scene
		TestColliders();
		// Resolve the positions and velocities of the entities
		ResolveCollision();
	}

	void CollisionManager::DestroySystem()
	{
		m_manifolds.clear();
	}

	// ----- Collision Methods ----- //

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
			Collider& collider1 = g_entityManager->Get<Collider>(ColliderID_1);

			Collider& collider1 = EntityManager::GetInstance().Get<Collider>(ColliderID_1);
			
			for (EntityID ColliderID_2 : SceneView<Collider>())
			{
				Collider& collider2 = g_entityManager->Get<Collider>(ColliderID_2);

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
						if (CollisionIntersection(col1, col2, contactPt))
						{
							//engine_logger.AddLog(false, "Collided!\n", __FUNCTION__);

							// adds collided objects so that it won't be checked again
							collider1.objectsCollided.emplace(ColliderID_2);
							collider2.objectsCollided.emplace(ColliderID_1);
							if (!collider1.isTrigger && !collider2.isTrigger)
							{
								if (std::holds_alternative<AABBCollider>(collider1.colliderVariant) && std::holds_alternative<CircleCollider>(collider2.colliderVariant))
								{
									m_manifolds.emplace_back
									(Manifold{ contactPt,
											   g_entityManager->Get<Transform>(ColliderID_2),
											   g_entityManager->Get<Transform>(ColliderID_1),
											   g_entityManager->GetPointer<RigidBody>(ColliderID_2),
											   g_entityManager->GetPointer<RigidBody>(ColliderID_1) });
								}
								else
								{
									m_manifolds.emplace_back
									(Manifold{ contactPt,
											   g_entityManager->Get<Transform>(ColliderID_1),
											   g_entityManager->Get<Transform>(ColliderID_2),
											   g_entityManager->GetPointer<RigidBody>(ColliderID_1),
											   g_entityManager->GetPointer<RigidBody>(ColliderID_2) });
								}
							}
							else
							{
								// else send message to trigger event associated with this entity?
								engine_logger.AddLog(false, "Collided with Trigger!\n", __FUNCTION__);
							}
							
						}

					}, collider2.colliderVariant);

				}, collider1.colliderVariant);

			}
		}
	}

	void CollisionManager::ResolveCollision()
	{
		for (Manifold& r_manifold : m_manifolds)
		{
			r_manifold.ResolveCollision();
		}
		m_manifolds.clear();
	}

	// ----- Collision Helper Functions ----- //

	// Rect + Rect
	bool CollisionIntersection(AABBCollider const& r_AABB1, AABBCollider const& r_AABB2, Contact& r_contactPt)
	{
		// If AABB1 bounds are outside AABB2 bounds - not colliding
		if (r_AABB1.max.x < r_AABB2.min.x || r_AABB1.min.x > r_AABB2.max.x) { return false; }
		if (r_AABB1.max.y < r_AABB2.min.y || r_AABB1.min.y > r_AABB2.max.y) { return false; }

		// vector from center of AABB2 to AABB1 center
		vec2 c1c2 = r_AABB2.center - r_AABB1.center; 

		if (c1c2.LengthSquared() == 0.f)
		{
			r_contactPt.normal = vec2{ 0.f, 1.f };
			r_contactPt.intersectionPoint = vec2{ r_AABB2.center.x, r_AABB2.max.y };
			r_contactPt.penetrationDepth = r_AABB1.scale.y * 0.5f;
		}
		else
		{
			r_contactPt.intersectionPoint = r_AABB1.center;
			Clamp(r_contactPt.intersectionPoint.x, r_AABB2.min.x, r_AABB2.max.x);
			Clamp(r_contactPt.intersectionPoint.y, r_AABB2.min.y, r_AABB2.max.y);

			vec2 c1InterPt = r_contactPt.intersectionPoint - r_AABB1.center;

			// checks if the center of the first rectangle is on the edge of the rectangle
			if (c1InterPt.LengthSquared() != 0.f)
			{
				float xIntersectLength = (r_AABB1.scale.x * 0.5f) - abs(c1InterPt.x);
				float yIntersectLength = (r_AABB1.scale.y * 0.5f) - abs(c1InterPt.y);
				// if y penetration length is larger, take x penetration length, vice versa
				r_contactPt.penetrationDepth = (xIntersectLength < yIntersectLength) ? xIntersectLength : yIntersectLength;
				
				// checks which axis is intersecting more
				if (xIntersectLength < yIntersectLength)
				{
					// penetraion by x axis is smaller
					r_contactPt.normal = (r_contactPt.intersectionPoint.x == r_AABB2.min.x)? vec2{ -1.f, 0.f } : vec2{ 1.f,0.f };
				}
				else
				{
					// penetration by y axis is smaller
					r_contactPt.normal = (r_contactPt.intersectionPoint.y == r_AABB2.min.y)? vec2{ 0.f, -1.f } : vec2{ 0.f, 1.f };
				}
			}
			else
			{
				// penetration depth will be set to the width since its at 
				if (r_contactPt.intersectionPoint.x == r_AABB2.min.x)
				{
					// if colliding left
					r_contactPt.normal = vec2{ -1.f, 0.f };
					r_contactPt.penetrationDepth = r_AABB1.scale.x * 0.5f;
				}
				else if (r_contactPt.intersectionPoint.x == r_AABB2.max.x)
				{
					// if colliding right
					r_contactPt.normal = vec2{ 1.f,0.f };
					r_contactPt.penetrationDepth = r_AABB1.scale.x * 0.5f;
				}
				else if (r_contactPt.intersectionPoint.y == r_AABB2.min.y)
				{
					// if colliding bottom
					r_contactPt.normal = vec2{ 0.f, -1.f };
					r_contactPt.penetrationDepth = r_AABB1.scale.y * 0.5f;
				}
				else
				{
					// if colliding top
					r_contactPt.normal = vec2{ 0.f, 1.f };
					r_contactPt.penetrationDepth = r_AABB1.scale.y * 0.5f;
				}
			}
		}
		return true;
	}

	// Circle + Circle
	bool CollisionIntersection(CircleCollider const& r_circle1, CircleCollider const& r_circle2, Contact& r_contactPt)
	{
		vec2 const deltaPosition{ r_circle1.center - r_circle2.center };
		float const deltaLengthSquared = deltaPosition.LengthSquared();
		float const totalRadius{ (r_circle1.radius + r_circle2.radius) };
		// Static Collision
		if (deltaLengthSquared < totalRadius * totalRadius)
		{
			// get contact point data etc.
			if (deltaLengthSquared == 0.f)
			{
				// if the circles are overlapping exactly
				r_contactPt.normal = vec2{ 0.f, 1.f };
				r_contactPt.intersectionPoint = (r_contactPt.normal * r_circle2.radius) + r_circle2.center;
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
		return false;
	}

	// Rect + Circle
	bool CollisionIntersection(AABBCollider const& r_AABB, CircleCollider const& r_circle, Contact& r_contactPt)
	{
		return (CollisionIntersection(r_circle, r_AABB, r_contactPt));
	}

	// Circle + Rect
	bool CollisionIntersection(CircleCollider const& r_circle, AABBCollider const& r_AABB, Contact& r_contactPt)
	{
		int collided{ 0 };

		if (r_circle.center.x >= r_AABB.min.x && r_circle.center.x <= r_AABB.max.x) // if circle center is within the AABB's x range
		{
			collided += 1;
		}
		else if (r_circle.center.x < r_AABB.min.x) // left side
		{
			LineSegment lineSeg{ r_AABB.min, vec2{r_AABB.min.x, r_AABB.max.y} };
			collided += CircleAABBEdgeIntersection(r_circle, lineSeg);
		}
		else if (r_circle.center.x > r_AABB.max.x) // right side
		{
			LineSegment lineSeg{ r_AABB.max, vec2{r_AABB.max.x, r_AABB.min.y} };
			collided += CircleAABBEdgeIntersection(r_circle, lineSeg);
		}
		else
		{
			collided = 0;
		}

		if (r_circle.center.y >= r_AABB.min.y && r_circle.center.y <= r_AABB.max.y) // if circle center is within the AABB's y range
		{
			collided += 1;
		}
		else if (r_circle.center.y < r_AABB.min.y) // bottom side
		{
			LineSegment lineSeg{ vec2{r_AABB.max.x, r_AABB.min.y}, r_AABB.min };
			collided += CircleAABBEdgeIntersection(r_circle, lineSeg);
		}
		else if (r_circle.center.y > r_AABB.max.y) // top side
		{
			LineSegment lineSeg{ vec2{r_AABB.min.x, r_AABB.max.y}, r_AABB.max };
			collided += CircleAABBEdgeIntersection(r_circle, lineSeg);
		}
		else
		{
			collided = collided;
		}
		
		if (collided >= 2)
		{
			if ((r_AABB.center - r_circle.center).LengthSquared() == 0.f)
			{
				r_contactPt.normal = vec2{ 0.f, 1.f };
				r_contactPt.intersectionPoint = vec2{ r_AABB.center.x, r_AABB.max.y };
				r_contactPt.penetrationDepth = r_circle.radius;
			}
			else
			{
				r_contactPt.intersectionPoint = r_circle.center;
				Clamp(r_contactPt.intersectionPoint.x, r_AABB.min.x, r_AABB.max.x);
				Clamp(r_contactPt.intersectionPoint.y, r_AABB.min.y, r_AABB.max.y);
				vec2 pseudoNormal = r_circle.center - r_contactPt.intersectionPoint;
				if (Dot(pseudoNormal, pseudoNormal) != 0.f)
				{
					r_contactPt.normal = pseudoNormal.GetNormalized();
					r_contactPt.penetrationDepth = r_circle.radius - (r_circle.center - r_contactPt.intersectionPoint).Length();
				}
				else
				{
					r_contactPt.normal = (r_circle.center - r_AABB.center).GetNormalized();
					r_contactPt.penetrationDepth = r_circle.radius;
				}
			}
		}
		return (collided >= 2);
	}

	// Circle + AABB Edge
	int CircleAABBEdgeIntersection(CircleCollider const& r_circle, LineSegment const& r_lineSeg)
	{
		// Static Collision
		float const check = Dot(r_lineSeg.normal, r_lineSeg.point0 - r_circle.center);
		if (check <= r_circle.radius)
		{
			float innerProduct = ((r_circle.center.x - r_lineSeg.point0.x) * r_lineSeg.lineVec.x) + ((r_circle.center.y - r_lineSeg.point0.y) * r_lineSeg.lineVec.y);
			if (0 <= innerProduct && innerProduct <= r_lineSeg.lineVec.LengthSquared()) // check if the circle's centre, if projected onto the line segment, would be within it
			{
				return 1;
			}
			else // checks for edges
			{
				float p0CenterLengthSqr = (r_lineSeg.point0 - r_circle.center).LengthSquared();
				float p1CenterLengthSqr = (r_lineSeg.point1 - r_circle.center).LengthSquared();
				float radiusSqr = r_circle.radius * r_circle.radius;
				return ((p0CenterLengthSqr <= radiusSqr) + (p1CenterLengthSqr <= radiusSqr));
			}
		}
		return 0;
	}
}

/* 
Physics + Collision Tentative Loop
Update Dynamics
TestColliders
ResolveCollision / Update Position
Update Colliders
*/


// Circle Circle Extra
/*else
		{
			// Dynamic Collision Check

			vec2 const& startPos_e1 = g_entityManager->Get<RigidBody>(r_entity1).m_prevPosition;
			vec2 const& endPos_e1 = r_circle1.center;
			//vec2 const& v_e1 = endPos_e1 - startPos_e1;

			vec2 const& startPos_e2 = g_entityManager->Get<RigidBody>(r_entity2).m_prevPosition;
			vec2 const& endPos_e2 = r_circle2.center;
			//vec2 const& v_e2 = endPos_e2 - startPos_e2;

			vec2 const v = (endPos_e1 - startPos_e1) - (endPos_e2 - startPos_e2); // v = v1 - v2
			vec2 const s = startPos_e1 - startPos_e2;
			
			float eqnC = Dot(s, s) - (totalRadius * totalRadius);
			if (eqnC < 0.f)
			{
				
			}

			// quadratic equation to solve for t
			float eqnA = Dot(v, v);
			if (eqnA < std::numeric_limits<float>::epsilon()) 
			{ return false; } // not moving relative to each other
			
			float eqnB = Dot(v, s);
			if (eqnB >= 0.f)
			{ return false; } // not moving towards each other

			
			
			// checks if it has root values;
			float discriminant = 
		}*/

// CircleAABBEdge Extra
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
			}
		else
		{
			return false; // call check line edges function here with true
		}*/
