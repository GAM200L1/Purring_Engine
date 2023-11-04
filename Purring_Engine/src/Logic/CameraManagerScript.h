/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CameraManagerScript.h
 \date     02-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Script that switches the main camera between multiple cameras in the scene.
	For M2 demo purposes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/


#pragma once
#include "Script.h"
#include "Events/EventHandler.h"

namespace PE
{
	/*!***********************************************************************************
	 \brief Data to to be stored for this scene.	 
	*************************************************************************************/
	struct CameraManagerScriptData
	{
		int NumberOfCamera;
		std::vector<EntityID> CameraIDs;
	};

	class CameraManagerScript : public Script
	{
	public:
		/*!***********************************************************************************
		\brief Subscribes to a key event.
		
		\param[in,out] id - Not used.
		*************************************************************************************/
		virtual void Init(EntityID id);
		/*!***********************************************************************************
		 \brief Changes main camera if key has been presseed
		 
		 \param[in,out] id  - Not used.
		 \param[in,out] deltaTime  - Not used.
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);
		/*!***********************************************************************************
		 \brief Erase script data
		 
		 \param[in,out] id - Not used.
		*************************************************************************************/
		virtual void Destroy(EntityID id);
		/*!***********************************************************************************
		 \brief Adds a bunch of cameras to the scene
		 
		 \param[in,out] id - ID of the scriptdata to update
		*************************************************************************************/
		virtual void OnAttach(EntityID id);
		/*!***********************************************************************************
		 \brief Erase all the data tied to this ID
		 
		 \param[in,out] id - Not used.
		*************************************************************************************/
		virtual void OnDetach(EntityID id);
		/*!***********************************************************************************
		 \brief Get the Script Data object
		 
		 \return std::map<EntityID, CameraManagerScriptData>& Map of script data.
		*************************************************************************************/
		std::map<EntityID, CameraManagerScriptData>& GetScriptData();
		/*!***********************************************************************************
		 \brief Get the Script Data object
		 
		 \param[in,out] id - ID of the script to get the data from
		 \return rttr::instance Instance of the script to get the data from
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);
		/*!***********************************************************************************
		 \brief Does nothing		 
		*************************************************************************************/
		virtual ~CameraManagerScript();
	private:
		/*!***********************************************************************************
		\brief Checks if the key to switch cameras has been pressed
		
		\param[in,out] r_ME Key event information
		*************************************************************************************/
		void OnKeyTriggered(const Event<KeyEvents>& r_ME);
		/*!***********************************************************************************
		 \brief Switch the main camera.
		 
		 \param[in,out] id - ID of the script that the camera is attached to.
		*************************************************************************************/
		void ChangeCamera(EntityID id);
	private:
		std::map<EntityID, CameraManagerScriptData> m_ScriptData;
		bool m_keyPressed; // true if the key to switch cameras has been pressed
		int	 m_key;

	};

}