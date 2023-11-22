/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     testScript2.h
 \date     03-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan\@digipen.edu


 \brief  This file contains the declarations of testScript2

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "StateManager.h"
namespace PE
{
	struct TestScript2Data
	{
		StateMachine* m_stateMachine;
	};

	class testScript2 : public Script
	{
	public:
		/*!***********************************************************************************
		 \brief Initialize the testScript2 for an entity (RTTR)

		 \param[in] id - The unique EntityID to initialize the script for
		 \return void
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief Update the testScript2 for an entity (RTTR)

		 \param[in] id - The unique EntityID to update the script for
		 \param[in] deltaTime - The time passed since the last update
		 \return void
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Destroy the testScript2 for an entity (RTTR)

		 \param[in] id - The unique EntityID to destroy the script for
		 \return void
		*************************************************************************************/
		virtual void Destroy(EntityID id);		

		/*!***********************************************************************************
		 \brief Attach the testScript2 to an entity (RTTR)

		 \param[in] id - The unique EntityID to attach the script to
		 \return void
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		 \brief Detach the testScript2 from an entity (RTTR)

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
		 \brief Destructor for the testScript2

		 \return void
		*************************************************************************************/
		~testScript2();
	private:
		std::map<EntityID, TestScript2Data> m_ScriptData;
	};

	class TestScript2IDLE : public State
	{
	public:

		/*!***********************************************************************************
		 \brief Perform actions when entering the "Idle" state.

		 This function is called when an entity enters the "Idle" state.

		 \param[in] id - The unique EntityID associated with the state transition
		 \return void
		*************************************************************************************/
		virtual void StateEnter(EntityID) override { std::cout << "Enter Idle" << std::endl; }

		/*!***********************************************************************************
		 \brief Perform actions during the "Idle" state update.

		 This function is called during the update of the "Idle" state.

		 \param[in] id - The unique EntityID associated with the state update
		 \param[in] deltaTime - The time elapsed since the last update
		 \return void
		*************************************************************************************/
		virtual void StateUpdate(EntityID, float) override { std::cout << "Update Idle" << std::endl; }

		/*!***********************************************************************************
		 \brief Perform actions when exiting the "Idle" state.

		 This function is called when an entity exits the "Idle" state.

		 \param[in] id - The unique EntityID associated with the state transition
		 \return void
		*************************************************************************************/
		virtual void StateExit(EntityID) override { std::cout << "Exit Idle" << std::endl; }

		/*!***********************************************************************************
		 \brief Get the name of the state.

		 This function returns the name of the "Idle" state.

		 \return std::string_view - The name of the state, which is "IDLE"
		*************************************************************************************/
		virtual std::string_view GetName() override { return "IDLE"; }
	};

	class TestScript2JIGGLE : public State
	{
	public:

		/*!***********************************************************************************
		 \brief Perform actions when entering the "Jiggle" state.

		 This function is called when an entity enters the "Jiggle" state.

		 \param[in] id - The unique EntityID associated with the state transition
		 \return void
		*************************************************************************************/
		virtual void StateEnter(EntityID) override { std::cout << "Enter Jiggle" << std::endl; }

		/*!***********************************************************************************
		 \brief Perform actions during the "Jiggle" state update.

		 This function is called during the update of the "Jiggle" state.

		 \param[in] id - The unique EntityID associated with the state update
		 \param[in] deltaTime - The time elapsed since the last update
		 \return void
		*************************************************************************************/
		virtual void StateUpdate(EntityID, float) override { std::cout << "Update Jiggle" << std::endl; }

		/*!***********************************************************************************
		 \brief Perform actions when exiting the "Jiggle" state.

		 This function is called when an entity exits the "Jiggle" state.

		 \param[in] id - The unique EntityID associated with the state transition
		 \return void
		*************************************************************************************/
		virtual void StateExit(EntityID) override { std::cout << "Exit Jiggle" << std::endl; }

		/*!***********************************************************************************
		 \brief Get the name of the state.

		 This function returns the name of the "Jiggle" state.

		 \return std::string_view - The name of the state, which is "JIGGLE"
		*************************************************************************************/
		virtual std::string_view GetName() override { return "JIGGLE"; }
	};
}