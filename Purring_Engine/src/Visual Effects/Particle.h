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

namespace PE
{
	enum EnumParticleType
	{
		SQUARE,
		CIRCLE,
		TEXTURED,
		ANIMATED
	};

	struct Particle
	{
		//EntityID spawnerID;
		EntityID particleID;
		float lifetime = 0.f;
		vec2 directionVector{ 0.f, 1.f };

		EnumParticleType type;

		Particle() = delete; // no default creation, all values to be based off an existing emitter

		Particle(EntityID spawnerId, EnumParticleType particleType)
		{
			particleID = EntityFactory::GetInstance().CreateEntity();
			type = particleType;
			
			// assigns a transform to the particle
			if (!EntityManager::GetInstance().Has<Transform>(particleID))
			{
				EntityFactory::GetInstance().Assign(particleID, { EntityManager::GetInstance().GetComponentID<Transform>() });
			}
			// assigns a renderer to the particle
			if (!EntityManager::GetInstance().Has<Graphics::Renderer>(particleID))
			{
				EntityFactory::GetInstance().Assign(particleID, { EntityManager::GetInstance().GetComponentID<Graphics::Renderer>() });
			}
			// assigns an animation component to the particle if it is animated
			if (type == EnumParticleType::ANIMATED && !EntityManager::GetInstance().Has<AnimationComponent>(particleID))
			{
				EntityFactory::GetInstance().Assign(particleID, { EntityManager::GetInstance().GetComponentID<AnimationComponent>() });
			}

			// sets parent ID as spawner ID
			Hierarchy::GetInstance().AttachChild(spawnerId, particleID);
		}

		void SetActive(bool isActive)
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(particleID).isActive = isActive;
		}

		void ResetParticle(vec2 const& r_startRelPosition, vec2 const& r_directionVector, float particleWidth, float particleHeight, float startRotation, float startLifetime)
		{
			// sets the starting position of the object relative to the position of the emitter
			EntityManager::GetInstance().Get<Transform>(particleID).relPosition = r_startRelPosition;

			// Sets the starting size of the particle
			EntityManager::GetInstance().Get<Transform>(particleID).width = particleWidth;
			EntityManager::GetInstance().Get<Transform>(particleID).height = particleHeight;

			EntityManager::GetInstance().Get<Transform>(particleID).relOrientation = startRotation;

			directionVector = r_directionVector;

			// Sets the lifetime of the particle
			lifetime = startLifetime;
		}

		void UpdatePosition(float speedPerFrame)
		{
			EntityManager::GetInstance().Get<Transform>(particleID).relPosition += directionVector * speedPerFrame;
		}

		void UpdateRotation(float rotationDifference)
		{
			EntityManager::GetInstance().Get<Transform>(particleID).relOrientation += rotationDifference;
		}

		void UpdateScale(float widthDifference, float heightDifference)
		{
			EntityManager::GetInstance().Get<Transform>(particleID).width += widthDifference;
			EntityManager::GetInstance().Get<Transform>(particleID).height += heightDifference;
		}

		void UpdateColor(vec4 const& colorDifference)
		{
			glm::vec4 const& particleColor = EntityManager::GetInstance().Get<Graphics::Renderer>(particleID).GetColor();
			EntityManager::GetInstance().Get<Graphics::Renderer>(particleID).SetColor(particleColor.x + colorDifference.x, particleColor.y + colorDifference.y, particleColor.z + colorDifference.z, particleColor.w + colorDifference.w);
		}

		// void UpdateAnimation()
	};
}