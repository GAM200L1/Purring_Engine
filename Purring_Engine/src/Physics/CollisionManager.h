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
#include "ECS/Components.h"

namespace PE
{
	class CollisionManager
	{
	public:

		// ----- Removed Copy Ctors/Assignments ----- //
		CollisionManager(CollisionManager const& r_cpyCollisionManager) = delete;
		CollisionManager& operator=(CollisionManager const& r_cpyCollisionManager) = delete;

		// ----- Public Getters ----- //
		CollisionManager* GetInstance();

		// ----- Public Methods ----- //
		

	private:
		CollisionManager* m_ptrInstance;
		float m_worldGravity;
	};

	// ----- Rectangle Collisions ----- //
	bool CheckCollision(AABB const& r_aabb1, vec2 const& r_vel1, AABB const& r_aabb2, vec2 const& r_vel2);


	// ----- Circle Collisions ----- //
	//bool CircleAndLineSegmentCollision(BoundingCircle const& r_circle, vec2 const& r_ptEnd, Line)
}