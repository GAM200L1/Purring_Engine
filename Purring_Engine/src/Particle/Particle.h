#pragma once
#include "ECS/Entity.h"
#include "ECS/EntityFactory.h"
#include "Graphics/Renderer.h"

namespace PE
{
	enum EnumParticleType
	{
		RANDOM, // mixes both square and circle
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

		EnumParticleType type = RANDOM;

		void CreateParticle(EntityID particleId, EntityID spawnerId, EnumParticleType particleType, float startParticleOrientation, float particleWidth, float particleHeight, float startLifetime)
		{
			particleID = particleId;
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
			EntityManager::GetInstance().Get<EntityDescriptor>(particleID).parent = spawnerId;
			
			// Sets the rotation of the particle
			EntityManager::GetInstance().Get<Transform>(particleID).relOrientation = startParticleOrientation;
			
			// Sets the starting size of the particle
			EntityManager::GetInstance().Get<Transform>(particleID).width = particleWidth;
			EntityManager::GetInstance().Get<Transform>(particleID).height = particleHeight;

			// Sets the lifetime of the particle
			lifetime = startLifetime;
		}

		void ResetParticle(float particleWidth, float particleHeight, float startLifetime)
		{
			// Sets the starting size of the particle
			EntityManager::GetInstance().Get<Transform>(particleID).width = particleWidth;
			EntityManager::GetInstance().Get<Transform>(particleID).height = particleHeight;

			// Sets the lifetime of the particle
			lifetime = startLifetime;
		}

		void UpdatePosition(vec2 const& r_positionDifference)
		{
			EntityManager::GetInstance().Get<Transform>(particleID).relPosition += r_positionDifference;
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

		void UpdateColor(float redDifference, float greenDifference, float blueDifference, float alphaDifference)
		{
			glm::vec4 const& particleColor = EntityManager::GetInstance().Get<Graphics::Renderer>(particleID).GetColor();
			EntityManager::GetInstance().Get<Graphics::Renderer>(particleID).SetColor(particleColor.x + redDifference, particleColor.y + greenDifference, particleColor.z + blueDifference, particleColor.w + alphaDifference);
		}

		// void UpdateAnimation()
	};
}