/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     VisualEffectsManager.h
 \date     15-12-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu
 \par      code %:     90%
 \par      changes:    Created Base defenitions and functions

 \co-author            Foong Jun Wei 
 \par      email:      f.junwei@digipen.edu
 \par      code %:     10%
 \par      changes:    Commented
 
 \brief		Contains the function declerations for the visual effects manager


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "System.h"

namespace PE
{
	class VisualEffectsManager : public System
	{
	public:
		/*!***********************************************************************************
		 \brief Construct a new Visual Effects Manager object
		 
		*************************************************************************************/
		VisualEffectsManager() = default;
		/*!***********************************************************************************
		 \brief Initializes the system
		 
		*************************************************************************************/
		void InitializeSystem();

		/*!***********************************************************************************
		 \brief Updates the system according to the delta time
		 
		 \param[in] deltaTime 
		*************************************************************************************/
		void UpdateSystem(float deltaTime);

		/*!***********************************************************************************
		 \brief Cleans up the resources used by the system
		 
		*************************************************************************************/
		void DestroySystem();

		/*!***********************************************************************************
		 \brief Get the name of the system which is VisualEffectsManager

		 \return std::string - m_systemName variable
		*************************************************************************************/
		std::string GetName() { return m_systemName; }

	private:
		std::string m_systemName{ "VisualEffectsManager" };
	};
}