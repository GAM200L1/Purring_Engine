/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Animation.h
 \date     01-11-2023

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
	class AnimationComponent
	{
		// ----- Public functions ----- //
	public:
		/*!***********************************************************************************
		\brief Get the current playing animation index for this component.

		\return Current animation index for this animation component.
		*************************************************************************************/
		inline std::string GetAnimationID() const { return m_currentAnimationID; }

		/*!***********************************************************************************
		\brief Get the current frame time of the animation.

		\return Current frame time of the animation.
		*************************************************************************************/
		inline float GetCurrentFrameTime() const { return m_currentFrameTime; }

		/*!***********************************************************************************
		\brief Get the current frame index of the animation.

		\return Current frame index of the animation.
		**************************************************************************************/
		inline unsigned GetCurrentFrameIndex() const { return m_currentFrameIndex; }

		/*!***********************************************************************************
		\brief Get the list of animations for the component.

		\return List of animations for the component.
		*************************************************************************************/
		inline std::set<std::string> const& GetAnimationList() const { return m_animationsID; }
		
		/*!***********************************************************************************
		\brief Set current frame time of the animation.

		\param[in] time Time to set to.
		*************************************************************************************/
		void SetCurrentFrameTime(float time) { m_currentFrameTime = time; }

		/*!***********************************************************************************
		\brief Set current frame index of the animation.

		\param[in] index Index to set to.
		*************************************************************************************/
		void SetCurrentFrameIndex(unsigned index) { m_currentFrameIndex = index; }

		/*!***********************************************************************************
		\brief Check if animationsID container is empty.

		\return bool value if the animationID is empty.
		*************************************************************************************/
		inline bool HasAnimation() const { return !m_animationsID.empty(); }

		/*!***********************************************************************************
		\brief Add animation to a component.

		\param[in] animationID id to add to component.
		*************************************************************************************/
		void AddAnimationToComponent(std::string animationID);

		/*!***********************************************************************************
		\brief Remove animation from a component.

		\param[in] animationID id to remove from component.
		*************************************************************************************/
		void RemoveAnimationFromComponent(std::string animationID);

		/*!***********************************************************************************
		\brief Set current playing animation index of the component

		\param[in] animationIndex index to set.
		*************************************************************************************/
		void SetCurrentAnimationID(std::string animationID);

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

		std::set<std::string> m_animationsID; // Stores all animations for the component // not in use now
		std::string m_currentAnimationID{}; // current playing animation
		std::string m_startingAnimationID{}; // starting playing animation // not sure if needed
		float m_currentFrameTime{}; // current frame time of the animation
		unsigned m_currentFrameIndex{}; // current frame index of the animation
	};

	/*!***********************************************************************************
	 \brief A structure representing a single frame in an animation.
	*************************************************************************************/
	struct AnimationFrame
	{
		// SERIALIZE THIS
		// un-comment if not using spritesheet
		//std::string textureKey;
		vec2 m_minUV{ 0, 0 };
		vec2 m_maxUV{ 1, 1 };
		float m_duration{ 0.f };
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
		 \brief Create an animation class with a spritesheet.

		 \param[in] spriteSheetKey Identifier for spritesheet to be used for this animation
		*************************************************************************************/
		Animation(std::string spriteSheetKey);

		void SetSpriteSheetKey(std::string spriteSheetKey);

		/*!***********************************************************************************
		 \brief Add a new frame to the animation sequence.

		 \param[in] minUV minUV coords for the texture to display in this frame.
		 \param[in] maxUV maxUV coords for the texture to display in this frame.
		 \param[in] duration Duration this frame will be displayed in seconds.
		*************************************************************************************/
		void AddFrame(vec2 const& minUV, vec2 const& maxUV,  float duration);

		/*!***********************************************************************************
		 \brief Update the animation based on elapsed time.

		 \param[in] deltaTime Time since last update.
		*************************************************************************************/
		void UpdateAnimationFrame(float deltaTime, float& currentFrameTime, unsigned& currentFrameIndex);

		/*!***********************************************************************************
			\brief Get the current frame of the animation.
				 
			\return Current frame of the animation.
		*************************************************************************************/
		AnimationFrame const& GetCurrentAnimationFrame(unsigned currentFrameIndex);

		void CreateAnimationFrames(unsigned totalSprites, float animationDuration);

		void SetCurrentAnimationFrameData(unsigned currentFrameIndex, float duration);

		void SetCurrentAnimationFrameData(unsigned currentFrameIndex, vec2 const& minUV, vec2 const& maxUV);

		void SetAnimationDuration(float animationDuration);

		void SetAnimationID(std::string animationID);

		inline std::string GetAnimationID() const { return m_animationID; }

		inline std::string GetSpriteSheetKey() const { return m_spriteSheetKey; }

		inline unsigned GetFrameCount() { return static_cast<unsigned>(m_animationFrames.size()); }

		inline float GetAnimationDuration() { return m_animationDuration; }

		// ----- Private Variables ----- //
	private:

		// SERIALIZE THESE
		std::string m_animationID;
		std::vector<AnimationFrame> m_animationFrames;
		std::string m_spriteSheetKey; // remove this, should be using texxture key from renderer component
		unsigned m_totalSprites;
		float m_animationDuration;

		AnimationFrame m_emptyFrame{};
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
		static std::string CreateAnimation(std::string animationID);

		/*!***********************************************************************************
		 \brief Add a frame to the specified animation.

		 \param[in] animationID Identifier for the animation to modify.
		 \param[in] textureKey Identifier for the texture to display in this frame.
		 \param[in] duration Duration this frame will be displayed in seconds.
		*************************************************************************************/
		static void AddFrameToAnimation(std::string animationID, vec2 const& minUV, vec2 const& maxUV, float duration);

		/*!***********************************************************************************
		 \brief Update the specified animation based on elapsed time.

		 \param[in] animationID Identifier for the animation to update.
		 \param[in] deltaTime Time since last update.

		 \return Current texture key for the frame to display.
		*************************************************************************************/
		AnimationFrame UpdateAnimation(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Get the spritesheet key of the current playing animation

		 \param[in] animationID to get spritesheet key from

		 \return Spritesheet key of current playing animation
		*************************************************************************************/
		std::string GetAnimationSpriteSheetKey(std::string animationID);

		/*!***********************************************************************************
		\brief Set the spritesheet key of the current playing animation
				 
		\param[in] animationID to set spritesheet key to
		\param[in] spriteSheetKey to set to
		*************************************************************************************/
		void SetAnimationSpriteSheetKey(std::string animationID, std::string spriteSheetKey);

		// ----- Private Variables ----- //
	private:
		//std::map<std::string, Animation> m_animations;
        std::string m_systemName{ "Animation" };
	};

}
