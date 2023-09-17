/*!*****************************************************************************
	@file       Colliders.h
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
#include "ECS/Components.h"


namespace PE
{
	struct Collider : public Component
	{
		virtual bool TestCollision(Collider& r_collisionObj) = 0;
		virtual bool TestCollision(CircleCollider& r_collisionCircle) = 0;
		virtual bool TestCollision(AABBCollider& r_collisionBox) = 0;
	};

	// may change this to box collider to combine obb and aabb in the future
	struct AABBCollider : public Collider
	{
		// ----- Public Variables ----- //
		vec2 min;
		vec2 max;
		// ----- Public Methods ----- //
		bool TestCollision(Collider& r_collisionObj);
		bool TestCollision(CircleCollider& r_collisionCircle);
		bool TestCollision(AABBCollider& r_collisionBox);
	};

	struct CircleCollider : public Collider
	{
		// ----- Public Variables ----- //
		vec2 center;
		float radius;
		// ----- Public Methods ----- //
		bool TestCollision(Collider& r_collisionObj);
		bool TestCollision(CircleCollider& r_collisionCircle);
		bool TestCollision(AABBCollider& r_collisionBox);
	};
}