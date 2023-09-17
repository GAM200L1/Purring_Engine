/*!*****************************************************************************
	@file       PhysicsManager.h
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

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

		// ----- Public Getters ----- //
		PhysicsManager();
		PhysicsManager* GetInstance();

		// ----- Public Methods ----- //
		void UpdateDynamics(float deltaTime); // update forces, acceleration and velocity here
		void UpdatePositions(float deltaTime); // update positions here

	private:
		// ----- Private Variables ----- //
		PhysicsManager* m_ptrInstance;
		float m_worldGravity;
	};
}