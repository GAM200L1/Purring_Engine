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

		EntityID HowToPlayCanvas;
		EntityID HowToPlayPageOne;
		EntityID HowToPlayPageTwo;

		EntityID SettingsMenu;

		EntityID TransitionPanel;

		int mouseClickEventID;
		int windowNotFocusEventID;
		int windowFocusEventID;
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

		/*!***********************************************************************************
		 \brief			Set the main menu to not load the splash screen
		 \param[in]     Event containing window-specific details.
		*************************************************************************************/
		void NotFirstStart();

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
		 \brief			Fade the Planning State HUD

		 \param[in]		the current game state manager
		 \param[in]		the time passed since the last update
		*************************************************************************************/
		void TransitionPanelFade(EntityID const id, float deltaTime, bool in);

		/*!***********************************************************************************
		 \brief			Set the alpha of all given objects

		 \param[in]		EntityID the canvas to set alpha
		 \param[in]		float the alpha to set
		*************************************************************************************/
		void FadeAllObject(EntityID Canvas, float const alpha);

		/*!***********************************************************************************
		 \brief			Play the game

		 \param[in]		for button
		*************************************************************************************/
		void PlayGameMM(EntityID);
		/*!***********************************************************************************
		 \brief			Quit the game

		 \param[in]		for button
		*************************************************************************************/
		void QuitGameMM(EntityID);
		/*!***********************************************************************************
		 \brief			Return from Are You Sure to the Main Menu

		 \param[in]		for button
		*************************************************************************************/
		void ReturnFromMMAYS(EntityID);
		/*!***********************************************************************************
		 \brief			Close How To Play Canvas

		 \param[in]		EntityID so that this function can be called by a buttopn
		*************************************************************************************/
		void MMCloseHTP(EntityID);
		/*!***********************************************************************************
		 \brief			Open How To Play Canvas

		 \param[in]		EntityID so that this function can be called by a buttopn
		*************************************************************************************/
		void MMOpenHTP(EntityID);
		/*!***********************************************************************************
		 \brief			Change to How To Play Page 2

		 \param[in]		EntityID so that this function can be called by a buttopn
		*************************************************************************************/
		void MMHTPPage2(EntityID);
		/*!***********************************************************************************
		 \brief			Change to How To Play Page 1

		 \param[in]		EntityID so that this function can be called by a buttopn
		*************************************************************************************/
		void MMHTPPage1(EntityID);

		/*!***********************************************************************************
		 \brief			Open Settings in Main Menu

		 \param[in]		EntityID so that this function can be called by a button
		*************************************************************************************/
		void MMOpenSettings(EntityID);

		/*!***********************************************************************************
		 \brief			Close Settings in Main Menu

		 \param[in]		EntityID so that this function can be called by a button
		*************************************************************************************/
		void MMCloseSettings(EntityID);

		/*!***********************************************************************************
		 \brief			Click Audio Sound
		*************************************************************************************/
		void PlayClickAudio();
		/*!***********************************************************************************
		 \brief			Click Positive Feedback Sound
		*************************************************************************************/
		void PlayPositiveFeedback();
		/*!***********************************************************************************
		 \brief			Click Negative Feedback Sound
		*************************************************************************************/
		void PlayNegativeFeedback();

		/*!***********************************************************************************
		 \brief			Button Hover Audio Sound
		*************************************************************************************/
		void PlayButtonHoverAudio(EntityID);
	private:
		//Script Variables
		std::map<EntityID, MainMenuControllerData> m_scriptData; // Data associated with each instance of the script
		SerializationManager m_serializationManager;
		bool m_firstStart{ true }, m_inSplashScreen{ false }, m_isPausedOnce{ false }, m_isResumedOnce{false};
		float m_splashTimer{ 2.f }; // Time in seconds that the splashscreen is displayed for
		float m_timeSinceEnteredState{ 0.f };
		float m_timeSinceExitedState { 0.f };
		EntityID m_currentMainMenuControllerEntityID;

		const float m_transitionTimer{ .75f };
		float m_timeSinceTransitionStarted{};
		float m_timeSinceTransitionEnded{};
		bool m_isTransitioning{ false };
		bool m_isTransitioningIn{ false };
	};



}
