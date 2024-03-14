/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     ParticleSystem.h
 \date     15-12-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include <random>
#include "Math/MathCustom.h"
#include "Particle.h"
#include "Singleton.h"
#include "Layers/LayerManager.h"
#include "Data/json.hpp"

constexpr size_t DEFAULT_MAX_PARTICLES = 100;


namespace PE
{
	enum EnumEmitterType
	{
		POINT = 0,
		LINE = 1,
		NUM_EMITTOR
	};

	// component assigned to an entity to make it spit out particles
	class ParticleEmitter
	{
	public:
		ParticleEmitter();
		ParticleEmitter(ParticleEmitter const& r_cpy);
		ParticleEmitter& operator=(ParticleEmitter const& r_cpy);
		void Update(float deltaTime);
		void CreateAllParticles();
		void ResetAllParticles(); // resets emission duration, lifetime of each particle and the position of each particle
		const std::vector<Particle>& GetParticles() const { return particles; }
		void SetParent(size_t id) { m_id = id; }
		/*!***********************************************************************************
		 \brief Serializes this struct into a json file

		 \param[in] id 				Entity ID of who owns this descriptor struct
		 \return nlohmann::json 	The generated json
		*************************************************************************************/
		nlohmann::json ToJson(size_t id) const;


	public:
		// max number of particles
		unsigned maxParticles;

		// turn particle system on and off
		bool isActive;

		// toggle whether particle system loops
		bool isLooping;

		// type of particle
		EnumParticleType particleType;
		// type of emitter
		EnumEmitterType emitterType{ POINT };

		// emission variables
		unsigned emissionRate; // particles per second
		float emissionDuration; // should not be used if looping
		float emissionDirection; // in radians - East: 0, North: PI/2, West: PI, South: 3*PI/2
		float emissionArc; // 'angle' of the cone (360 for circle around entity etc.)
		float startEmissionRadius; // idk how to explain (start position of each particle will be more varying ?)

		float emittorLength{ 0.f };

		// how long each particle should exist
		float startLifetime;
		
		// how often and how much the particles will stray off an intended path
		float randomnessFactor;

		// on spawn variables
		float startDelay; // particles appear later after entity is active check if needed
		float startSpeed;
		float startRotation;
		vec2 startScale; // width and height
		vec4 startColor;

		// end variables
		float endDelay; // so particles disappear later after reaching max lifetime
		float endRotation;
		vec2 endScale; // width and height
		vec4 endColor;
		vec2 minMaxSpeed;
		vec2 scaleChangeSpeed{ -10.f, -10.f };
		std::map<std::string, bool> toggles;
	private:
		// Random value generators
		vec2 GeneratePosition(float radius);
		vec2 GenerateDirectionVector(vec2 const& r_startPosition);

	private:
		// vector of particles belonging to the entity with this system
		std::vector<Particle> particles;

		float orientationChangeSpeed;
		
		vec4 colorChangeSpeed;
		vec2 emissionVector;

		// track how long particle system has been emitting particles
		float emissionElapsed;
		unsigned existingParticles;

		// random seed
		std::random_device seed;

		

		// Owner's ID
		size_t m_id;

		bool initPos{ false };
	};

	// not a true system, it is called by the VisualEffectsManager
	/*class ParticleSubSystem
	{
		bool enabled{ true };
	public:
		void Update(const float& r_dt)
		{
			if (!enabled)
				return;
			for (const auto& layer : LayerView<ParticleEmitter>())
			{
				for (const auto& id : InternalView(layer))
				{
					EntityManager::GetInstance().Get<ParticleEmitter>(id).Update(r_dt);
				}
			}
		}
	};*/
	
}