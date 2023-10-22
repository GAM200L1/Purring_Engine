/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     PhysicsManager.h
 \date     10-09-2023
 
 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu

 \brief    Contains declaration of PhysicsManager class and its member functions
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include "prpch.h"
#include "System.h"
namespace PE
{
	class PhysicsManager : public System
	{
	public:
		// ----- Constructor ----- //
		/*!***********************************************************************************
		 \brief Construct a new Physics Manager object
		 
		*************************************************************************************/
		PhysicsManager();
		
		// ----- Public Getters and Setters ----- //
		/*!***********************************************************************************
		 \brief Get the Velocity Negligence object
		 
		 \return float - m_velocityNegligence variable
		*************************************************************************************/
		float GetVelocityNegligence();

		/*!***********************************************************************************
		 \brief Set the Velocity Negligence object
		 
		 \param[in] negligence - new m_velocityNegligence value
		*************************************************************************************/
		void SetVelocityNegligence(float negligence);

		/*!***********************************************************************************
		 \brief Check if Step-by-step Physics mode is on
		 
		 \return true - In Step-by-Step Physics Mode
		 \return false - In regular Physics Simulation
		*************************************************************************************/
		static bool& GetStepPhysics();
		
		/*!***********************************************************************************
		 \brief Check if user to go to the next frame and update physics for that one frame
		 
		 \return true - Update physics by one frame
		 \return false - Do not allow physics to update
		*************************************************************************************/
		static bool& GetAdvanceStep();

		/*!***********************************************************************************
		 \brief Get the m_systemName object
		 
		 \return std::string - m_systemName variable
		*************************************************************************************/
		std::string GetName() { return m_systemName; }

		// ----- Public Methods ----- //
		/*!***********************************************************************************
		 \brief Prints to console log that PhysicsManager has successfully been initialized
		 		and added to list of systems
		 
		*************************************************************************************/
		void InitializeSystem();
		
		/*!***********************************************************************************
		 \brief Calls helper functions to update physics according to deltaTime
		 
		 \param[in] deltaTime - difference in time between previous frame and current frame
		*************************************************************************************/
		void UpdateSystem(float deltaTime);

		/*!***********************************************************************************
		 \brief Calls any destructors and frees any memory allocated
		 
		*************************************************************************************/
		void DestroySystem();

		// ----- Physics Methods ----- //
		/*!***********************************************************************************
		 \brief Updates the velocity and positions of each object with a RigidBody.
		 
		 \param[in] deltaTime - difference in time between previous frame and current frame
		*************************************************************************************/
		void UpdateDynamics(float deltaTime);

	private:
		// ----- Private Variables ----- //
		float m_velocityNegligence{};
		static bool m_applyStepPhysics;
		static bool m_advanceStep;
		std::string m_systemName{ "Physics" };
	};
}