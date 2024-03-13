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
#include "Logging/Logger.h"
#define M_PI           3.14159265358979323846f // temp definition of pi, will need to discuss where shld we leave this later on

extern Logger engine_logger;

namespace PE
{
	ParticleEmitter::ParticleEmitter() :
		maxParticles{ DEFAULT_MAX_PARTICLES }, isActive{ false }, isLooping{ false }, particleType{ SQUARE },
		emissionRate{ 1 }, emissionDuration{ 5.f }, startLifetime{ 1.f }, startSpeed{ 1.f },
		startDelay{ 0.f }, startRotation{ 0.f }, startScale{ 1.f, 1.f }, startColor{ 1.f,1.f,1.f,1.f },
		endDelay{ 0.f }, endRotation{ 0.f }, endScale{ 1.f, 1.f }, endColor{ 1.f,1.f,1.f,1.f },
		emissionDirection{ 0.f }, emissionVector{ sinf(emissionDirection), cosf(emissionDirection) },
		emissionArc{ M_PI }, startEmissionRadius{ 1.f }, emissionElapsed{ emissionDuration },
		randomnessFactor{ 0.f }
	{
		emissionVector.Normalize();
		orientationChangeSpeed = (endRotation - startRotation) / emissionDuration;
		//scaleChangeSpeed = (endScale - startScale) / emissionDuration;
		colorChangeSpeed = (endColor - startColor) / emissionDuration;
	}

	ParticleEmitter::ParticleEmitter(ParticleEmitter const& r_cpy) :
		maxParticles{ r_cpy.maxParticles }, isActive{ r_cpy.isActive }, isLooping{ r_cpy.isLooping }, particleType{ r_cpy.particleType },
		emissionRate{ r_cpy.emissionRate }, emissionDuration{ r_cpy.emissionDuration }, startLifetime{ r_cpy.startLifetime }, startSpeed{ r_cpy.startSpeed },
		startDelay{ r_cpy.startDelay }, startRotation{ r_cpy.startRotation }, startScale{ r_cpy.startScale }, startColor{ r_cpy.startColor },
		endDelay{ r_cpy.endDelay }, endRotation{ r_cpy.endRotation }, endScale{ r_cpy.endScale }, endColor{ r_cpy.endColor },
		emissionDirection{ r_cpy.emissionDirection }, emissionVector{ sinf(emissionDirection), cosf(emissionDirection) },
		emissionArc{ r_cpy.emissionArc }, startEmissionRadius{ r_cpy.startEmissionRadius }, emissionElapsed{ emissionDuration },
		randomnessFactor{ r_cpy.randomnessFactor }
		{
			emissionVector.Normalize();
			orientationChangeSpeed = (endRotation - startRotation) / emissionDuration;
			scaleChangeSpeed = (endScale - startScale) / emissionDuration;
			colorChangeSpeed = (endColor - startColor) / emissionDuration;
		}

	ParticleEmitter& ParticleEmitter::operator=(ParticleEmitter const& r_cpy)
	{
		maxParticles = r_cpy.maxParticles , isActive = r_cpy.isActive , isLooping = r_cpy.isLooping , particleType = r_cpy.particleType ,
		emissionRate = r_cpy.emissionRate , emissionDuration = r_cpy.emissionDuration , startLifetime = r_cpy.startLifetime , startSpeed = r_cpy.startSpeed ,
		startDelay = r_cpy.startDelay , startRotation = r_cpy.startRotation , startScale = r_cpy.startScale , startColor = r_cpy.startColor ,
		endDelay = r_cpy.endDelay , endRotation = r_cpy.endRotation , endScale = r_cpy.endScale , endColor = r_cpy.endColor ,
		emissionDirection = r_cpy.emissionDirection, emissionVector = vec2{ sinf(emissionDirection), cosf(emissionDirection) },
		emissionArc = r_cpy.emissionArc , startEmissionRadius = r_cpy.startEmissionRadius , emissionElapsed = emissionDuration ,
		randomnessFactor = r_cpy.randomnessFactor ;
		return *this;
	}

	void ParticleEmitter::Update(float deltaTime)
	{
		// if at any point particle system becomes inactive, reset the particles
		static bool particlesResetted{ false };
		if (!isActive)
		{
			if (!particlesResetted)
				ResetAllParticles();
			particlesResetted = true;
			return;
		}
		particlesResetted = false;

		// if emission duration finished
		//if (emissionElapsed <= 0.f)
		//{
		//	if (!isLooping)
		//		return;
		//	else // if looping
		//		emissionElapsed = emissionDuration;
		//}
		//emissionElapsed -= deltaTime;
		
		for (Particle& r_particle : particles)
		{
			if (r_particle.lifetime <= 0.f) {

				// when lifetime of individual particle is depleted reset lifetime
				//vec2 const& r_particleStartPosition{ GeneratePosition(startEmissionRadius) };
				//r_particle.Reset(r_particleStartPosition, GenerateDirectionVector(r_particleStartPosition), startScale.x, startScale.y, startRotation, startLifetime);
			}
			else
			{
				if (!r_particle.Update(deltaTime))
				{
					if (isLooping)
					{
						// reset the particle
						auto pos = GeneratePosition(startEmissionRadius);
						std::mt19937 generator(seed());
						std::uniform_real_distribution<float> distributor(minMaxSpeed.x, minMaxSpeed.y);
						std::uniform_real_distribution<float> distributorScale(endScale.Length(), startScale.Length());
						if (EntityManager::GetInstance().Has<AnimationComponent>(m_id))
						{
							std::uniform_int_distribution<unsigned> distributorAnim(0U, EntityManager::GetInstance().Get<AnimationComponent>(m_id).GetAnimation()->GetFrameCount());
							r_particle.Reset(particleType, pos, vec2(1.f, 1.f) * distributorScale(generator), GenerateDirectionVector(pos), scaleChangeSpeed, orientationChangeSpeed, distributor(generator), startLifetime, distributorAnim(generator));
						}
						else
						{
							r_particle.Reset(particleType, pos, vec2(1.f, 1.f) * distributorScale(generator), GenerateDirectionVector(pos), scaleChangeSpeed, orientationChangeSpeed, distributor(generator), startLifetime);
						}
					}
					else
					{
						r_particle.enabled = false;
					}
				}
				
				//r_particle.UpdatePosition(startSpeed * deltaTime);
				//r_particle.UpdateRotation(orientationChangeSpeed * deltaTime);
				//r_particle.UpdateScale(scaleChangeSpeed.x * deltaTime, scaleChangeSpeed.y * deltaTime);
				//r_particle.UpdateColor(colorChangeSpeed * deltaTime);
			}
		}
	}

	void ParticleEmitter::CreateAllParticles()
	{
		particles.clear();
		particles.reserve(maxParticles);
		const auto& xform = EntityManager::GetInstance().Get<Transform>(m_id);
		for (unsigned i = 0; i < maxParticles; i++)
		{
			auto pos = GeneratePosition(startEmissionRadius);
			std::mt19937 generator(seed());
			std::uniform_real_distribution<float> distributor(minMaxSpeed.x, minMaxSpeed.y);
			std::uniform_real_distribution<float> distributorScale(endScale.Length(), startScale.Length());
			particles.emplace_back(Particle(particleType, pos, vec2(1.f,1.f) * distributorScale(generator), GenerateDirectionVector(pos), scaleChangeSpeed, orientationChangeSpeed, distributor(generator), startLifetime));
			if (EntityManager::GetInstance().Has<AnimationComponent>(m_id))
			{
				std::uniform_int_distribution<unsigned> distributorAnim(0U, EntityManager::GetInstance().Get<AnimationComponent>(m_id).GetAnimation()->GetFrameCount());
				particles.emplace_back(Particle(particleType, pos, vec2(1.f, 1.f) * distributorScale(generator), GenerateDirectionVector(pos), scaleChangeSpeed, orientationChangeSpeed, distributor(generator), startLifetime, distributorAnim(generator)));
			}
			else
			{
				particles.emplace_back(Particle(particleType, pos, vec2(1.f, 1.f) * distributorScale(generator), GenerateDirectionVector(pos), scaleChangeSpeed, orientationChangeSpeed, distributor(generator), startLifetime));
			}
		}
	}

	void ParticleEmitter::ResetAllParticles()
	{
		for (Particle& r_particle : particles)
		{
			//vec2 const& r_particleStartPosition{ GeneratePosition(startEmissionRadius) };
			//r_particle.Reset(r_particleStartPosition, GenerateDirectionVector(r_particleStartPosition), startScale.x, startScale.y, startRotation, startLifetime);
			//r_particle.enabled = true;
			auto pos = GeneratePosition(startEmissionRadius);
			std::mt19937 generator(seed());
			std::uniform_real_distribution<float> distributor(minMaxSpeed.x, minMaxSpeed.y);
			std::uniform_real_distribution<float> distributorScale(endScale.Length(), startScale.Length());
			if (EntityManager::GetInstance().Has<AnimationComponent>(m_id))
			{
				std::uniform_int_distribution<unsigned> distributorAnim(0U, EntityManager::GetInstance().Get<AnimationComponent>(m_id).GetAnimation()->GetFrameCount());
				r_particle.Reset(particleType, pos, vec2(1.f, 1.f) * distributorScale(generator), GenerateDirectionVector(pos), scaleChangeSpeed, orientationChangeSpeed, distributor(generator), startLifetime, distributorAnim(generator));
			}
			else
			{
				r_particle.Reset(particleType, pos, vec2(1.f, 1.f) * distributorScale(generator), GenerateDirectionVector(pos), scaleChangeSpeed, orientationChangeSpeed, distributor(generator), startLifetime);
			}
		}
	}

	vec2 ParticleEmitter::GeneratePosition(float radius)
	{
		if (radius == 0.f)
			return vec2{ 0.f, 0.f };
		
		// get the direction vector to the left and right of the emission direction
		const auto& xform = EntityManager::GetInstance().Get<Transform>(m_id);
		emissionVector = xform.position;
		vec2 rightVector{ emissionVector.y, -emissionVector.x };
		vec2 leftVector{ -emissionVector.y, emissionVector.x };

		/*std::mt19937 generator(seed());

		if (std::rand() % 2)
		{
			std::uniform_real_distribution<float> distributor(0.f, (rightVector * radius).Length());
			return rightVector * distributor(generator);
		}
		else
		{
			std::uniform_real_distribution<float> distributor(0.f, (leftVector * radius).Length());
			return leftVector * distributor(generator);
		}*/
		return xform.position;
	}

	vec2 ParticleEmitter::GenerateDirectionVector(vec2 const& r_startPosition)
	{
		// internal formula is to get the end of the particle emission's 'radius'

		std::mt19937 generator(seed());
		const auto& xform = EntityManager::GetInstance().Get<Transform>(m_id);
		float trueDir = emissionDirection - xform.orientation;
		std::uniform_real_distribution<float> distributor(trueDir - emissionArc * 0.5f, trueDir + emissionArc * 0.5f);
		float gen = distributor(generator);
		vec2 dir{ sinf(gen), cosf(gen) };
		dir.Normalize();

		return dir;
	}
}