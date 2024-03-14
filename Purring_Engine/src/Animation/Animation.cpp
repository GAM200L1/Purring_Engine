/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Animation.cpp
 \date     27-11-2023

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
#include "Logging/Logger.h"
#include "ECS/SceneView.h"
#include "Graphics/Renderer.h"
#include "ResourceManager/ResourceManager.h"
#include "Animation.h"
#include "GameStateManager.h"
#include "Layers/LayerManager.h"
#include "Logic/LogicSystem.h"

#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE

extern Logger engine_logger;

namespace PE
{
	Animation::Animation() : m_totalSprites{ 6 }, m_frameRate{ 30 }
	{

	}

	Animation::Animation(std::string spriteSheetKey) : m_spriteSheetKey{ spriteSheetKey }, m_totalSprites{ 6 }, m_frameRate{ 30 }
	{
	
	}

	void Animation::SetSpriteSheetKey(std::string spriteSheetKey)
	{
		m_spriteSheetKey = spriteSheetKey;
	}

	void Animation::AddFrame(vec2 const& r_minUV, vec2 const& r_maxUV, float duration)
	{
		m_animationFrames.emplace_back(AnimationFrame{ r_minUV, r_maxUV, duration });
	}

	void Animation::AddFrame(std::string textureKey, float duration)
	{
		m_animationFrames.emplace_back(AnimationFrame{ vec2{0.f, 0.f}, vec2{1.f, 1.f}, duration, textureKey });
	}

	void Animation::UpdateAnimationFrame(float deltaTime, float& r_currentFrameTime, unsigned& r_currentFrameIndex)
	{
		r_currentFrameTime += deltaTime;

		try
		{
			if (r_currentFrameTime >= m_animationFrames.at(r_currentFrameIndex).m_duration)
			{
				if (m_isLooping)
				{
					// move on the the next frame when current frame duration is reached
					r_currentFrameIndex = (r_currentFrameIndex + 1) % m_animationFrames.size();
				}
				else
				{
					// if not last frame, move on to the next frame
					if (r_currentFrameIndex < m_animationFrames.size() - 1)
					{
						++r_currentFrameIndex;
					}
					else // if last frame
					{

					}
				}

				r_currentFrameTime = 0.f;
			}
		}
		catch (const std::out_of_range&)
		{
			return;
		}
	}

	void Animation::UpdateAnimationFrame(float deltaTime, EntityID id)
	{
		AnimationComponent& animationComponent = EntityManager::GetInstance().Get<AnimationComponent>(id);
		animationComponent.m_currentFrameTime += deltaTime;

		try
		{
			if (animationComponent.m_currentFrameTime >= m_animationFrames.at(animationComponent.m_currentFrameIndex).m_duration)
			{
				// reset current frame time
				animationComponent.m_currentFrameTime = 0.f;

				if (m_isLooping)
				{
					// if not last frame, set animation ended to true
					if (animationComponent.m_currentFrameIndex < m_animationFrames.size() - 1)
					{
						animationComponent.SetAnimationEnded(false);
					}
					else
					{
						animationComponent.SetAnimationEnded(true);
					}

					// move on the the next frame when current frame duration is reached
					animationComponent.m_currentFrameIndex = (animationComponent.m_currentFrameIndex + 1) % m_animationFrames.size();
					animationComponent.m_frameChanged = true;
				}
				else
				{
					// if not last frame, move on to the next frame
					if (animationComponent.m_currentFrameIndex < m_animationFrames.size() - 1)
					{
						++animationComponent.m_currentFrameIndex;
						animationComponent.m_frameChanged = true;
					}
					else // if last frame, pause animation
					{
						animationComponent.PauseAnimation();
						animationComponent.SetAnimationEnded(true);

						return;
					}
				}
			}

			// if frame changed just changed
			if (animationComponent.m_frameChanged)
			{
				// set on frame enter to true and frame changed to false
				animationComponent.m_onFrameEnter = true;
				animationComponent.m_frameChanged = false;
			}
			else
			{
				animationComponent.m_onFrameEnter = false;
			}

			// if animation entered a new frame
			if (animationComponent.OnFrameEnter())
			{
				// trigger all actions in the current frame
				for (AnimationAction const& action : m_animationFrames[animationComponent.m_currentFrameIndex].actions)
				{
					// check if entity has the script
					if (EntityManager::GetInstance().Has<ScriptComponent>(id))
					{
						ScriptComponent& scriptComponent = EntityManager::GetInstance().Get<ScriptComponent>(id);
						if (scriptComponent.m_scriptKeys.find(action.scriptName) != scriptComponent.m_scriptKeys.end())
						{
							LogicSystem::m_scriptContainer[action.scriptName]->animationFunctions[action.scriptFunction](id);
						}
					}
				}
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

	void Animation::CreateAnimationFrames(unsigned totalSprites)
	{
		m_animationFrames.clear();

		if (!totalSprites)
		{
			return;
		}

		m_totalSprites = totalSprites;
		float duration = 1.f / static_cast<float>(totalSprites);

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

	void Animation::SetCurrentAnimationFrameData(unsigned currentFrameIndex, unsigned framesToHold)
	{
		try
		{
			m_animationFrames.at(currentFrameIndex).m_duration = static_cast<float>(framesToHold) / static_cast<float>(m_frameRate);
		}
		catch (std::out_of_range const&)
		{
			return;
		}
	}

	void Animation::SetCurrentAnimationFrameData(unsigned currentFrameIndex, vec2 const& r_minUV, vec2 const& r_maxUV)
	{
		try
		{
			m_animationFrames.at(currentFrameIndex).m_minUV = r_minUV;
			m_animationFrames.at(currentFrameIndex).m_maxUV = r_maxUV;
		}
		catch (std::out_of_range const&)
		{
			return;
		}
	}

	void Animation::SetCurrentAnimationFrameData(unsigned currentFrameIndex, std::string textureKey)
	{
		try
		{
			m_animationFrames.at(currentFrameIndex).m_textureKey = textureKey;
		}
		catch (std::out_of_range const&)
		{
			return;
		}
	}

	void Animation::AddCurrentAnimationFrameAction(unsigned currentFrameIndex, AnimationAction action)
	{
		try
		{
			if(std::find(m_animationFrames.at(currentFrameIndex).actions.begin(), m_animationFrames.at(currentFrameIndex).actions.end(), action) == m_animationFrames.at(currentFrameIndex).actions.end())
			m_animationFrames.at(currentFrameIndex).actions.emplace_back(action);
		}
		catch (std::out_of_range const&)
		{
			return;
		}
	}

	void Animation::RemoveCurrentAnimationFrameAction(unsigned currentFrameIndex, AnimationAction action)
	{
		try
		{
			auto itr = std::remove(m_animationFrames.at(currentFrameIndex).actions.begin(), m_animationFrames.at(currentFrameIndex).actions.end(), action);
			m_animationFrames.at(currentFrameIndex).actions.erase(itr, m_animationFrames.at(currentFrameIndex).actions.end());
		}
		catch (std::out_of_range const&)
		{
			return;
		}
	}

	void Animation::SetCurrentAnimationFrameRate(unsigned frameRate)
	{
		m_frameRate = frameRate;
	}

	void Animation::SetAnimationID(std::string animationID)
	{
		m_animationID = animationID;
	}

	float Animation::GetAnimationDuration()
	{
		float duration{ 0.f };
		for (auto const& frame : m_animationFrames)
		{
			duration += frame.m_duration;
		}

		return duration;
	}

	float Animation::GetAnimationFrameTime()
	{
		return (m_animationFrames.empty())? 0.f : m_animationFrames.front().m_duration;
	}

	// AnimationComponent
	std::shared_ptr<Animation> AnimationComponent::GetAnimation() const
	{
		return ResourceManager::GetInstance().GetAnimation(m_currentAnimationID);
	}

	void AnimationComponent::AddAnimationToComponent(std::string animationID)
	{
		m_animationsID.insert(animationID);
	}

	void AnimationComponent::RemoveAnimationFromComponent(std::string animationID)
	{
		m_animationsID.erase(animationID);
	}

	void AnimationComponent::SetCurrentAnimationID(std::string animationIndex)
	{
		ResetAnimation();

		m_currentAnimationID = animationIndex;
	}

	unsigned AnimationComponent::GetAnimationTotalFrames()
	{
		return ResourceManager::GetInstance().GetAnimation(m_currentAnimationID)->GetFrameCount();
	}

	unsigned AnimationComponent::GetAnimationMaxIndex()
	{
		return GetAnimationTotalFrames() - 1;
	}

	unsigned AnimationComponent::GetAnimationFrameRate()
	{
		return ResourceManager::GetInstance().GetAnimation(m_currentAnimationID)->GetFrameRate();
	}

	double AnimationComponent::GetAnimationFrameTime()
	{
		return ResourceManager::GetInstance().GetAnimation(m_currentAnimationID)->GetAnimationFrameTime();
	}

	void AnimationComponent::PlayAnimation()
	{
		m_isPlaying = true;
	}

	void AnimationComponent::PauseAnimation()
	{
		m_isPlaying = false;
	}

	void AnimationComponent::StopAnimation()
	{
		// Pause the animation and reset the frame index and frame time
		PauseAnimation();
		ResetAnimation();
	}

	void AnimationComponent::ResetAnimation()
	{
		// Reset the frame index and frame time
		m_currentFrameIndex = 0;
		m_currentFrameTime = 0.f;
		m_animationEnded = false;
	}

	void AnimationComponent::SetAnimationEnded(bool ended)
	{
		m_animationEnded = ended;
	}

	nlohmann::json AnimationComponent::ToJson(size_t id) const
	{
		id;
		nlohmann::json j;
		j["CurrentAnimationID"] = m_currentAnimationID;
		j["AnimationIDs"] = m_animationsID;
		return j;
	}

	AnimationComponent& AnimationComponent::Deserialize(const nlohmann::json& r_j)
	{
		if (r_j.contains("CurrentAnimationID"))
			m_currentAnimationID = r_j["CurrentAnimationID"].get<std::string>();
		if (r_j.contains("AnimationIDs"))
		{
			m_animationsID = r_j["AnimationIDs"].get<std::set<std::string>>();
		}
		return *this;
	}

	// Animation Frame
	nlohmann::json AnimationFrame::ToJson() const
	{
		nlohmann::json j;
		j["minUV"] = { m_minUV.x, m_minUV.y };
		j["maxUV"] = { m_maxUV.x, m_maxUV.y };
		j["duration"] = m_duration;
		j["textureKey"] = m_textureKey;
		
		for (auto const& action : actions)
		{
			j["actions"].push_back(action.ToJson());
		}
		return j;
	}

	bool Animation::LoadAnimation(std::string const& r_filePath)
	{
		std::filesystem::path filePath{ r_filePath.c_str() };

		SerializationManager serializationManager;  // Create an instance
		nlohmann::json loadedAnimationData = serializationManager.LoadAnimationFromFile(filePath);

		// if able to load animation
		if (!loadedAnimationData.is_null())
		{
			Deserialize(loadedAnimationData);

			return true;
		}

		return false;
	}

	void Animation::SetLooping(bool isLooping)
	{
		m_isLooping = isLooping;
	}

	// Animation Properties
	nlohmann::json Animation::ToJson() const
	{
		nlohmann::json j;
		j["animationID"] = m_animationID;
		j["spriteSheetKey"] = m_spriteSheetKey;
		j["totalSprites"] = m_totalSprites;
		j["frameRate"] = m_frameRate;
		j["isSpriteSheet"] = m_isSpriteSheet;
		j["isLooping"] = m_isLooping;

		// Serialize the animation frames
		for (const auto& frame : m_animationFrames)
		{
			j["animationFrames"].push_back(frame.ToJson());
		}

		return j;
	}

	Animation& Animation::Deserialize(const nlohmann::json& r_j)
	{
		m_animationID = r_j["animationID"].get<std::string>();
		m_spriteSheetKey = r_j["spriteSheetKey"].get<std::string>();
		m_totalSprites = r_j["totalSprites"].get<unsigned>();
		m_frameRate = r_j["frameRate"].get<unsigned>();

		if(r_j.contains("isSpriteSheet"))
			m_isSpriteSheet = r_j["isSpriteSheet"].get<bool>();

		if (r_j.contains("isLooping"))
			m_isLooping = r_j["isLooping"].get<bool>();

		if(r_j.contains("animationFrames"))
		for (const auto& frameJson : r_j["animationFrames"])
		{
			AnimationFrame frame;
			frame.m_minUV.x = frameJson["minUV"][0].get<float>(); // Assign x
			frame.m_minUV.y = frameJson["minUV"][1].get<float>(); // Assign y
			frame.m_maxUV.x = frameJson["maxUV"][0].get<float>(); // Assign x
			frame.m_maxUV.y = frameJson["maxUV"][1].get<float>(); // Assign y
			frame.m_duration = frameJson["duration"].get<float>();

			if (frameJson.contains("textureKey"))
			{
				frame.m_textureKey = frameJson["textureKey"].get<std::string>();
				ResourceManager::GetInstance().AddTextureKeyToLoad(frame.m_textureKey);
			}

			if(frameJson.contains("actions"))
			for (const auto& actionJson : frameJson["actions"])
			{
				AnimationAction action;
				action.scriptName = actionJson["scriptName"].get<std::string>();
				action.scriptFunction = actionJson["scriptFunction"].get<std::string>();

				frame.actions.push_back(action);
			}

			m_animationFrames.push_back(frame);
		}

		// load sprite sheet for animation
		if(m_spriteSheetKey != "")
		ResourceManager::GetInstance().AddTextureKeyToLoad(m_spriteSheetKey);

		return *this;
	}

	nlohmann::json AnimationAction::ToJson() const
	{
		nlohmann::json j;
		j["scriptName"] = scriptName;
		j["scriptFunction"] = scriptFunction;
		return j;
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

		for (const auto& layer : LayerView<AnimationComponent>())
		{
			// updates only active entities animation
			for (EntityID const& id : InternalView(layer))
			{
				std::shared_ptr<Animation> p_animation{ ResourceManager::GetInstance().GetAnimation(EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID()) };

				// update animation and get current frame
				p_currentFrame = UpdateAnimation(id, deltaTime);

				// update entity based on frame data
				// in the future probably check for bools in animation component, then update data accordingly
				// check if theres animation
				if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID() != "")
				{
					if (EntityManager::GetInstance().Has<Graphics::Renderer>(id))
					{
						// spritesheet animation
						if (p_animation->IsSpriteSheet())
						{
							EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetTextureKey(p_animation->GetSpriteSheetKey());
							EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetUVCoordinatesMin(p_currentFrame.m_minUV);
							EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetUVCoordinatesMax(p_currentFrame.m_maxUV);
						}
						else // texture key animation
						{
							EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetTextureKey(p_currentFrame.m_textureKey);
						}
					}
				}
			}
		}
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

	void AnimationManager::AddFrameToAnimation(std::string animationID, vec2 const& r_minUV, vec2 const& r_maxUV, float duration)
	{
		if (ResourceManager::GetInstance().Animations.find(animationID) != ResourceManager::GetInstance().Animations.end())
		{
			ResourceManager::GetInstance().Animations[animationID]->AddFrame(r_minUV, r_maxUV, duration);
		}
	}

	AnimationFrame AnimationManager::UpdateAnimation(EntityID id, float deltaTime)
	{

		AnimationComponent& animationComponent = EntityManager::GetInstance().Get<AnimationComponent>(id);

		// store animations in resource manager instead
		if (ResourceManager::GetInstance().Animations.find(animationComponent.GetAnimationID()) != ResourceManager::GetInstance().Animations.end())
		{
#ifndef GAMERELEASE
			// if run time use IsPlaying variable
			if (Editor::GetInstance().IsRunTime())
			{
				if (animationComponent.IsPlaying())
					ResourceManager::GetInstance().Animations[animationComponent.GetAnimationID()]->UpdateAnimationFrame(deltaTime, id);
			}
			else // else, in editor use editor play variable
			{
				if (animationComponent.isPlayingEditor)
					ResourceManager::GetInstance().Animations[animationComponent.GetAnimationID()]->UpdateAnimationFrame(deltaTime, id);
			}		
#else
			if (animationComponent.IsPlaying())
				ResourceManager::GetInstance().Animations[animationComponent.GetAnimationID()]->UpdateAnimationFrame(deltaTime, id);
#endif // !GAMERELEASE			
			

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

	void AnimationManager::PlayAllAnimations() const
	{
		for (const auto& layer : LayerView<AnimationComponent>())
		{
			// sets all active and inactive entities to play animation
			for (EntityID const& id : InternalView(layer, true))
			{
				EntityManager::GetInstance().Get<AnimationComponent>(id).PlayAnimation();
			}
		}
	}

	void AnimationManager::PauseAllAnimations() const
	{
		for (const auto& layer : LayerView<AnimationComponent>())
		{
			// sets all active and inactive entities to pause animation
			for (EntityID const& id : InternalView(layer, true))
			{
				EntityManager::GetInstance().Get<AnimationComponent>(id).PauseAnimation();
			}
		}
	}

	void AnimationManager::StopAllAnimations() const
	{
		for (const auto& layer : LayerView<AnimationComponent>())
		{
			// sets all active and inactive entities to stop animation
			for (EntityID const& id : InternalView(layer, true))
			{
				EntityManager::GetInstance().Get<AnimationComponent>(id).StopAnimation();
			}
		}
	}

	void AnimationManager::SetEntityFirstFrame(EntityID id) const
	{
		// update animation and get current frame

		// update entity based on frame data
		// in the future probably check for bools in animation component, then update data accordingly
		// check if theres animation
		if(EntityManager::GetInstance().Has<AnimationComponent>(id))
		if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID() != "")
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(id))
			{
				std::shared_ptr<Animation> p_animation{ ResourceManager::GetInstance().GetAnimation(EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID()) };

				AnimationFrame p_firstFrame = p_animation->GetCurrentAnimationFrame(0);
				// spritesheet animation
				if (p_animation->IsSpriteSheet())
				{
					EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetTextureKey(p_animation->GetSpriteSheetKey());
					EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetUVCoordinatesMin(p_firstFrame.m_minUV);
					EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetUVCoordinatesMax(p_firstFrame.m_maxUV);
				}
				else // texture key animation
				{
					EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetTextureKey(p_firstFrame.m_textureKey);
				}
			}
		}
	}
}
