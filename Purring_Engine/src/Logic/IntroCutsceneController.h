/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     IntroCutsceneController.h
 \date     7-2-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu


 \brief  This file contains the declarations of IntroCutsceneController that controls
		 the cutscene for the introduction of the game.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Events/EventHandler.h"
#include "Script.h"
namespace PE
{
	struct IntroCutsceneControllerData
	{
		EntityID TransitionScreen;
		EntityID Text;
		EntityID CutsceneObject;
		EntityID FinalScene;

		int windowNotFocusEventID;
		int windowFocusEventID;
	};

	class IntroCutsceneController : public Script
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
		 \brief Get the script data for all entities

		 \return std::map<EntityID, TestScriptData>& - A map of EntityID to TestScriptData
		*************************************************************************************/
		std::map<EntityID, IntroCutsceneControllerData>& GetScriptData();

		/*!***********************************************************************************
		 \brief Get the script data instance for a specific entity (RTTR)

		 \param[in] id - The unique EntityID to retrieve the script data for
		 \return rttr::instance - The script data instance for the specified entity
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);

		/*!***********************************************************************************
		 \brief Function for UI button to continue to level

		 \param[in] id - The unique EntityID.
		*************************************************************************************/
		void ContinueToLevel(EntityID id);

		/*!***********************************************************************************
		 \brief Function to play click audio.
		*************************************************************************************/
		void PlayClickAudio();

		/*!***********************************************************************************
		 \brief Function to play click audio.
		*************************************************************************************/
		void PlaySceneTransitionAudio();

		/*!***********************************************************************************
		 \brief Get the script data instance for a specific entity (RTTR)

		 \param[in] id - The unique EntityID to retrieve the script data for
		 \return rttr::instance - The script data instance for the specified entity
		*************************************************************************************/
		IntroCutsceneController();

		/*!***********************************************************************************
		 \brief			Fade the Planning State HUD

		 \param[in]		the current game state manager
		 \param[in]		the time passed since the last update
		*************************************************************************************/
		void TransitionPanelFade(EntityID const id, float deltaTime);

		/*!***********************************************************************************
		 \brief			Set the alpha of all given objects

		 \param[in]		EntityID the canvas to set alpha
		 \param[in]		float the alpha to set
		*************************************************************************************/
		void FadeAllObject(EntityID id, float const alpha);

		/*!***********************************************************************************
		 \brief			Handle window out of focus event.
		 \param[in]     Event containing window-specific details.
		*************************************************************************************/
		void OnWindowOutOfFocus(const PE::Event<PE::WindowEvents>& r_event);

		/*!***********************************************************************************
		 \brief			Handle window out of focus event.
		 \param[in]     Event containing window-specific details.
		*************************************************************************************/
		void OnWindowFocus(const PE::Event<PE::WindowEvents>& r_event);

	private:
		std::map<EntityID, IntroCutsceneControllerData> m_scriptData;
		SerializationManager m_serializationManager;
		float m_sceneTimer{ 22.3f };
		float m_elapsedTime{ 0 };
		bool m_endCutscene{ false };
		bool m_startCutscene{ true };

		float m_transitionTimer{ 1.f };
		float m_transitionElapsedTime{ 0 };
		bool m_isTransitioning{ false };

		// Fade out variable
		bool m_isFadingOut{ false };
	};


}