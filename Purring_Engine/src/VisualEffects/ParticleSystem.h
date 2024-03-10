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
namespace PE
{
	// component assigned to an entity to make it spit out particles
	class ParticleSystem
	{
	public:
		ParticleSystem();
		ParticleSystem(ParticleSystem const& r_cpy);
		ParticleSystem& operator=(ParticleSystem const& r_cpy);
		void Update(float deltaTime);
		void CreateAllParticles(EntityID emitterID);
		void ResetAllParticles(); // resets emission duration, lifetime of each particle and the position of each particle
		
	public:
		// max number of particles
		unsigned maxParticles;

		// turn particle system on and off
		bool isActive;

		// toggle whether particle system loops
		bool isLooping;

		// type of particle
		EnumParticleType particleType;

		// emission variables
		unsigned emissionRate; // particles per second
		float emissionDuration; // should not be used if looping
		float emissionDirection; // in radians - East: 0, North: PI/2, West: PI, South: 3*PI/2
		float emissionArc; // 'angle' of the cone (360 for circle around entity etc.)
		float startEmissionRadius; // idk how to explain (start position of each particle will be more varying ?)

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
	
	private:
		// Random value generators
		vec2 GeneratePosition(float radius);
		vec2 GenerateDirectionVector(vec2 const& r_startPosition);

	private:
		// vector of particles belonging to the entity with this system
		std::vector<Particle> particles;

		float orientationChangeSpeed;
		vec2 scaleChangeSpeed;
		vec4 colorChangeSpeed;
		vec2 emissionVector;

		// track how long particle system has been emitting particles
		float emissionElapsed;
		unsigned existingParticles;

		// random seed
		std::random_device seed;
	};


}