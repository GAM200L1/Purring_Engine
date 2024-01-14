/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     VisualEffectManager.cpp
 \date     15-12-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include <prpch.h>
#include "VisualEffectsManager.h"
#include "Logging/Logger.h"
#include "ECS/SceneView.h"
#include "ECS/Entity.h"
#include "ParticleSystem.h"

extern Logger engine_logger;

namespace PE
{
	void VisualEffectsManager::InitializeSystem()
	{
		// Check if Initialization was successful
		engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
		engine_logger.SetTime();
		engine_logger.AddLog(false, "VisualEffectsManager initialized!", __FUNCTION__);
	}
	
	void VisualEffectsManager::UpdateSystem(float deltaTime)
	{
		// runs particle system
		for (EntityID ParticleSystemID : SceneView<Transform, ParticleSystem>())
		{
			ParticleSystem& r_particleSystem = EntityManager::GetInstance().Get<ParticleSystem>(ParticleSystemID);

			if (!r_particleSystem.isActive)
			{
				// if particle system is not active, do not update it
				continue;
			}
			r_particleSystem.Update(deltaTime);
		}
	}
	
	void VisualEffectsManager::DestroySystem()
	{

	}
}