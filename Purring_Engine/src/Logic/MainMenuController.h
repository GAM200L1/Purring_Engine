/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     MainMenuController.h
 \date     02-02-2024

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	MainMenu of the game

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "LogicSystem.h"
#include "Data/SerializationManager.h"

namespace PE
{
	struct MainMenuControllerData
	{
		EntityID SplashScreen;
		EntityID AreYouSureCanvas;
		EntityID MainMenuCanvas;

		int mouseClickEventID;
	};

	class MainMenuController : public Script
	{
	public:
		MainMenuController();
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
		std::map<EntityID, MainMenuControllerData>& GetScriptData();

		/*!***********************************************************************************
		 \brief Get the script data instance for a specific entity (RTTR)

		 \param[in] id - The unique EntityID to retrieve the script data for
		 \return rttr::instance - The script data instance for the specified entity
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);

		/*!***********************************************************************************
		 \brief			Checks if the mouse cursor is within the bounds of any GUI objects

		 \param[in]		r_ME mouse click event details
		*************************************************************************************/
		void OnMouseClick(const Event<MouseEvents>& r_ME);

	private:
		/*!***********************************************************************************
		 \brief			Activate the given object and its childrens

		 \param[in]		The objects to set active
		*************************************************************************************/
		void ActiveObject(EntityID);

		/*!***********************************************************************************
		 \brief			Deactivate the given object and its childrens

		 \param[in]		The objects to set deactivate
		*************************************************************************************/
		void DeactiveObject(EntityID);

		/*!***********************************************************************************
		 \brief			Fade the Splashscreen

		 \param[in]		the current game state manager
		 \param[in]		the time passed since the last update
		*************************************************************************************/
		void SplashScreenFade(EntityID const id, float deltaTime);

		/*!***********************************************************************************
		 \brief			Set the alpha of all given objects

		 \param[in]		EntityID the canvas to set alpha
		 \param[in]		float the alpha to set
		*************************************************************************************/
		void FadeAllObject(EntityID Canvas, float const alpha);

		void PlayGameMM(EntityID);
		void QuitGameMM(EntityID);
		void ReturnFromMMAYS(EntityID);
		void PlayClickAudio();
	private:
		//Script Variables
		std::map<EntityID, MainMenuControllerData> m_scriptData; // Data associated with each instance of the script
		SerializationManager m_serializationManager;
		bool m_firstStart{ true }, m_inSplashScreen{ false }, m_isPausedOnce{ false }, m_isResumedOnce{false};
		float m_splashTimer{ 2.f }; // Time in seconds that the splashscreen is displayed for
		float m_timeSinceEnteredState{ 0.f };
		float m_timeSinceExitedState { 0.f };
		EntityID m_currentMainMenuControllerEntityID;
	};



}
