/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     ParticleSystem.cpp
 \date     15-12-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "ParticleSystem.h"
# define M_PI           3.14159265358979323846 // temp definition of pi, will need to discuss where shld we leave this later on
namespace PE
{
	ParticleSystem::ParticleSystem() :
		maxParticles{ 100 }, isActive{ false }, isLooping{ false }, particleType{ SQUARE },
		emissionRate{ 1 }, emissionDuration{ 5.f }, startLifetime{ 1.f },
		startDelay{ 0.f }, startRotation{ 0.f }, startScale{ 1.f, 1.f }, startColor{ 1.f,1.f,1.f,1.f },
		endDelay{ 0.f }, endRotation{ 0.f }, endScale{ 1.f, 1.f }, endColor{ 1.f,1.f,1.f,1.f },
		emissionDirection{ 0.f }, emissionVector{ sinf(emissionDirection), cosf(emissionDirection) },
		emissionArc{ M_PI / 4.f }, startEmissionRadius{ 0.f }, emissionElapsed{ emissionDuration }
		{
			emissionVector.Normalize();
			particles.reserve(maxParticles);
			orientationChangeSpeed = (endRotation - startRotation)/emissionDuration;
			scaleChangeSpeed = (endScale - startScale) / emissionDuration;
			colorChangeSpeed = (endColor - startColor) / emissionDuration;
			seed();
		}

	ParticleSystem::ParticleSystem(ParticleSystem const& r_cpy) :
		maxParticles{ r_cpy.maxParticles }, isActive{ r_cpy.isActive }, isLooping{ r_cpy.isLooping }, particleType{ r_cpy.particleType },
		emissionRate{ r_cpy.emissionRate }, emissionDuration{ r_cpy.emissionDuration }, startLifetime{ r_cpy.startLifetime },
		startDelay{ r_cpy.startDelay }, startRotation{ r_cpy.startRotation }, startScale{ r_cpy.startScale }, startColor{ r_cpy.startColor },
		endDelay{ r_cpy.endDelay }, endRotation{ r_cpy.endRotation }, endScale{ r_cpy.endScale }, endColor{ r_cpy.endColor },
		emissionDirection{ r_cpy.emissionDirection }, emissionVector{ sinf(emissionDirection), cosf(emissionDirection) },
		emissionArc{ r_cpy.emissionArc }, startEmissionRadius{ r_cpy.startEmissionRadius }, emissionElapsed{ emissionDuration }
		{
			emissionVector.Normalize();
			for (Particle const& r_particle : r_cpy.particles) {
				particles.emplace_back(r_particle);
			}
			orientationChangeSpeed = (endRotation - startRotation) / emissionDuration;
			scaleChangeSpeed = (endScale - startScale) / emissionDuration;
			colorChangeSpeed = (endColor - startColor) / emissionDuration;
			seed();
		}

	ParticleSystem& ParticleSystem::operator=(ParticleSystem const& r_cpy)
	{
		ParticleSystem ret{ r_cpy };
		return ret;
	}

	void ParticleSystem::Update(float deltaTime)
	{
		// if emission duration finished
		if (emissionElapsed <= 0.f)
		{
			if (!isLooping)
				return;
			else // if looping
				emissionElapsed = emissionDuration;
		}
		emissionElapsed -= deltaTime;
		for (Particle& r_particle : particles)
		{
			if (r_particle.lifetime <= 0.f) {

				// when lifetime of individual particle is depleted reset lifetime
				r_particle.ResetParticle(startScale.x, startScale.y, startRotation, startLifetime);
			}
			else
			{
				if (r_particle.type == ANIMATED)
				{
					//r_particle.UpdateAnimation();
				}
				r_particle.UpdatePosition(positionChangeSpeed * deltaTime);
				r_particle.UpdateRotation(orientationChangeSpeed * deltaTime);
				r_particle.UpdateScale(scaleChangeSpeed.x * deltaTime, scaleChangeSpeed.y * deltaTime);
				r_particle.UpdateColor(colorChangeSpeed * deltaTime);
			}
		}
	}

	void ParticleSystem::ResetAllParticles()
	{
		for (Particle& r_particle : particles)
		{
			r_particle.ResetParticle(, startScale.x, startScale.y, startRotation, startLifetime);
		}
	}

	vec2 ParticleSystem::GeneratePosition()
	{
		if (startEmissionRadius == 0.f)
			return vec2{ 0.f, 0.f };
		
		// get the direction vector to the left and right of the emission direction
		vec2 rightVector{ emissionVector.y, -emissionVector.x };
		vec2 leftVector{ -emissionVector.y, emissionVector.x };

		std::mt19937 generator(seed);

		if (std::rand() % 2)
		{
			std::uniform_real_distribution<float> distributor(0.f, (rightVector * startEmissionRadius).Length());
			return rightVector * distributor(generator);
		}
		else
		{
			std::uniform_real_distribution<float> distributor(0.f, (leftVector * startEmissionRadius).Length());
			return leftVector * distributor(generator);
		}
	}

	vec2 ParticleSystem::GenerateDirectionVector()
	{
		std::mt19937 generator(seed);
		std::uniform_real_distribution<float> distributor(-emissionArc * 0.5f, emissionArc * 0.5f);

		float generatedAngle = distributor(generator);

		return vec2{ sinf(generatedAngle), cosf(generatedAngle) }.GetNormalized();
	}
}