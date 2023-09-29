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

namespace PE
{
	Animation::Animation() : m_currentFrameIndex{ 0 }, m_elapsedTime{ 0.0f }
	{
	
	}

	void Animation::AddFrame(std::string textureKey, float duration)
	{
		m_animationFrames.emplace_back(AnimationFrame{ textureKey, duration });
	}

	std::string Animation::UpdateAnimation(float deltaTime)
	{
		m_elapsedTime += deltaTime;
		if (m_elapsedTime >= m_animationFrames[m_currentFrameIndex].duration)
		{
			// move on the the next frame when current frame duration is reached
			m_currentFrameIndex = (m_currentFrameIndex + 1) % m_animationFrames.size();
			m_elapsedTime = 0.f;
		}
		
		return m_animationFrames[m_currentFrameIndex].textureKey;		
	}

	int AnimationManager::CreateAnimation()
	{
		m_animations.emplace_back(Animation());
		return static_cast<int>(m_animations.size() - 1);
	}

	void AnimationManager::AddFrameToAnimation(int animationID, std::string textureKey, float duration)
	{
		if (animationID >= 0 && animationID < m_animations.size())
		{
			m_animations[animationID].AddFrame(textureKey, duration);
		}
	}

	std::string AnimationManager::UpdateAnimation(int animationID, float deltaTime)
	{
		if (animationID >= 0 && animationID < m_animations.size()) {
			return m_animations[animationID].UpdateAnimation(deltaTime);
		}
		return std::string();
	}
}
