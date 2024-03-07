/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Animation.h
 \date     27-11-2023

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
#include "Math/MathCustom.h"
#include "ECS/Entity.h"

namespace PE
{
	// forward declare animation class
	class Animation;

	class AnimationComponent
	{
		// ----- Public functions ----- //
	public:
		/*!***********************************************************************************
		\brief Get the current playing animation index for this component.

		\return Current animation ID for this animation component.
		*************************************************************************************/
		inline std::string GetAnimationID() const { return m_currentAnimationID; }

		/*!***********************************************************************************
		\brief Get the current playing animation for this component.

		\return Current animation for this animation component.
		*************************************************************************************/
		inline std::shared_ptr<Animation> GetAnimation() const { return m_currentAnimation; }

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

		/*!***********************************************************************************
		 \brief Get the Total Frames in the animation
		 
		 \return unsigned 
		*************************************************************************************/
		unsigned GetAnimationTotalFrames();

		/*!***********************************************************************************
		 \brief Get the maximum index for the animation

		 \return unsigned
		*************************************************************************************/
		unsigned GetAnimationMaxIndex();

		/*!***********************************************************************************
		 \brief Get the Animation Frame Rate
		 
		 \return unsigned 
		*************************************************************************************/
		unsigned GetAnimationFrameRate();

		/*!****v*******************************************************************************
		 \brief Get the Animation Frame Time
		 
		 \return double 
		*************************************************************************************/
		double GetAnimationFrameTime();

		/*!***********************************************************************************
		 \brief Set animation to play.
		*************************************************************************************/
		void PlayAnimation();

		/*!***********************************************************************************
		 \brief Set animation to pause.
		*************************************************************************************/
		void PauseAnimation();

		/*!***********************************************************************************
		 \brief Stop animation and reset to the start.
		*************************************************************************************/
		void StopAnimation();

		/*!***********************************************************************************
		 \brief Reset animation to the start.
		*************************************************************************************/
		void ResetAnimation();

		/*!***********************************************************************************
		 \brief Set animation has ended.

		 \param[in] ended bool value if the animation has ended.
		*************************************************************************************/
		void SetAnimationEnded(bool ended);

		/*!***********************************************************************************
		 \brief Check if animation has ended.

		 \return bool value if the animation has ended.
		*************************************************************************************/
		inline bool HasAnimationEnded() const { return m_animationEnded; }

		/*!***********************************************************************************
		 \brief Check if animation is playing.

		 \return bool value if the animation is playing.
		*************************************************************************************/
		inline bool IsPlaying() const { return m_isPlaying; }

		/*!***********************************************************************************
		 \brief Serializes the data attached to this component.

		 \param[in] id The id of the entity that owns this component.
		 \return A JSON object with the data of this component.
		*************************************************************************************/
		nlohmann::json ToJson(size_t id) const;

		/*!***********************************************************************************
		 \brief Deserializes data from a JSON file and loads it as values to set this
		        component to.

		 \param[in] j JSON object containing the values to load into the renderer component.
		 \return The animation component with the loaded values;
		*************************************************************************************/
		AnimationComponent& Deserialize(const nlohmann::json& r_j);

		// need to serialize
		std::set<std::string> m_animationsID; // Stores all animations for the component // not in use now
		std::string m_currentAnimationID{}; // current playing animation
		std::string m_startingAnimationID{}; // starting playing animation // may not need this as there are starting states

		// new stuff to serialize


		// do not need to serialize
		float m_currentFrameTime{}; // current frame time of the animation
		unsigned m_currentFrameIndex{}; // current frame index of the animation
		std::shared_ptr<Animation> m_currentAnimation; // current playing animation
		bool m_isPlaying{ false }; // is the animation playing, false by default
		bool m_animationEnded{ false }; // has the animation ended, false by default

		// add later
		//std::map<std::string, std::shared_ptr<Animation>> m_animationsMap; // Stores all animations for the component - need this for states? not sure if just key is enough
		// std::map<std::string, std::shared_ptr<Animation>> m_animationStates; // not sure if using shared_ptr or key for the animation
		// EntryState m_entryState; // entry state of the animation
		
	};

	/*!***********************************************************************************
	 \brief A structure representing a single frame in an animation.
	*************************************************************************************/
	struct AnimationFrame
	{
		vec2 m_minUV{ 0, 0 };
		vec2 m_maxUV{ 1, 1 };
		float m_duration{ 0.f };

		// un-comment if not using spritesheet
		std::string m_textureKey;

		/*!***********************************************************************************
		 \brief Serializes the data attached to this frame.

		 \return A JSON object with the data of this component.
		*************************************************************************************/
		nlohmann::json AnimationFrame::ToJson() const;
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

		/*!***********************************************************************************
		 \brief Create an animation class with a spritesheet and an animation ID.

		 \param[in] spriteSheetKey ID for spritesheet to be used for this animation
		*************************************************************************************/
		void SetSpriteSheetKey(std::string spriteSheetKey);

		/*!***********************************************************************************
		 \brief Add a new frame to the animation sequence.

		 \param[in] minUV minUV coords for the texture to display in this frame.
		 \param[in] maxUV maxUV coords for the texture to display in this frame.
		 \param[in] duration Duration this frame will be displayed in seconds.
		*************************************************************************************/
		void AddFrame(vec2 const& r_minUV, vec2 const& r_maxUV,  float duration);

		/*!***********************************************************************************
		 \brief Add a new frame to the animation sequence.

		 \param[in] textureKey Identifier for the texture to display in this frame.
		 \param[in] duration Duration this frame will be displayed in seconds.
		*************************************************************************************/
		void AddFrame(std::string textureKey, float duration);

		/*!***********************************************************************************
		 \brief Update the animation based on elapsed time.

		 \param[in] deltaTime Time since last update.
		 \param[in] r_currentFrameTime Current frame time of the animation.
		 \param[in] r_currentFrameIndex Current frame index of the animation.
		*************************************************************************************/
		void UpdateAnimationFrame(float deltaTime, float& r_currentFrameTime, unsigned& r_currentFrameIndex);

		/*!***********************************************************************************
		 \brief Update the animation based on elapsed time.

		 \param[in] deltaTime Time since last update.
		 \param[in] r_animationComponent
		*************************************************************************************/
		void UpdateAnimationFrame(float deltaTime, AnimationComponent& r_animationComponent);

		/*!***********************************************************************************
			\brief Get the current frame of the animation.
				 
			\param[in] currentFrameIndex Index of the current frame.
			\return Current frame of the animation.
		*************************************************************************************/
		AnimationFrame const& GetCurrentAnimationFrame(unsigned currentFrameIndex);

		/*!***********************************************************************************
		 \brief Create the animation frames based on the total number of sprites.

		 \param[in] deltaTime Time since last update.
		*************************************************************************************/
		void CreateAnimationFrames(unsigned totalSprites);

		/*!***********************************************************************************
		 \brief Set the frame duration of the current frame.

		 \param[in] currentFrameIndex Index of the current frame.
		 \param[in] duration Duration this frame will be displayed in seconds.
		*************************************************************************************/
		void SetCurrentAnimationFrameData(unsigned currentFrameIndex, float duration);

		/*!***********************************************************************************
		 \brief Set the frames to hold of the current frame.

		 \param[in] currentFrameIndex Index of the current frame.
		 \param[in] framesToHold Number of frames to hold.
		*************************************************************************************/
		void SetCurrentAnimationFrameData(unsigned currentFrameIndex, unsigned framesToHold);

		/*!***********************************************************************************
		 \brief Set the UV coords of the spritesheet for the current frame.

		 \param[in] currentFrameIndex Index of the current frame.
		 \param[in] r_minUV minUV coords for the texture to display in this frame.
		 \param[in] r_maxUV maxUV coords for the texture to display in this frame.
		*************************************************************************************/
		void SetCurrentAnimationFrameData(unsigned currentFrameIndex, vec2 const& r_minUV, vec2 const& r_maxUV);

		/*!***********************************************************************************
		 \brief Set the texture key of the animation for the current frame.

		 \param[in] currentFrameIndex Index of the current frame.
		 \param[in] textureKey Identifier for the texture to display in this frame.
		*************************************************************************************/
		void SetCurrentAnimationFrameData(unsigned currentFrameIndex, std::string textureKey);

		/*!***********************************************************************************
		 \brief Set the frame rate of the animation

		 \param[in] frameRate Frame rate of the animation
		*************************************************************************************/
		void SetCurrentAnimationFrameRate(unsigned frameRate);

		/*!***********************************************************************************
		 \brief Set the animation ID of the animation

		 \param[in] animationID ID of the animation
		*************************************************************************************/
		void SetAnimationID(std::string animationID);

		/*!***********************************************************************************
		 \brief Get the animation ID of the animation.

		 \return Animation ID of the animation.
		*************************************************************************************/
		inline std::string GetAnimationID() const { return m_animationID; }

		/*!***********************************************************************************
		 \brief Get the spritesheet key of the animation.

		 \return Spritesheet key of the animation.
		*************************************************************************************/
		inline std::string GetSpriteSheetKey() const { return m_spriteSheetKey; }

		/*!***********************************************************************************
		 \brief Get the total number of frames in the animation.

		 \return Total number of frames in the animation.
		*************************************************************************************/
		inline unsigned GetFrameCount() { return static_cast<unsigned>(m_animationFrames.size()); }
		
		/*!***********************************************************************************
		 \brief Get the frame rate of the animation.

		 \return Frame rate of the animation.
		*************************************************************************************/
		inline unsigned GetFrameRate() { return m_frameRate; }

		/*!***********************************************************************************
		 \brief Get the duration of the animation.

		 \return Duration of the animation.
		*************************************************************************************/
		float GetAnimationDuration();

		/*!***********************************************************************************
		 \brief Get the Animations' duration per frame
		 
		 \return float 
		*************************************************************************************/
		float GetAnimationFrameTime();

		/*!***********************************************************************************
		 \brief Check if animation is using spritesheet.

		 \return float
		*************************************************************************************/
		inline bool IsSpriteSheet() const { return m_isSpriteSheet; }

		/*!***********************************************************************************
		 \brief Load animation from file.

		 \param[in] r_filePath File path of animation.
		 \return True if animation is loaded successfully, false otherwise.
		*************************************************************************************/
		bool LoadAnimation(std::string const& r_filePath);

		/*!***********************************************************************************
		 \brief Set the looping of the animation.

		 \param[in] looping True if animation is to loop, false otherwise.
		*************************************************************************************/
		void SetLooping(bool looping);

		/*!***********************************************************************************
		 \brief Check if animation is looping.

		 \return True if animation is looping, false otherwise.
		*************************************************************************************/
		inline bool IsLooping() const { return m_isLooping; }

		/*!***********************************************************************************
		 \brief Serializes the data attached to this animation.

		 \return A JSON object with the data of this animation.
		*************************************************************************************/
		nlohmann::json Animation::ToJson() const;

		/*!***********************************************************************************
		 \brief Deserializes data from a JSON file and loads it as values to set this
				animation to.

		 \param[in] r_j JSON object containing the values to load into the animation.
		 \return The animation with the loaded values;
		*************************************************************************************/
		Animation& Deserialize(const nlohmann::json& r_j);


		// ----- Private Variables ----- //
	private:
		std::string m_animationID;
		std::vector<AnimationFrame> m_animationFrames;
		std::string m_spriteSheetKey; // if using spritesheet
		unsigned m_totalSprites;
		unsigned m_frameRate;
		bool m_isSpriteSheet{ true };

		// new to serialize
		bool m_isLooping{ true };

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
		static void AddFrameToAnimation(std::string animationID, vec2 const& r_minUV, vec2 const& r_maxUV, float duration);

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

		/*!***********************************************************************************
		 \brief Set all animations to play.
		*************************************************************************************/
		void PlayAllAnimations() const;

		/*!***********************************************************************************
		 \brief Set all animations to pause.
		*************************************************************************************/
		void PauseAllAnimations() const;

		/*!***********************************************************************************
		 \brief Stop and reset all animation.
		*************************************************************************************/
		void StopAllAnimations() const;

		// ----- Private Variables ----- //
	private:
		//std::map<std::string, Animation> m_animations;
        std::string m_systemName{ "Animation" };
	};

}
