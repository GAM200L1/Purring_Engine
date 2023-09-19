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

namespace PE
{
	class PhysicsManager
	{
	public:
		// ----- Removed Copy Ctors/Assignments ----- //
		PhysicsManager(PhysicsManager const& r_cpyPhysicsManager) = delete;
		PhysicsManager& operator=(PhysicsManager const& r_cpyPhysicsManager) = delete;

		// ----- Public Getters and Setters ----- //
		PhysicsManager() = default;
		static PhysicsManager* GetInstance();

		static float GetLinearDragCoefficient();
		static void SetLinearDragCoefficient(float newCoefficient);

		// ----- Public Methods ----- //
		static void UpdateDynamics(float deltaTime); // update forces, acceleration and velocity here

	private:
		// ----- Private Variables ----- //
		static PhysicsManager* m_ptrInstance;
		static float m_linearDragCoefficient;
	};
}