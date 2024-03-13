/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Particle.h
 \date     15-12-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "ECS/Entity.h"
#include "ECS/EntityFactory.h"
#include "Hierarchy/HierarchyManager.h"
#include "Graphics/Renderer.h"
#include "Physics/RigidBody.h"

namespace PE
{
	enum EnumParticleType
	{
		SQUARE,
		//CIRCLE,
		TEXTURED,
		ANIMATED,
		NUM_TYPES
	};

	struct Particle
	{
		// prevent default construcion
		Particle() = delete;

		Particle(const EnumParticleType& r_tp, 
				 const vec2& r_p, 
				 const vec2& r_scl, 
				 const vec2& r_dir, 
				 const vec2& r_dScl, 
				 const float& r_dr, 
			     const float& r_spd,
				 const float& r_lifetime,
				 const int	  r_animIdx = 0
			)
			: type(r_tp), directionVector(r_dir), deltaScale(r_dScl), deltaOrientation(r_dr),speed(r_spd), lifetime(r_lifetime)
		{
			transform.position = r_p;
			transform.height = r_scl.y;
			transform.width = r_scl.x;
			spriteID = r_animIdx;
			enabled = true;
		}
		
		void Reset(const EnumParticleType& r_tp,
				   const vec2& r_p,
				   const vec2& r_scl,
				   const vec2& r_dir,
				   const vec2& r_dScl,
				   const float& r_dr,
				   const float& r_spd,
				   const float& r_lifetime,
				   const int& r_animIdx = 0
			)
		{
			type = r_tp;
			transform.position = r_p;
			transform.height = r_scl.y;
			transform.width = r_scl.x;

			// change in postion
			directionVector = r_dir;
			// change in scale
			deltaScale = r_dScl;
			// chane in orientation
			deltaOrientation = r_dr;
			speed = r_spd;
			lifetime = r_lifetime;
			spriteID = r_animIdx;
			enabled = true;
		}

		bool Update(const float& dt)
		{
			if (!enabled)
				return false;
			transform.position += directionVector * speed * dt;
			transform.orientation += deltaOrientation * dt;
			transform.width += deltaScale.x * dt;
			transform.height += deltaScale.y * dt;

			if (type == ANIMATED)
			{
				++spriteID;
				if (spriteID > maxSpriteID)
					spriteID = 0;
			}

			if (transform.width <= 0.f || transform.height <= 0.f)
				lifetime = 0.f;
			lifetime -= dt;
			// future, update sprite id?				
			return (lifetime <= 0.f)? false : true; 
		}

		EnumParticleType type;
		Transform transform;

#ifdef ENABLE_PARTICLE_PHYSICS
		RigidBody rigidbody;
#endif // ENABLE_PARTICLE_PHYSICS

		vec2 directionVector{ 0.f, 1.f };
		vec2 deltaScale{ 0.f, 0.f };
		float deltaOrientation{ 0.f };
		float lifetime = 0.f;
		float speed = 100.f;

		int maxSpriteID{ 0 };

		int spriteID{ 0 };
		bool enabled{ false };
	};
}