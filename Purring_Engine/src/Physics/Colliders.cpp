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

}


//bool AABBCollider::TestCollision(Collider& r_collisionObj)
//{

//}
//
//bool AABBCollider::TestCollision(CircleCollider& r_collisionCircle)
//{

//}
//
//bool AABBCollider::TestCollision(AABBCollider& r_collisionBox)
//{

//}


//bool CircleCollider::TestCollision(Collider& r_collisionObj)
//{

//}
//
//bool CircleCollider::TestCollision(CircleCollider& r_collisionCircle)
//{

//}
//
//bool CircleCollider::TestCollision(AABBCollider& r_collisionBox)
//{

//}

// do not do this for m1
/*struct OBB
{

};*/