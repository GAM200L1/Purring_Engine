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
#include <variant>


namespace PE
{
	struct Collider : Component
	{
		std::variant<AABBCollider, CircleCollider> colliderVariant;
	};


	// may change this to box collider to combine obb and aabb in the future
	struct AABBCollider
	{
		// ----- Public Variables ----- //
		vec2 min;
		vec2 max;
	};
	
	void Update(AABBCollider& r_AABB, vec2 const& r_position, vec2 const& r_scale);

	struct CircleCollider
	{
		// ----- Public Variables ----- //
		vec2 center;
		float radius;
	};

	void Update(CircleCollider& r_circle, vec2 const& r_position, vec2 const& r_scale);
}



// ----- Public Methods ----- //

//void Update(vec2 const& r_position, vec2 const& r_scale);
// ----- Public Methods ----- //

//void Update(vec2 const& r_position, vec2 const& r_scale);
/*bool TestCollision(Collider& r_collisionObj);
bool TestCollision(CircleCollider& r_collisionCircle);
bool TestCollision(AABBCollider& r_collisionBox);*/

/*bool TestCollision(Collider& r_collisionObj);
bool TestCollision(CircleCollider& r_collisionCircle);
bool TestCollision(AABBCollider& r_collisionBox);*/

//struct Collider : public Component
//{
//	virtual void Update(vec2 const& r_position, vec2 const& r_scale) = 0;
//	virtual bool TestCollision(Collider& r_collisionObj) = 0;
//	virtual bool TestCollision(CircleCollider& r_collisionCircle) = 0;
//	virtual bool TestCollision(AABBCollider& r_collisionBox) = 0;
//};