/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Animation.h
 \date     25-09-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief	   This file contains the implementation of the Animation and AnimationManager
		   classes. Animation handles the logic for updating and maintaining individual
		   animation frames, while AnimationManager manages multiple Animation objects,
		   offering an interface for creating, updating, and adding frames to animations.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include <vector>
#include <string>


namespace PE
{
	/*!***********************************************************************************
	 \brief A structure representing a single frame in an animation.
	*************************************************************************************/
	struct AnimationFrame
	{
		std::string textureKey;
		float duration;
	};

	/*!***********************************************************************************
	 \brief A class handling individual animations, made up of multiple frames.
	*************************************************************************************/
	class Animation
	{
		// ----- Public Methods ----- //
	public:
		/*!***********************************************************************************
		 \brief Default constructor for Animation class.
		*************************************************************************************/
		Animation();

		/*!***********************************************************************************
		 \brief Add a new frame to the animation sequence.
		 \param[in] textureKey Identifier for the texture to display in this frame.
		 \param[in] duration Duration this frame will be displayed in seconds.
		*************************************************************************************/
		void AddFrame(std::string textureKey, float duration);

		/*!***********************************************************************************
		 \brief Update the animation based on elapsed time.
		 \param[in] deltaTime Time since last update.
		 \return Current texture key for the frame to display.
		*************************************************************************************/
		std::string UpdateAnimation(float deltaTime);

		// ----- Private Variables ----- //
	private:
		std::vector<AnimationFrame> m_animationFrames;
		unsigned m_currentFrameIndex;
		float m_elapsedTime;
	};


	/*!***********************************************************************************
	 \brief Manages multiple animations.
	*************************************************************************************/
	class AnimationManager
	{
		// ----- Public Methods ----- //
	public:
		/*!***********************************************************************************
		 \brief Create a new Animation object and return its ID.
		 \return Identifier for the newly created animation.
		*************************************************************************************/
		int CreateAnimation();

		/*!***********************************************************************************
		 \brief Add a frame to the specified animation.
		 \param[in] animationID Identifier for the animation to modify.
		 \param[in] textureKey Identifier for the texture to display in this frame.
		 \param[in] duration Duration this frame will be displayed in seconds.
		*************************************************************************************/
		void AddFrameToAnimation(int animationID, std::string textureKey, float duration);

		/*!***********************************************************************************
		 \brief Update the specified animation based on elapsed time.
		 \param[in] animationID Identifier for the animation to update.
		 \param[in] deltaTime Time since last update.
		 \return Current texture key for the frame to display.
		*************************************************************************************/
		std::string UpdateAnimation(int animationID, float deltaTime);

		// ----- Private Variables ----- //
	private:
		std::vector<Animation> m_animations;
	};

}
