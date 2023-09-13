/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EntityFactory.h
 \date     11-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief 
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#pragma once
#include "prpch.h"
#include "Entity.h"
//#include "Components.h"


namespace Engine
{
	struct EntityFactory
	{
		EntityFactory();
		~EntityFactory();
		EntityID Clone(EntityID id);
		void AddComponentCreator(const ComponentID& name, ComponentCreator* creator);

		template<typename... CT>
		EntityID CreateEntity();

		template<typename... CT>
		EntityID CreateEntity(CT ... args);

		template<typename... T>
		void Assign(EntityID id, T ... var);

		template<typename T>
		void Assign(EntityID id, std::initializer_list<T> var);


		template<typename ... T>
		void Copy(EntityID id, T ... component);


		typedef std::map<ComponentID, ComponentCreator*> ComponentMapType;
		ComponentMapType m_componentMap;

		Engine::EntityManager* p_entityManager{ nullptr };
	};
	extern EntityFactory* g_entityFactory;


	template<typename... CT>
	EntityID EntityFactory::CreateEntity()
	{
		EntityID ret = p_entityManager->NewEntity();
		// credit https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list
		if constexpr (sizeof...(CT))
		{
			([&]
				{
					p_entityManager->Assign<CT>(ret);
				}
			(), ...);
		}

		return ret;
	}

	template<typename... CT>
	EntityID EntityFactory::CreateEntity(CT ... args)
	{
		EntityID ret = p_entityManager->NewEntity();
		if constexpr (sizeof...(CT))
		{
			([&]
				{
					p_entityManager->Assign<CT>(ret, args);
				}
			(), ...);
		}

		return ret;
	}

	template<typename... T>
	void EntityFactory::Assign(EntityID id, T ... var)
	{
		if constexpr (sizeof...(T))
		{
			([&]
				{
					if (typeid(T) != typeid(ComponentID) && typeid(T) != typeid(const char*))
						throw;
					if (ComponentMap.contains(var))
					{
						p_entityManager->Assign(id, var, ComponentMap[var]);
					}
				}
			(), ...);
		}
	}

	template<typename T>
	void EntityFactory::Assign(EntityID id, std::initializer_list<T> var)
	{
		if (typeid(T) != typeid(ComponentID) && typeid(T) != typeid(const char*))
			throw;
		for (const T& type : var)
		{
			if (ComponentMap.contains(type))
			{
				p_entityManager->Assign(id, type, ComponentMap[type]);
			}
		}
	}


	template<typename ... T>
	void EntityFactory::Copy(EntityID id, T ... component)
	{
		if (!p_entityManager->IsEntityValid(id))
			throw;
		if constexpr (sizeof...(T))
		{
			([&]
				{
					p_entityManager->Copy(id, component);
				}
			(), ...);
		}
	}

}

