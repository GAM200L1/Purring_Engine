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
	Animation::Animation() : m_totalSprites{ 6 }, m_animationDuration{ 1.f }
	{

	}

	Animation::Animation(std::string spriteSheetKey) : m_spriteSheetKey{ spriteSheetKey }, m_totalSprites{ 6 }, m_animationDuration { 1.f }
	{
	
	}

	void Animation::SetSpriteSheetKey(std::string spriteSheetKey)
	{
		m_spriteSheetKey = spriteSheetKey;
	}

	void Animation::AddFrame(vec2 const& minUV, vec2 const& maxUV, float duration)
	{
		m_animationFrames.emplace_back(AnimationFrame{ minUV, maxUV, duration });
	}

	void Animation::UpdateAnimationFrame(float deltaTime, float& currentFrameTime, unsigned& currentFrameIndex)
	{
		currentFrameTime += deltaTime;

		try
		{
			if (currentFrameTime >= m_animationFrames.at(currentFrameIndex).m_duration)
			{
				// move on the the next frame when current frame duration is reached
				currentFrameIndex = (currentFrameIndex + 1) % m_animationFrames.size();
				currentFrameTime = 0.f;
			}
		}
		catch (const std::out_of_range&)
		{
			return;
		}
	}

	AnimationFrame const& Animation::GetCurrentAnimationFrame(unsigned currentFrameIndex)
	{
		try
		{
			return m_animationFrames.at(currentFrameIndex);
		}
		catch (std::out_of_range const&)
		{
			return m_emptyFrame;
		}
	}

	void Animation::CreateAnimationFrames(unsigned totalSprites, float animationDuration)
	{
		m_animationFrames.clear();

		if (!totalSprites)
		{
			return;
		}

		m_totalSprites = totalSprites;
		m_animationDuration = animationDuration;

		float duration = animationDuration / static_cast<float>(totalSprites);

		for (unsigned i = 0; i < totalSprites; ++i)
		{
			AddFrame(vec2{ static_cast<float>(i) / static_cast<float>(totalSprites), 0.f },
					 vec2{ static_cast<float>(i + 1) / static_cast<float>(totalSprites), 1.f },
					 duration);
		}
	}

	void Animation::SetCurrentAnimationFrameData(unsigned currentFrameIndex, float duration)
	{
		try
		{
			m_animationFrames.at(currentFrameIndex).m_duration = duration;
		}
		catch (std::out_of_range const&)
		{
			return;
		}
	}

	void Animation::SetCurrentAnimationFrameData(unsigned currentFrameIndex, vec2 const& minUV, vec2 const& maxUV)
	{
		try
		{
			m_animationFrames.at(currentFrameIndex).m_minUV = minUV;
			m_animationFrames.at(currentFrameIndex).m_maxUV = maxUV;
		}
		catch (std::out_of_range const&)
		{
			return;
		}
	}

	void Animation::SetAnimationDuration(float animationDuration)
	{
		m_animationDuration = animationDuration;

		for (auto& frame : m_animationFrames)
		{
			frame.m_duration = m_animationDuration / static_cast<float>(m_totalSprites);
		}
	}

	void Animation::SetAnimationID(std::string animationID)
	{
		m_animationID = animationID;
	}

	// AnimationComponent
	void AnimationComponent::AddAnimationToComponent(std::string animationID)
	{
		m_animationsID.insert(animationID);
		//m_currentAnimationID = animationID;
	}

	void AnimationComponent::RemoveAnimationFromComponent(std::string animationID)
	{
		m_animationsID.erase(animationID);
	}

	void AnimationComponent::SetCurrentAnimationID(std::string animationIndex)
	{
		m_currentAnimationID = animationIndex;
	}

	nlohmann::json AnimationComponent::ToJson(size_t id) const
	{
		id;
		nlohmann::json j;
		j["CurrentAnimationID"] = m_currentAnimationID;
		return j;
	}

	AnimationComponent& AnimationComponent::Deserialize(const nlohmann::json& j)
	{
		m_currentAnimationID = j["CurrentAnimationID"].get<std::string>();
		return *this;
	}

	// Animation Frame
	nlohmann::json AnimationFrame::ToJson() const
	{
		nlohmann::json j;
		// j["textureKey"] = textureKey; @Brandon do I need this?
		j["minUV"] = { m_minUV.x, m_minUV.y };
		j["maxUV"] = { m_maxUV.x, m_maxUV.y };
		j["duration"] = m_duration;
		return j;
	}

	// Animiation Properties
	nlohmann::json Animation::ToJson() const
	{
		nlohmann::json j;
		j["animationID"] = m_animationID;
		j["spriteSheetKey"] = m_spriteSheetKey;
		j["totalSprites"] = m_totalSprites;
		j["animationDuration"] = m_animationDuration;

		// Serialize the animation frames
		for (const auto& frame : m_animationFrames)
		{
			j["animationFrames"].push_back(frame.ToJson());
		}
		// Serialize the empty frame @Brandon if need to, just uncomment it - Hans
		// j["emptyFrame"] = m_emptyFrame.ToJson();

		return j;
	}

	Animation& Animation::Deserialize(const nlohmann::json& j)
	{
		m_animationID = j["animationID"].get<std::string>();
		m_spriteSheetKey = j["spriteSheetKey"].get<std::string>();
		m_totalSprites = j["totalSprites"].get<unsigned>();
		m_animationDuration = j["animationDuration"].get<float>();

		for (const auto& frameJson : j["animationFrames"])
		{
			AnimationFrame frame;
			frame.m_minUV.x = frameJson["minUV"][0].get<float>(); // Assign x
			frame.m_minUV.y = frameJson["minUV"][1].get<float>(); // Assign y
			frame.m_maxUV.x = frameJson["maxUV"][0].get<float>(); // Assign x
			frame.m_maxUV.y = frameJson["maxUV"][1].get<float>(); // Assign y
			frame.m_duration = frameJson["duration"].get<float>();
			m_animationFrames.push_back(frame);
		}

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

		SetAnimationSpriteSheetKey(playerWalkAnimation, "../Assets/Textures/Animations/Individual Rows/Cat_Grey_Walk.png");
		SetAnimationSpriteSheetKey(playerAttackAnimation, "../Assets/Textures/Animations/Individual Rows/Cat_Grey_Attack.png");
		SetAnimationSpriteSheetKey(ratDeathAnimation, "../Assets/Textures/Animations/Individual Rows/Rat_Gutter_Attack.png");
		SetAnimationSpriteSheetKey(ratDeathAnimation, "../Assets/Textures/Animations/Individual Rows/Rat_Gutter_Death.png");

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
						EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetTextureKey(GetAnimationSpriteSheetKey(EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID()));
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
		ResourceManager::GetInstance().Animations[animationID]->SetAnimationID(animationID);
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
			ResourceManager::GetInstance().Animations[animationComponent.GetAnimationID()]->UpdateAnimationFrame(deltaTime, animationComponent.m_currentFrameTime, animationComponent.m_currentFrameIndex);
			return ResourceManager::GetInstance().Animations[animationComponent.GetAnimationID()]->GetCurrentAnimationFrame(animationComponent.m_currentFrameIndex);
		}
		return AnimationFrame{};
	}

	std::string AnimationManager::GetAnimationSpriteSheetKey(std::string animationID)
	{
		return ResourceManager::GetInstance().GetAnimation(animationID)->GetSpriteSheetKey();
	}

	void AnimationManager::SetAnimationSpriteSheetKey(std::string animationID, std::string spriteSheetKey)
	{
		ResourceManager::GetInstance().GetAnimation(animationID)->SetSpriteSheetKey(spriteSheetKey);
	}
}
