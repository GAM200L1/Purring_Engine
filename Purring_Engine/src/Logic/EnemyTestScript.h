/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EnemyTestScript.h
 \date     02-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Script that controls the enemy states. For M2 demo purposes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#pragma once
#include "Script.h"
#include "StateManager.h"

namespace PE {
	struct EnemyTestScriptData
	{
		EntityID playerID{ 1 };
		float speed{ 5000 };
		float idleTimer{ 3.0f };
		float alertTimer{ 0 };
		float timerBuffer{ 3.0f };
		float patrolTimer{ 5.0f };
		float patrolBuffer{ 3.0f };
		float distanceFromPlayer{ 0.0f };
		float TargetRange{ 200 };
		float Health{ 1 };
		bool bounce{ true };
		StateMachine* m_stateManager;
	};

	class EnemyTestScript : public PE::Script
	{
	public:
		/*!***********************************************************************************
		\brief Creates a statemachine for the enemy and sets it to idle by default.
		
		\param[in,out] id - ID of the script to update
		*************************************************************************************/
		virtual void Init(EntityID id);
		/*!***********************************************************************************
		 \brief Updates the state of the enemy based on its distance from the player
		 
		 \param[in,out] id - ID of the instance to update
		 \param[in,out] deltaTime - delta time used to update the state
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);
		/*!***********************************************************************************
		 \brief Does nothing
		*************************************************************************************/
		virtual void Destroy(EntityID id);
		/*!***********************************************************************************
		 \brief Ensures that the enemy has all the necessary components for this script to work.
		 
		 \param[in,out] id - ID of Instance of script to update
		*************************************************************************************/
		virtual void OnAttach(EntityID id);
		/*!***********************************************************************************
		 \brief Clears the script data
		 
		 \param[in,out] id - ID of instance of script to clear
		*************************************************************************************/
		virtual void OnDetach(EntityID id);
		/*!***********************************************************************************
		 \brief Get the Script Data object		 
		 
		 \return std::map<EntityID, CameraManagerScriptData>& Map of script data.
		*************************************************************************************/
		std::map<EntityID, EnemyTestScriptData>& GetScriptData();

		/*!***********************************************************************************
		 \brief Get the Script Data object
		 
		 \param[in,out] id - ID of the script to get the data from
		 \return rttr::instance Instance of the script to get the data from
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);

		/*!***********************************************************************************
		 \brief Destroy the Enemy Test Script objects in the map		 
		*************************************************************************************/
		virtual ~EnemyTestScript();

	public:
		std::map<EntityID, EnemyTestScriptData> m_ScriptData;

	private:
		/*!***********************************************************************************
		\brief Get the Distance of this enemy from the Player object
		
		\param[in,out] id - ID of instance of script 
		\return float Distance of enemy from player
		*************************************************************************************/
		float GetDistanceFromPlayer(EntityID id);
	};

	class EnemyTestIDLE : public State
	{
		/*!***********************************************************************************
		 \brief Play the idle animation and start the timer
		 
		 \param[in,out] id - ID of instance of script 
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;
		/*!***********************************************************************************
		 \brief Checks if its state should change
		 
		 \param[in,out] id - ID of instance of script 
		 \param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;
		/*!***********************************************************************************
		 \brief does nothing
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;
		/*!***********************************************************************************
		 \brief Get the name of the state
		 
		 \return std::string_view name of state
		*************************************************************************************/
		virtual std::string_view GetName() override;
		/*!***********************************************************************************
		 \brief Sets the data to null		 
		*************************************************************************************/
		virtual ~EnemyTestIDLE() { p_data = nullptr; }
	private:
		EnemyTestScriptData* p_data;
	};

	class EnemyTestPATROL : public State
	{
		/*!***********************************************************************************
		 \brief Start the timer to change the state
		 
		 \param[in,out] id - ID of instance of script 
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;
		/*!***********************************************************************************
		 \brief Checks if its state should change and updates the animation of the enemy
		 
		 \param[in,out] id - ID of instance of script 
		 \param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;
		/*!***********************************************************************************
		 \brief does nothing
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;
		/*!***********************************************************************************
		 \brief Get the name of the state
		 
		 \return std::string_view name of state
		*************************************************************************************/
		virtual std::string_view GetName() override;
		/*!***********************************************************************************
		 \brief Sets the data to null		 
		*************************************************************************************/
		virtual ~EnemyTestPATROL() { p_data = nullptr; }
	private:
		EnemyTestScriptData* p_data;
	};

	class EnemyTestALERT : public State
	{
		/*!***********************************************************************************
		 \brief Start the timer to change the state
		 
		 \param[in,out] id - ID of instance of script 
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;
		/*!***********************************************************************************
		 \brief Checks if its state should change
		 
		 \param[in,out] id - ID of instance of script 
		 \param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;
		/*!***********************************************************************************
		 \brief does nothing
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;
		/*!***********************************************************************************
		 \brief Get the name of the state
		 
		 \return std::string_view name of state
		*************************************************************************************/
		virtual std::string_view GetName() override;
		/*!***********************************************************************************
		 \brief Sets the data to null		 
		*************************************************************************************/
		virtual ~EnemyTestALERT() { p_data = nullptr; }
	private:
		EnemyTestScriptData* p_data;
	};

	class EnemyTestTARGET : public State
	{
		/*!***********************************************************************************
		 \brief Start the timer to change the state and get the data of the enemy
		 
		 \param[in,out] id - ID of instance of script 
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;
		/*!***********************************************************************************
		 \brief Checks if its state should change
		 
		 \param[in,out] id - ID of instance of script 
		 \param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;
		/*!***********************************************************************************
		 \brief does nothing
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;
		/*!***********************************************************************************
		 \brief Get the name of the state
		 
		 \return std::string_view name of state
		*************************************************************************************/
		virtual std::string_view GetName() override;
		/*!***********************************************************************************
		 \brief Sets the data to null		 
		*************************************************************************************/
		virtual ~EnemyTestTARGET() { p_data = nullptr; }
	private:
		EnemyTestScriptData* p_data;
	};

	class EnemyTestATTACK : public State
	{
		/*!***********************************************************************************
		 \brief Get the data of the enemy
		 
		 \param[in,out] id - ID of instance of script 
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;
		/*!***********************************************************************************
		 \brief Checks if its state should change
		 
		 \param[in,out] id - ID of instance of script 
		 \param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;
		/*!***********************************************************************************
		 \brief does nothing
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;
		/*!***********************************************************************************
		 \brief Get the name of the state
		 
		 \return std::string_view name of state
		*************************************************************************************/
		virtual std::string_view GetName() override;
		/*!***********************************************************************************
		 \brief Sets the data to null		 
		*************************************************************************************/
		virtual ~EnemyTestATTACK() { p_data = nullptr; }
	private:
		EnemyTestScriptData* p_data;
	};
}
