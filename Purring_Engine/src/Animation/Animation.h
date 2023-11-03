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
#include <glm/glm.hpp>
#include "System.h"
#include "Data/json.hpp"

namespace PE
{
	/*!***********************************************************************************
	 \brief A structure representing a single frame in an animation.
	*************************************************************************************/
	struct AnimationFrame
	{
		// un-comment if not using spritesheet
		//std::string textureKey;
		glm::vec2 m_minUV;
		glm::vec2 m_maxUV;
		float m_duration;
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
		//Animation();

		/*!***********************************************************************************
		 \brief Create an animation class with a spritesheet.
		 \param[in] spriteSheetKey Identifier for spritesheet to be used for this animation
		*************************************************************************************/
		Animation(std::string spriteSheetKey);

		/*!***********************************************************************************
		 \brief Add a new frame to the animation sequence.
		 \param[in] textureKey Identifier for the texture to display in this frame.
		 \param[in] duration Duration this frame will be displayed in seconds.
		*************************************************************************************/
		void AddFrame(glm::vec2 const& minUV, glm::vec2 const& maxUV,  float duration);

		/*!***********************************************************************************
		 \brief Update the animation based on elapsed time.
		 \param[in] deltaTime Time since last update.
		 \return Current texture key for the frame to display.
		*************************************************************************************/
		AnimationFrame const& UpdateAnimation(float deltaTime);

		void ResetAnimation();

		inline std::string GetSpriteSheetKey() { return m_textureKey; }

		// ----- Private Variables ----- //
	private:
		std::vector<AnimationFrame> m_animationFrames;
		std::string m_textureKey;
		unsigned m_currentFrameIndex;
		float m_elapsedTime;
	};

	class AnimationComponent
    {
        // ----- Public functions ----- //
    public:
        /*!***********************************************************************************
        \brief Get the RGBA color values of the object as a glm::vec4 object.

        \return glm::vec4 const& - RGBA values of the color of the object.
        *************************************************************************************/
        inline std::string GetAnimationID() const { return m_currentAnimationIndex; }
		void SetAnimationID(const std::string& str) { m_currentAnimationIndex = str; }

		inline bool HasAnimation() const { return !m_animationsID.empty(); }

		void AddAnimationToComponent(std::string animationID);

		void SetCurrentAnimationIndex(std::string animationIndex);

        ///*!***********************************************************************************
        // \brief Sets the RGBA color of the object. If the object has a texture on it,
        //        this tints the color of the texture.

        // \param[in] newColor RGBA color to set the object to (the values should be on
        //                        a range of [0, 1]).
        //*************************************************************************************/
        //void Renderer::SetColor(glm::vec4 const& newColor);

        ///*!***********************************************************************************
        // \brief Serializes the data attached to this renderer.
        //*************************************************************************************/
        nlohmann::json ToJson(size_t id) const;

        ///*!***********************************************************************************
        // \brief Deserializes data from a JSON file and loads it as values to set this
        //        component to.

        // \param[in] j JSON object containing the values to load into the renderer component.
        //*************************************************************************************/
        AnimationComponent& Deserialize(const nlohmann::json& j);

    private:

		std::vector<std::string> m_animationsID; // Stores all animations for the component // not in use now
		std::string m_currentAnimationIndex{}; // current playing animation
		std::string m_startingAnimationIndex{}; // starting playing animation
    };

	/*!***********************************************************************************
	 \brief Manages multiple animations.
	*************************************************************************************/
	class AnimationManager : public System
	{
		// ----- Public Methods ----- //
	public:
        /*!***********************************************************************************
         \brief Creates a VAO and VBO for a quad and stores it, and compiles and links a
                simple shader program to draw the triangle.
        *************************************************************************************/
        void InitializeSystem();

        /*!***********************************************************************************
         \brief Draws the editor windows, the gameobjects and the debug shapes.

         \param[in] deltaTime Timestep (in seconds). Not used by the renderer manager.
        *************************************************************************************/
        void UpdateSystem(float deltaTime);

        /*!***********************************************************************************
         \brief Delete the buffers, VAO and shader program allocated through OpenGL.
        *************************************************************************************/
        void DestroySystem();

        /*!***********************************************************************************
         \brief Returns the name of the Graphics system.
        *************************************************************************************/
        inline std::string GetName() { return m_systemName; }

		/*!***********************************************************************************
		 \brief Create a new Animation object and return its ID.
		 \return Identifier for the newly created animation.
		*************************************************************************************/
		static std::string CreateAnimation(std::string animationID, std::string textureKey);

		/*!***********************************************************************************
		 \brief Add a frame to the specified animation.
		 \param[in] animationID Identifier for the animation to modify.
		 \param[in] textureKey Identifier for the texture to display in this frame.
		 \param[in] duration Duration this frame will be displayed in seconds.
		*************************************************************************************/
		static void AddFrameToAnimation(std::string animationID, glm::vec2 const& minUV, glm::vec2 const& maxUV, float duration);

		/*!***********************************************************************************
		 \brief Update the specified animation based on elapsed time.
		 \param[in] animationID Identifier for the animation to update.
		 \param[in] deltaTime Time since last update.
		 \return Current texture key for the frame to display.
		*************************************************************************************/
		AnimationFrame UpdateAnimation(std::string animationID, float deltaTime);

		std::string GetAnimationSpriteSheetKey(std::string animationID);

		// ----- Private Variables ----- //
	private:
		//std::map<std::string, Animation> m_animations;
        std::string m_systemName{ "Animation" };
	};

}
