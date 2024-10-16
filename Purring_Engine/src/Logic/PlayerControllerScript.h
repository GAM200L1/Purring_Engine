/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     PlayerControllerScript.h 
 \date     03-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan\@digipen.edu
 \par      code %:     95% 
 \par      changes:    Majority of the code 

 \co-author            FOONG Jun Wei 
 \par      email:      f.junwei\@digipen.edu 
 \par      code %:     5% 
 \par      changes:    rttr::instance GetScriptData()


 \brief  This file contains the declaration of the player controller

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/ 

#pragma once
#include "Script.h"
#include "Events/EventHandler.h"
#include "Math/MathCustom.h"
#include "Data/SerializationManager.h"

namespace PE
{
	/*!***********************************************************************************
	 \brief Enum to handle the player state

	*************************************************************************************/
	enum class  PlayerState{ IDLE = 0, MOVING, DEAD };

	/*!***********************************************************************************
	 \brief Data used by the player controller script
	 
	*************************************************************************************/
	struct PlayerControllerScriptData
	{
		PlayerState currentPlayerState{ PlayerState::IDLE };
		int HP{ 100 };
		float speed{ 500 };
	};

	/*!***********************************************************************************
	 \brief Player controller script
	 
	*************************************************************************************/
	class PlayerControllerScript : public Script
	{
	public:
		/*!***********************************************************************************
		 \brief Initializes the script to input param entity id
		 
		 \param[in] id Entity that owns an instance of this script
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief Updates the script
		 
		 \param[in] id 			The entity that owns the instance of the script
		 \param[in] deltaTime 	The delta time to update according to
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Destroys the instance of the script held on to by input param id
		 
		 \param[in] id 
		*************************************************************************************/
		virtual void Destroy(EntityID id);

		/*!***********************************************************************************
		 \brief On attach behaviour
		 
		 \param[in] id 
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		 \brief On detatch behaviour
		 
		 \param[in] id 
		*************************************************************************************/
		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Moves the player specified by id
		 
		 \param[in] id 			ID to treat as the player
		 \param[in] deltaTime 	delta time to update according to
		*************************************************************************************/
		void MovePlayer(EntityID id,float deltaTime);

		/*!***********************************************************************************
		 \brief Checks the state of the input param id
		 
		 \param[in] id 
		*************************************************************************************/
		void CheckState(EntityID id);

		/*!***********************************************************************************
		 \brief Get the Script Data
		 
		 \return std::map<EntityID, PlayerControllerScriptData>& 
		*************************************************************************************/
		std::map<EntityID, PlayerControllerScriptData>& GetScriptData();

		/*!***********************************************************************************
		 \brief Get the Script Data instance (RTTR)
		 
		 \param[in] id 
		 \return rttr::instance 
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);

		/*!***********************************************************************************
		 \brief Destroy the Player Controller Script object
		 
		*************************************************************************************/
		~PlayerControllerScript();
	private:
		/*!***********************************************************************************
		 \brief Callback function to subscribe to mouse events
		 
		 \param[in] r_ME const reference to the mous eevent receoived
		*************************************************************************************/
		void OnMouseClick(const Event<MouseEvents>& r_ME);

		/*!***********************************************************************************
		 \brief Helper function to move specified Entity of EntityID 'id' to click point

		 \param[in] id the EntityID of the object to move
		 \param[in] deltaTime the difference between the current and last frame
		*************************************************************************************/
		void MoveTowardsClicked(EntityID id, float deltaTime);

		void PlayFootstepAudio();
	private:
		std::map<EntityID, PlayerControllerScriptData> m_ScriptData;
		vec2 m_currentMousePos;
		bool m_mouseClicked;
		SerializationManager m_serializationManager;

		float footstepDelay{0.45f};
		float footstepTimer{footstepDelay};
	};

}