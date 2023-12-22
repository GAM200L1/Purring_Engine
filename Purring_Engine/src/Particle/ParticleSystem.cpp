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
		startDelay{ 0.f }, startRotation{ 0.f }, startScaleMultiplier{ 1.f }, startColor{ 1.f,1.f,1.f,1.f },
		endDelay{ 0.f }, endRotation{ 0.f }, endScaleMultiplier{ 1.f }, endColor{ 1.f,1.f,1.f,1.f },
		emissionDirection{ 0.f }, emissionVector{ sinf(emissionDirection), cosf(emissionDirection) },
		emissionArc{ M_PI / 4.f }, startEmissionRadius{ 0.f }
		{
			emissionVector.Normalize();
			particles.reserve(maxParticles);
			orientationDifference = endRotation - startRotation;
			scaleDifference = endScaleMultiplier - startScaleMultiplier;
			colorDifference = endColor - startColor;
		}

	ParticleSystem::ParticleSystem(ParticleSystem const& r_cpy) :
		maxParticles{ r_cpy.maxParticles }, isActive{ r_cpy.isActive }, isLooping{ r_cpy.isLooping }, particleType{ r_cpy.particleType },
		emissionRate{ r_cpy.emissionRate }, emissionDuration{ r_cpy.emissionDuration }, startLifetime{ r_cpy.startLifetime },
		startDelay{ r_cpy.startDelay }, startRotation{ r_cpy.startRotation }, startScaleMultiplier{ r_cpy.startScaleMultiplier }, startColor{ r_cpy.startColor },
		endDelay{ r_cpy.endDelay }, endRotation{ r_cpy.endRotation }, endScaleMultiplier{ r_cpy.endScaleMultiplier }, endColor{ r_cpy.endColor },
		emissionDirection{ r_cpy.emissionDirection }, emissionVector{ sinf(emissionDirection), cosf(emissionDirection) },
		emissionArc{ r_cpy.emissionArc }, startEmissionRadius{ r_cpy.startEmissionRadius }
		{
			emissionVector.Normalize();
			for (Particle const& r_particle : r_cpy.particles) {
				particles.emplace_back(r_particle);
			}
			orientationDifference = endRotation - startRotation;
			scaleDifference = endScaleMultiplier - startScaleMultiplier;
			colorDifference = endColor - startColor;
		}

	ParticleSystem& ParticleSystem::operator=(ParticleSystem const& r_cpy)
	{
		ParticleSystem ret{ r_cpy };
		return ret;
	}

	void ParticleSystem::Update(float deltaTime)
	{
		for (Particle const& r_particle : particles)
		{

		}
	}
}