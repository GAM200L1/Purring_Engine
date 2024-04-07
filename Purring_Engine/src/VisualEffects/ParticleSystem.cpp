/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     ParticleSystem.cpp
 \date     15-12-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu
 \par      code %:     30%
 \par      changes:    Created Base defenitions and functions

 \co-author            Foong Jun Wei 
 \par      email:      f.junwei@digipen.edu
 \par      code %:     70%
 \par      changes:    Expanded on existing functions and completed functionality

 \brief 	Contains the function defenitions for the particle effect/emittor 
 			component.
	

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
		maxParticles{ DEFAULT_MAX_PARTICLES }, isActive{ true }, isLooping{ false }, particleType{ SQUARE },
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

		toggles["Emission Arc"] = true;

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
			//scaleChangeSpeed = (endScale - startScale) / emissionDuration;
			colorChangeSpeed = (endColor - startColor) / emissionDuration;
			toggles = r_cpy.toggles;
			minMaxSpeed = r_cpy.minMaxSpeed;
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
		toggles = r_cpy.toggles;
		minMaxSpeed = r_cpy.minMaxSpeed;
		return *this;
	}

	void ParticleEmitter::Update(float deltaTime)
	{
		// if at any point particle system becomes inactive, reset the particles
		if (pause)
			return;
		static bool particlesResetted{ false };
		if (!initPos)
		{
			initPos = EntityManager::GetInstance().Has<Transform>(m_id);
			if (initPos)
			{
				CreateAllParticles();
			}
			else
			{
				for (auto& r_p : particles)
				{
					r_p.enabled = false;
				}
				return;
			}
		}

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
				
			}
		}
	}

	void ParticleEmitter::CreateAllParticles()
	{
		particles.clear();
		particles.reserve(maxParticles);
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

	void ParticleEmitter::ResetAllParticles(bool enable)
	{
		for (Particle& r_particle : particles)
		{
			auto pos = GeneratePosition(startEmissionRadius);
			std::mt19937 generator(seed());
			std::uniform_real_distribution<float> distributor(minMaxSpeed.x, minMaxSpeed.y);
			std::uniform_real_distribution<float> distributorScale(endScale.Length(), startScale.Length());
			if (EntityManager::GetInstance().Has<AnimationComponent>(m_id))
			{
				std::uniform_int_distribution<unsigned> distributorAnim(0U, EntityManager::GetInstance().Get<AnimationComponent>(m_id).GetAnimation()->GetFrameCount());
				r_particle.Reset(particleType, pos, vec2(1.f, 1.f) * distributorScale(generator), GenerateDirectionVector(pos), scaleChangeSpeed, orientationChangeSpeed, distributor(generator), startLifetime, distributorAnim(generator), enable);
			}
			else
			{
				r_particle.Reset(particleType, pos, vec2(1.f, 1.f) * distributorScale(generator), GenerateDirectionVector(pos), scaleChangeSpeed, orientationChangeSpeed, distributor(generator), startLifetime, 0, enable);
			}
		}
	}

	nlohmann::json ParticleEmitter::ToJson(size_t id) const
	{
		rttr::type currType = rttr::type::get_by_name(EntityManager::GetInstance().GetComponentID<ParticleEmitter>().to_string().c_str());
		nlohmann::json ret;
		rttr::instance inst = EntityManager::GetInstance().Get<ParticleEmitter>(id);
		if (inst.is_valid())
		{
			for (auto& prop : currType.get_properties())
			{
				rttr::variant var = prop.get_value(inst);

				if (var.get_type().get_name() == "bool")
				{
					ret[prop.get_name().to_string().c_str()] = var.get_value<bool>();
				}
				else if (var.get_type().get_name() == "unsignedint")
				{
					ret[prop.get_name().to_string().c_str()] = var.get_value<unsigned>();
				}
				else if (var.get_type().get_name() == "float")
				{
					ret[prop.get_name().to_string().c_str()] = var.get_value<float>();
				}
				else if (var.get_type().get_name() == "structPE::vec2")
				{
					ret[prop.get_name().to_string().c_str()]["x"] = var.get_value<vec2>().x;
					ret[prop.get_name().to_string().c_str()]["y"] = var.get_value<vec2>().y;
				}
				else if (var.get_type().get_name() == "structPE::vec4")
				{
					ret[prop.get_name().to_string().c_str()]["x"] = var.get_value<vec4>().x;
					ret[prop.get_name().to_string().c_str()]["y"] = var.get_value<vec4>().y;
					ret[prop.get_name().to_string().c_str()]["z"] = var.get_value<vec4>().z;
					ret[prop.get_name().to_string().c_str()]["w"] = var.get_value<vec4>().w;
				}
				else if (var.get_type().get_name() == "enumPE::EnumParticleType")
				{
					ret[prop.get_name().to_string().c_str()] = var.get_value<EnumParticleType>();
				}
				else if (var.get_type().get_name() == "enumPE::EnumEmitterType")
				{
					ret[prop.get_name().to_string().c_str()] = var.get_value<EnumEmitterType>();
				}
				else if (var.get_type().get_name() == "classstd::map<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>,bool,structstd::less<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>>,classstd::allocator<structstd::pair<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>const,bool>> >")
				{
					ret[prop.get_name().to_string().c_str()] = var.get_value<std::map<std::string, bool>>();
				}
				else
				{
					std::cout << prop.get_type().get_name().to_string() << std::endl;
				}
			}
		}
		return ret;
	}

	vec2 ParticleEmitter::GeneratePosition(float radius)
	{	
		// get the direction vector to the left and right of the emission direction
		if (EntityManager::GetInstance().Has<Transform>(m_id))
			emissionVector = GenerateDirectionVector(EntityManager::GetInstance().Get<Transform>(m_id).position);
		else
		{
			initPos = false;
			return vec2{ 0.f,0.f };
		}

		std::mt19937 generator(seed());
		if (emitterType == POINT)
		{
			emittorLength = 0.f;
		}
		std::uniform_real_distribution<float> distributor(-emittorLength * 0.5f, emittorLength * 0.5f);
		const float scale = distributor(generator);
		return vec2{ EntityManager::GetInstance().Get<Transform>(m_id).position.x + (-emissionVector.y * scale), EntityManager::GetInstance().Get<Transform>(m_id).position.y + emissionVector.x * scale };
	}

	vec2 ParticleEmitter::GenerateDirectionVector(vec2 const& r_startPosition)
	{
		// internal formula is to get the end of the particle emission's 'radius'

		std::mt19937 generator(seed());
		float trueDir{};
		if (EntityManager::GetInstance().Has<Transform>(m_id))
			trueDir = emissionDirection - EntityManager::GetInstance().Get<Transform>(m_id).orientation;
		else
		{
			initPos = false;
			return vec2{ 0.f,0.f };
		}

		float gen{};
		if (!toggles.count("Emission Arc") || toggles.at("Emission Arc"))
		{
			std::uniform_real_distribution<float> distributor(trueDir - emissionArc * 0.5f, trueDir + emissionArc * 0.5f);
			gen = distributor(generator);
		}
		else
		{
			gen = trueDir;
		}
		
		vec2 dir{ sinf(gen), cosf(gen) };
		dir.Normalize();

		return dir;
	}
}