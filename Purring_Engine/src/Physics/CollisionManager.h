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
		
		// ----- Removed Copy Ctors/Assignments ----- //
		CollisionManager(CollisionManager const& r_cpyCollisionManager) = delete;
		CollisionManager& operator=(CollisionManager const& r_cpyCollisionManager) = delete;

		// ----- Public Getters ----- //
		static CollisionManager* GetInstance();

		// ----- Public Methods ----- //
		static void UpdateColliders();
		static void TestColliders();
		static void ResolveCollision();
		static void DeleteInstance();

	private:
		CollisionManager() {}
		static std::vector<Manifold> m_manifolds;
		static CollisionManager* p_instance;
	};

	// Static + Dynamic Collision Checks
	// Rect + Rect
	bool CollisionIntersection(AABBCollider const& r_AABB1, AABBCollider const& r_AABB2, Contact& r_contactPt);
	// Circle + Circle
	bool CollisionIntersection(CircleCollider const& r_circle1, CircleCollider const& r_circle2, Contact& r_contactPt);
	// Rect + Circle
	bool CollisionIntersection(AABBCollider const& r_AABB, CircleCollider const& r_circle, Contact& r_contactPt);
	// Circle + Rect
	bool CollisionIntersection(CircleCollider const& r_circle, AABBCollider const& r_AABB, Contact& r_contactPt);
	// Circle + Line
	int CircleAABBEdgeIntersection(CircleCollider const& r_circle, LineSegment const& r_lineSeg); //, float& r_interTime, Contact& r_contactPt
	
}