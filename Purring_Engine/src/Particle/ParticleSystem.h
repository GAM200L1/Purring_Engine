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
#include "Math/MathCustom.h"
#include "Particle.h"
namespace PE
{
	enum EnumEmissionShape
	{
		NONE,
		BOX,
		CONE,
		SPHERE
	};

	// component assigned to an entity to make it spit out particles
	class ParticleSystem
	{
	public:
		ParticleSystem();
		void Update();
		

	protected:
		// max number of particles
		unsigned maxParticles = 100;

		// turn particle system on and off
		bool isActive;

		// toggle whether particle system loops
		bool isLooping;

		// type of particle
		EnumParticleType particleShapes = RANDOM;

		// emission variables
		float emissionRate; // particles per second
		EnumEmissionShape emissionShape = NONE;
		float emissionDuration;
		bool infiniteEmission; // allows particles to constantly generate

		// how long each particle should exist
		float startLifetime;

		// on spawn variables
		float startDelay; // particles appear later after entity is active check if needed
		float startRotation;
		float startScale;
		float startColor;

		// end variables
		float endDelay; // so particles disappear later after reaching max lifetime
		float endRotation;
		float endScale;
		float endColor;

	private:
		// vector of particles belonging to the entity with this system
		std::vector<Particle> particles;

		// calculated variables
		vec2 positionDifference;
		float orientationDifference;
		float widthDifference, heightDifference;
	};

	// this is a problem
	class ParticleSystemManager
	{
	public:
		void Initialize();
		void Update();
	};
}