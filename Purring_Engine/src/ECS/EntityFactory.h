/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EntityFactory.h
 \date     11-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief 	This file contains the Entity factory implementation.
			The entity factory can be used to generate entities using various methods

			Creating an empty entity with no components, cloning a existing entity,
			assigning specific components, are implemented here.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#pragma once
#include "prpch.h"
#include "Entity.h"
//#include "Components.h"


namespace Engine
{
	/*!***********************************************************************************
	 \brief Enity factory struct
	 
	*************************************************************************************/
	class EntityFactory
	{
	// ----- Constructors -----//
	public:
	    /*!***********************************************************************************
		\brief Construct a new Entity Factory:: Entity Factory object
				Also ensures that only one instance of the entity factory exists,
				in the event another the creation of another instance has occured, it will
				throw an exception.
		
		*************************************************************************************/
		EntityFactory();

		/*!***********************************************************************************
		\brief Destroy the Entity Factory:: Entity Factory object
				Deletes pointers within the component map
		
		*************************************************************************************/
		~EntityFactory();
	// ----- Public Methods -----//
	public:
		/*!***********************************************************************************
		 \brief Clones an existing entity by way of using it's ID
		 
		 \param[in] id 		entity ID to clone compoenents & data from
		 \return EntityID 	the cloned entity
		*************************************************************************************/
		EntityID Clone(EntityID id);

		/*!***********************************************************************************
		 \brief Adds a component to be used in the factory and the ECS as a whole
		 		This function is not to be called directly, and is to be called using the
				REGISTERCOMPONENT() macro, where the first param is the class/struct, and
				the second is the size of that struct/class.

				e.g. REGISTERCOMPONENT(Position, sizeof(Position));
		 
		 \param[in] name 	The ID for the component
		 \param[in] creator Component creator class that is only used for the size of the
		 					class/struct
		*************************************************************************************/
		void AddComponentCreator(const ComponentID& name, ComponentCreator* creator);

		/*!***********************************************************************************
		 \brief Create a Entity object with input component types.
		 		Variadic template function, any number of components can be added.
				Values are NOT initialized
		 
		 \tparam CT 		The components to attatch to the returned entity, if none is
		 					provided, it will have no components.
		 \return EntityID 	The entity generated with provided components
		*************************************************************************************/
		template<typename... CT>
		EntityID CreateEntity();

		/*!***********************************************************************************
		 \brief Create a Entity object with input component types, values of the input 
		 		components in args, will be copied over after the component is assigned to
				the newly created entity
		 
		 \tparam CT 		No need to fill this. The function input params are enough.
		 \param[in] args 	The components to copy/initialize to the newly created entity with.
		 \return EntityID 	The ID of the entity generated
		*************************************************************************************/
		template<typename... CT>
		EntityID CreateEntity(CT ... args);

		/*!***********************************************************************************
		 \brief Assings components to the entity
		 
		 \tparam T 			No need to fill, the input parameters are enough
		 \param[in] id 		The ID of the entity to assign the components to
		 \param[in] var 	The components to assign the entity
		*************************************************************************************/
		template<typename... T>
		void Assign(EntityID id, T ... var);

		/*!***********************************************************************************
		 \brief Assigns components to an entity using an inisializer list of componentIDs
		 
		 \tparam T 			The ComponentID(std::string) data type or 
		 					a c-style string (const char*) can be used
		 \param[in] id 		The id of the entity to assign the components
		 \param[in] var 	The initializer list of components
		*************************************************************************************/
		template<typename T>
		void Assign(EntityID id, std::initializer_list<T> var);

		/*!***********************************************************************************
		 \brief Copies the components in input arguments into the entity
		 		Similar behaviour to Assign.
		 
		 \tparam T 				No need to fill, the input parameters are enough
		 \param[in] id 			The entity to copy the componnets with values to
		 \param[in] component 	The components with values to copy from
		*************************************************************************************/
		template<typename ... T>
		void Copy(EntityID id, T ... component);

		// Hans
		void AssignComponent(EntityID id, const std::string& name, int componentData);


	// ----- Private Variables ----- //
	private:
		typedef std::map<ComponentID, ComponentCreator*> ComponentMapType; // component map typedef
		ComponentMapType m_componentMap;								   // component map (ID, ptr to creator)
		Engine::EntityManager* p_entityManager{ nullptr };				   // pointer to entity manager
	};

	// extern for those including the .h to access the factory instance
	extern EntityFactory* g_entityFactory;

	
	//-------------------- Templated function implementations --------------------//

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

