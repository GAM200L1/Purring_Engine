/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     VisualEffectsManager.h
 \date     15-12-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "System.h"

namespace PE
{
	class VisualEffectsManager : public System
	{
	public:
		VisualEffectsManager() = default;

		void InitializeSystem();

		void UpdateSystem(float deltaTime);

		void DestroySystem();

		/*!***********************************************************************************
		 \brief Get the name of the system which is Collision

		 \return std::string - m_systemName variable
		*************************************************************************************/
		std::string GetName() { return m_systemName; }

	private:
		std::string m_systemName{ "VisualEffectsManager" };
	};
}