/*!*****************************************************************************
	@file       Collision.h
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#pragma once
#include "../MathCustom.h"
#include "../Entity/Components.h"

struct AABB : public Component
{
	vec2 min;
	vec2 max;
};

struct BoundingCircle : public Component
{
	vec2 m_center;
	float m_radius;
};

// do not do this for m1
struct OBB
{

};

// ----- Rectangle Collisions ----- //
bool RectangleAndRectangleCollision(AABB const& r_aabb1, vec2 const& r_vel1, AABB const& r_aabb2, vec2 const& r_vel2);


// ----- Circle Collisions ----- //
//bool CircleAndLineSegmentCollision(BoundingCircle const& r_circle, vec2 const& r_ptEnd, Line)