/*!*****************************************************************************
	@file       PhysicsManager.h
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       10/9/2023

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

		

		// ----- Removed Copy Ctors/Assignments ----- //
		PhysicsManager(PhysicsManager const& r_cpyPhysicsManager) = delete;
		PhysicsManager& operator=(PhysicsManager const& r_cpyPhysicsManager) = delete;

		// ----- Public Getters and Setters ----- //
		PhysicsManager();

		void InitializeSystem();

		float GetLinearDragCoefficient();
		void SetLinearDragCoefficient(float newCoefficient);

		// ----- Public Methods ----- //
		void UpdateSystem(float deltaTime);
		void UpdateDynamics(float deltaTime); // update forces, acceleration and velocity here
		void DestroySystem();

		std::string GetName() { return m_systemName; }

	private:
		// ----- Private Variables ----- //
		float m_linearDragCoefficient;
		float m_velocityNegligence;
		bool m_applyStepPhysics;
		bool m_advanceStep;
		float m_fixedDt;
		std::string m_systemName{ "Physics" };
	};
}