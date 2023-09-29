/*!*****************************************************************************
	@file       PhysicsManager.h
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       10-9-2023

	@brief
	
All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#pragma once
#include "prpch.h"
#include "System.h"
namespace PE
{
	class PhysicsManager : public System
	{
	public:
		// ----- Constructor ----- //
		PhysicsManager();
		
		// ----- Public Getters and Setters ----- //
		float GetLinearDragCoefficient();
		void SetLinearDragCoefficient(float newCoefficient);

		float GetVelocityNegligence();
		void SetVelocityNegligence(float negligence);

		//float GetFixedDt();
		//void SetFixedDt(float fixDt);

		static bool& GetStepPhysics();
		static bool& GetAdvanceStep();

		std::string GetName() { return m_systemName; }

		// ----- Public Methods ----- //
		void InitializeSystem();
		void UpdateSystem(float deltaTime);
		void DestroySystem();

		// ----- Physics Methods ----- //
		void UpdateDynamics(float deltaTime);

	private:
		// ----- Private Variables ----- //
		float m_linearDragCoefficient{};
		float m_velocityNegligence{};
		//float m_fixedDt{};
		//float m_accumulator{};
		//float m_accumulatorLimit{};
		static bool m_applyStepPhysics;
		static bool m_advanceStep;
		std::string m_systemName{ "Physics" };
	};
}