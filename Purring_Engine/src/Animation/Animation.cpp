/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Animation.cpp
 \date     01-11-2023

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
#include "Math/MathCustom.h"
#include "Logging/Logger.h"
#include "ECS/SceneView.h"
#include "ECS/Entity.h"
#include "Graphics/Renderer.h"
#include "ResourceManager/ResourceManager.h"
#include "Animation.h"


#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE


extern Logger engine_logger;

namespace PE
{
	Animation::Animation() : m_currentFrameIndex{ 0 }, m_currentFrameTime{ 0.0f }
	{

	}

	Animation::Animation(std::string spriteSheetKey) : m_textureKey{ spriteSheetKey }, m_currentFrameIndex { 0 }, m_currentFrameTime{ 0.0f }
	{
	
	}

	void Animation::AddFrame(vec2 const& minUV, vec2 const& maxUV, float duration)
	{
		m_animationFrames.emplace_back(AnimationFrame{ minUV, maxUV, duration });
	}

	AnimationFrame const& Animation::UpdateAnimationFrame(float deltaTime, AnimationComponent& r_animationComponent)
	{
		r_animationComponent.SetCurrentFrameTime(r_animationComponent.GetCurrentFrameTime() + deltaTime);
		if (r_animationComponent.GetCurrentFrameTime() >= m_animationFrames[r_animationComponent.GetCurrentFrameIndex()].m_duration)
		{
			// move on the the next frame when current frame duration is reached
			r_animationComponent.SetCurrentFrameIndex((r_animationComponent.GetCurrentFrameIndex() + 1) % m_animationFrames.size());
			r_animationComponent.SetCurrentFrameTime(0.0f);
		}

		return m_animationFrames[r_animationComponent.GetCurrentFrameIndex()];
	}

	void Animation::ResetAnimation()
	{
		m_currentFrameIndex = 0;
	}

	// AnimationComponent
	void AnimationComponent::AddAnimationToComponent(std::string animationID)
	{
		m_animationsID.emplace_back(animationID);
		m_currentAnimationIndex = animationID;
	}

	void AnimationComponent::SetCurrentAnimationIndex(std::string animationIndex)
	{
		m_currentAnimationIndex = animationIndex;
	}

	nlohmann::json AnimationComponent::ToJson(size_t id) const
	{
		id;
		nlohmann::json j;
		j["CurrentAnimationIndex"] = m_currentAnimationIndex;
		return j;
	}

	AnimationComponent& AnimationComponent::Deserialize(const nlohmann::json& j)
	{
		m_currentAnimationIndex = j["CurrentAnimationIndex"].get<std::string>();
		return *this;
	}

	// AnimationManager
	void AnimationManager::InitializeSystem()
	{
		// Check if Initialization was successful
		engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
		engine_logger.SetTime();
		engine_logger.AddLog(false, "AnimationManager initialized!", __FUNCTION__);

		// Create animations here for now
		std::string playerWalkAnimation, playerAttackAnimation, ratAttackAnimation, ratDeathAnimation;
		playerWalkAnimation   = CreateAnimation("playerWalk");
		playerAttackAnimation = CreateAnimation("playerAttack");
		ratAttackAnimation	  = CreateAnimation("ratAttack");
		ratDeathAnimation	  = CreateAnimation("ratDeath");

		// animation 1
		AddFrameToAnimation(playerWalkAnimation, vec2{ 0.f, 0.f }, vec2{ 1.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(playerWalkAnimation, vec2{ 1.f / 6.f, 0.f }, vec2{ 2.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(playerWalkAnimation, vec2{ 2.f / 6.f, 0.f }, vec2{ 3.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(playerWalkAnimation, vec2{ 3.f / 6.f, 0.f }, vec2{ 4.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(playerWalkAnimation, vec2{ 4.f / 6.f, 0.f }, vec2{ 5.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(playerWalkAnimation, vec2{ 5.f / 6.f, 0.f }, vec2{ 1.f, 1.f }, 1.f / 6.f);

		// animation 2
		AddFrameToAnimation(playerAttackAnimation, vec2{ 0.f, 0.f }, vec2{ 1.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(playerAttackAnimation, vec2{ 1.f / 6.f, 0.f }, vec2{ 2.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(playerAttackAnimation, vec2{ 2.f / 6.f, 0.f }, vec2{ 3.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(playerAttackAnimation, vec2{ 3.f / 6.f, 0.f }, vec2{ 4.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(playerAttackAnimation, vec2{ 4.f / 6.f, 0.f }, vec2{ 5.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(playerAttackAnimation, vec2{ 5.f / 6.f, 0.f }, vec2{ 1.f, 1.f }, 1.f / 6.f);

		// animation 3
		AddFrameToAnimation(ratAttackAnimation, vec2{ 0.f, 0.f }, vec2{ 1.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(ratAttackAnimation, vec2{ 1.f / 6.f, 0.f }, vec2{ 2.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(ratAttackAnimation, vec2{ 2.f / 6.f, 0.f }, vec2{ 3.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(ratAttackAnimation, vec2{ 3.f / 6.f, 0.f }, vec2{ 4.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(ratAttackAnimation, vec2{ 4.f / 6.f, 0.f }, vec2{ 5.f / 6.f, 1.f }, 1.f / 6.f);
		AddFrameToAnimation(ratAttackAnimation, vec2{ 5.f / 6.f, 0.f }, vec2{ 1.f, 1.f }, 1.f / 6.f);

		// animation 4
		AnimationManager::AddFrameToAnimation(ratDeathAnimation, vec2{ 0.f, 0.f }, vec2{ 1.f / 6.f, 1.f }, 1.f / 6.f);
		AnimationManager::AddFrameToAnimation(ratDeathAnimation, vec2{ 1.f / 6.f, 0.f }, vec2{ 2.f / 6.f, 1.f }, 1.f / 6.f);
		AnimationManager::AddFrameToAnimation(ratDeathAnimation, vec2{ 2.f / 6.f, 0.f }, vec2{ 3.f / 6.f, 1.f }, 1.f / 6.f);
		AnimationManager::AddFrameToAnimation(ratDeathAnimation, vec2{ 3.f / 6.f, 0.f }, vec2{ 4.f / 6.f, 1.f }, 1.f / 6.f);
		AnimationManager::AddFrameToAnimation(ratDeathAnimation, vec2{ 4.f / 6.f, 0.f }, vec2{ 5.f / 6.f, 1.f }, 1.f / 6.f);
		AnimationManager::AddFrameToAnimation(ratDeathAnimation, vec2{ 5.f / 6.f, 0.f }, vec2{ 1.f, 1.f }, 1.f / 6.f);
	}

	void AnimationManager::UpdateSystem(float deltaTime)
	{
		AnimationFrame p_currentFrame;

#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
		{
#endif
			for (EntityID const& id : SceneView<AnimationComponent>())
			{
					// update animation and get current frame
					p_currentFrame = UpdateAnimation(id, deltaTime);

					// update entity based on frame data
					// in the future probably check for bools in animation component, then update data accordingly

					if (EntityManager::GetInstance().Has<Graphics::Renderer>(id))
					{
						//EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetTextureKey(GetAnimationSpriteSheetKey(animationComponent.GetAnimationID()));
						EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetUVCoordinatesMin(p_currentFrame.m_minUV);
						EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetUVCoordinatesMax(p_currentFrame.m_maxUV);
					}
			}
#ifndef GAMERELEASE
		}
#endif
	}

	void AnimationManager::DestroySystem()
	{

	}

	std::string AnimationManager::CreateAnimation(std::string animationID)
	{
		ResourceManager::GetInstance().Animations[animationID] = std::make_shared<Animation>();
		return animationID;
	}

	void AnimationManager::AddFrameToAnimation(std::string animationID, vec2 const& minUV, vec2 const& maxUV, float duration)
	{
		if (ResourceManager::GetInstance().Animations.find(animationID) != ResourceManager::GetInstance().Animations.end())
		{
			ResourceManager::GetInstance().Animations[animationID]->AddFrame(minUV, maxUV, duration);
		}
	}

	AnimationFrame AnimationManager::UpdateAnimation(EntityID id, float deltaTime)
	{
		AnimationComponent& animationComponent = EntityManager::GetInstance().Get<AnimationComponent>(id);

		// store animations in resource manager instead
		if (ResourceManager::GetInstance().Animations.find(animationComponent.GetAnimationID()) != ResourceManager::GetInstance().Animations.end())
		{
			return ResourceManager::GetInstance().Animations[animationComponent.GetAnimationID()]->UpdateAnimationFrame(deltaTime, animationComponent);
		}
		return AnimationFrame{};
	}

	std::string AnimationManager::GetAnimationSpriteSheetKey(std::string animationID)
	{
		// store animations in resource manager instead
		if (ResourceManager::GetInstance().Animations.find(animationID) != ResourceManager::GetInstance().Animations.end())
		{
			return ResourceManager::GetInstance().Animations[animationID]->GetSpriteSheetKey();
		}
		return "";
	}
}
