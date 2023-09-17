/*!*****************************************************************************
	@file       CollisionManager.h
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#pragma once
#include "prpch.h"
#include "ECS/Entity.h"
#include "Math/MathCustom.h"
#include "Colliders.h"

namespace PE
{
	class CollisionManager
	{
	public:

		CollisionManager() = default;

		// ----- Removed Copy Ctors/Assignments ----- //
		CollisionManager(CollisionManager const& r_cpyCollisionManager) = delete;
		CollisionManager& operator=(CollisionManager const& r_cpyCollisionManager) = delete;

		// ----- Public Getters ----- //
		CollisionManager* GetInstance();

		// ----- Public Methods ----- //
		void UpdateColliders();
		void TestColliders();
		void ResolveCollision();

	private:
		CollisionManager* m_ptrInstance;
		std::vector<Manifold> m_manifolds;
	};

	// Static + Dynamic Collision Checks
	// Rect + Rect
	bool CollisionIntersection(AABBCollider const& r_AABB1, AABBCollider const& r_AABB2, EntityID const& r_entity1, EntityID const& r_entity2, Contact& r_contactPt);
	// Circle + Circle
	bool CollisionIntersection(CircleCollider const& r_circle1, CircleCollider const& r_circle2, EntityID const& r_entity1, EntityID const& r_entity2, Contact& r_contactPt);
	// Rect + Circle
	bool CollisionIntersection(AABBCollider const& r_AABB, CircleCollider const& r_circle, EntityID const& r_entity1, EntityID const& r_entity2, Contact& r_contactPt);
	// Circle + Rect
	bool CollisionIntersection(CircleCollider const& r_circle, AABBCollider const& r_AABB, EntityID const& r_entity1, EntityID const& r_entity2, Contact& r_contactPt);
	// Circle + Line
	bool CircleLineIntersection(CircleCollider const& r_circle, LineSegment const& r_lineSeg, EntityID const& r_entity1, float& r_interTime, Contact& r_contactPt);
	
	
	// ----- Rectangle Collisions ----- //
	//bool CheckCollision(AABB const& r_aabb1, vec2 const& r_vel1, AABB const& r_aabb2, vec2 const& r_vel2);


	// ----- Circle Collisions ----- //
	//bool CircleAndLineSegmentCollision(BoundingCircle const& r_circle, vec2 const& r_ptEnd, Line)
}