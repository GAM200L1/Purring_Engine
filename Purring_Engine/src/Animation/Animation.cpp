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
	/*!***********************************************************************************
	 \brief     Constructs an Animation object with initial default values.

	 \tparam T  This function does not use a template.
	 \return    The constructor initializes the object's member variables.
	*************************************************************************************/
	Animation::Animation() : m_currentFrameIndex{ 0 }, m_elapsedTime{ 0.0f }
	{
	
	}



	/*!***********************************************************************************
	 \brief     Adds a new animation frame to the list of animation frames.

	 \tparam T  This function does not use a template.
	 \param[in] textureKey The key used to identify the texture for this frame.
	 \param[in] duration   The duration for which this frame should be displayed.
	 \return    void       This function does not return a value but modifies the internal state of the Animation object.
	*************************************************************************************/
	void Animation::AddFrame(std::string textureKey, float duration)
	{
		m_animationFrames.emplace_back(AnimationFrame{ textureKey, duration });
	}



	/*!***********************************************************************************
	 \brief     Updates the current animation frame based on elapsed time.

	 \tparam T  This function does not use a template.
	 \param[in] deltaTime The time that has passed since the last frame update.
	 \return    std::string The texture key of the current animation frame.
	*************************************************************************************/
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



	/*!***********************************************************************************
	 \brief     Creates a new Animation object and adds it to the list of managed animations.

	 \tparam T  This function does not use a template.
	 \return    int The index at which the new Animation object is stored in the list.
	*************************************************************************************/
	int AnimationManager::CreateAnimation()
	{
		m_animations.emplace_back(Animation());
		return static_cast<int>(m_animations.size() - 1);
	}



	/*!***********************************************************************************
	 \brief     Adds a new frame to an existing animation specified by its animationID.

	 \tparam T  This function does not use a template.
	 \param[in] animationID The identifier of the animation to which the frame will be added.
	 \param[in] textureKey  The key used to identify the texture for this frame.
	 \param[in] duration    The duration for which this frame should be displayed.
	 \return    void        This function does not return a value but modifies the internal state of the Animation object.
	*************************************************************************************/
	void AnimationManager::AddFrameToAnimation(int animationID, std::string textureKey, float duration)
	{
		if (animationID >= 0 && animationID < m_animations.size())
		{
			m_animations[animationID].AddFrame(textureKey, duration);
		}
	}



	/*!***********************************************************************************
	 \brief     Updates the current animation frame for an existing animation specified
				by its animationID based on elapsed time.

	 \tparam T  This function does not use a template.
	 \param[in] animationID The identifier of the animation to update.
	 \param[in] deltaTime  The time that has passed since the last frame update.
	 \return    std::string The texture key of the current animation frame. Returns an empty string if the animationID is invalid.
	*************************************************************************************/
	std::string AnimationManager::UpdateAnimation(int animationID, float deltaTime)
	{
		if (animationID >= 0 && animationID < m_animations.size()) {
			return m_animations[animationID].UpdateAnimation(deltaTime);
		}
		return std::string();
	}

}
