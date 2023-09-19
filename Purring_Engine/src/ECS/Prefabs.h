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
#include "prpch.h"
#include "Entity.h"
#include "Components.h"
#include "EntityFactory.h"
#include "Data/SerializationManager.h"


namespace PE
{
	namespace Prefabs
	{
		constexpr std::initializer_list<const char*> GameObject = { "RigidBody", "Collider", "Transform" };
	}
	class Prefab
	{
	public:
		std::map<std::string, std::initializer_list<const char*>> m_map;

		void LoadPrefabs()
		{
			//LoadComponents();
			m_map.emplace("GameObject", Prefabs::GameObject);
		}
	
	};

}