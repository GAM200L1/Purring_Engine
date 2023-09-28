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

	// Animation Manager
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