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

	private:
		CollisionManager* m_ptrInstance;
	};

	// Rect + Rect
	bool CollisionIntersection(AABBCollider const& r_AABB1, AABBCollider const& r_AABB2);
	// Circle + Circle
	bool CollisionIntersection(CircleCollider const& r_circle1, CircleCollider const& r_circle2);	
	// Rect + Circle
	bool CollisionIntersection(AABBCollider const& r_AABB, CircleCollider const& r_circle);
	// Circle + Rect
	bool CollisionIntersection(CircleCollider const& r_circle, AABBCollider const& r_AABB);

	// ----- Rectangle Collisions ----- //
	//bool CheckCollision(AABB const& r_aabb1, vec2 const& r_vel1, AABB const& r_aabb2, vec2 const& r_vel2);


	// ----- Circle Collisions ----- //
	//bool CircleAndLineSegmentCollision(BoundingCircle const& r_circle, vec2 const& r_ptEnd, Line)
}