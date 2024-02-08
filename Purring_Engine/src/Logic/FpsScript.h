/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     FpsScript.h
 \date     7-2-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu


 \brief  This file contains the declarations of FpsScript that displays the FPS of the
		 game through a text object

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "Events/EventHandler.h"

namespace PE
{
	struct FpsScriptData
	{

	};

	class FpsScript : public Script
	{
	public:
		/*!***********************************************************************************
		 \brief Initialize the test script for an entity (RTTR)

		 \param[in] id - The unique EntityID to initialize the script for
		 \return void
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief Update the test script for an entity (RTTR)

		 \param[in] id - The unique EntityID to update the script for
		 \param[in] deltaTime - The time passed since the last update
		 \return void
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Destroy the test script for an entity (RTTR)

		 \param[in] id - The unique EntityID to destroy the script for
		 \return void
		*************************************************************************************/
		virtual void Destroy(EntityID id);

		/*!***********************************************************************************
		 \brief Attach the test script to an entity (RTTR)

		 \param[in] id - The unique EntityID to attach the script to
		 \return void
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		 \brief Detach the test script from an entity (RTTR)

		 \param[in] id - The unique EntityID to detach the script from
		 \return void
		*************************************************************************************/
		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Get the script data instance for a specific entity (RTTR)

		 \param[in] id - The unique EntityID to retrieve the script data for
		 \return rttr::instance - The script data instance for the specified entity
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);

		/*!***********************************************************************************
		\brief      Handle keyboard-specific events.

		\param[in]  r_event Event containing keyboard-specific details.
		*************************************************************************************/
		void OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event);
	private:
		std::map<EntityID, FpsScriptData> m_scriptData;
		float m_elapsedTime;
		EntityID m_currentEntityID;
		int m_keyPressedKey;
	};


}