/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Animation.cpp
 \date     25-09-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief	   This file contains the implementation of the Animation and AnimationManager 
           classes. Animation handles the logic for updating and maintaining individual 
           animation frames, while AnimationManager manages multiple Animation objects,
           offering an interface for creating, updating, and adding frames to animations.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "Animation.h"
#include "Logging/Logger.h"
#include "ECS/SceneView.h"
#include "ECS/Entity.h"
#include "Graphics/Renderer.h"
#include "ResourceManager/ResourceManager.h"

extern Logger engine_logger;

namespace PE
{
	Animation::Animation() : m_currentFrameIndex{ 0 }, m_elapsedTime{ 0.0f }
	{
	
	}

	void Animation::AddFrame(std::string textureKey, float duration)
	{
		m_animationFrames.emplace_back(AnimationFrame{ textureKey, duration });
	}

	AnimationFrame const& Animation::UpdateAnimation(float deltaTime)
	{
		m_elapsedTime += deltaTime;
		if (m_elapsedTime >= m_animationFrames[m_currentFrameIndex].duration)
		{
			// move on the the next frame when current frame duration is reached
			m_currentFrameIndex = (m_currentFrameIndex + 1) % m_animationFrames.size();
			m_elapsedTime = 0.f;
		}
		
		return m_animationFrames[m_currentFrameIndex];
	}

	// AnimationComponent
	void AnimationComponent::AddAnimationID(std::string key)
	{
		m_animationsID.emplace_back(key);
	}

	// AnimationManager
	void AnimationManager::InitializeSystem()
	{
		// Check if Initialization was successful
		engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
		engine_logger.SetTime();
		engine_logger.AddLog(false, "AnimationManager initialized!", __FUNCTION__);
	}

	void AnimationManager::UpdateSystem(float deltaTime)
	{
		AnimationFrame p_currentFrame;
		for (EntityID const& id : SceneView<AnimationComponent>())
		{
			AnimationComponent const& animationComponent = EntityManager::GetInstance().Get<AnimationComponent>(id);
			
			// update animation and get current frame
			p_currentFrame = UpdateAnimation(animationComponent.GetAnimationID(), deltaTime);

			// update entity based on frame data
			// in the future probably check for bools in animation component, then update data accordingly
			EntityManager::GetInstance().GetInstance().Get<Graphics::Renderer>(id).SetTextureKey(p_currentFrame.textureKey);
		}
	}

	void AnimationManager::DestroySystem()
	{

	}

	std::string AnimationManager::CreateAnimation(std::string key)
	{
		ResourceManager::GetInstance().Animations[key] = std::make_shared<Animation>();
		return key;
	}

	void AnimationManager::AddFrameToAnimation(std::string animationID, std::string textureKey, float duration)
	{
		if (ResourceManager::GetInstance().Animations.find(animationID) != ResourceManager::GetInstance().Animations.end())
		{
			ResourceManager::GetInstance().Animations[animationID]->AddFrame(textureKey, duration);
		}
	}

	AnimationFrame AnimationManager::UpdateAnimation(std::string animationID, float deltaTime)
	{
		// store animations in resource manager instead
		if (ResourceManager::GetInstance().Animations.find(animationID) != ResourceManager::GetInstance().Animations.end())
		{
			return ResourceManager::GetInstance().Animations[animationID]->UpdateAnimation(deltaTime);
		}
		return AnimationFrame{};
	}
}
