/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Prefabs.h
 \date     18-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief 	Contains the Prefabs for use for default Entity compositions


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once


// INCLUDES
#include "Entity.h"
#include "GUISystem.h"

namespace PE
{
	/*!***********************************************************************************
	 \brief Contains the prefabs of the game
	 
	*************************************************************************************/
	namespace Prefabs
	{
	}

	/*!***********************************************************************************
	 \brief Prefab class handled by the entity factory
	 
	*************************************************************************************/
	class Prefab
	{
	// ----- Public variables ----- // 
	public:
		std::map<std::string, std::vector<ComponentID>> prefabs;
		
	// ----- Constructors ----- //
	public:
		/*!***********************************************************************************
		 \brief Loads all prefabs with it's attatched name into m_map
		 
		*************************************************************************************/
		Prefab()
		{
			prefabs.emplace("GameObject", std::initializer_list<ComponentID>{ EntityManager::GetInstance().GetComponentID<Transform>(), EntityManager::GetInstance().GetComponentID<Graphics::Renderer>() });
			
			prefabs.emplace("CameraObject", std::initializer_list<ComponentID>{ EntityManager::GetInstance().GetComponentID<Transform>(), EntityManager::GetInstance().GetComponentID<Graphics::Camera>() });

			prefabs.emplace("ButtonObject", std::initializer_list<ComponentID>{ EntityManager::GetInstance().GetComponentID<Transform>(), EntityManager::GetInstance().GetComponentID<GUI>(),
																							EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>() });
		}
	};
}