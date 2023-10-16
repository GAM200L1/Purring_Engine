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
#include "EntityFactory.h"
#include "Entity.h"
#include "Components.h"
#include "Data/SerializationManager.h"
#include "Physics/Colliders.h"
#include "Physics/RigidBody.h"
#include "Graphics/Renderer.h"
//#include "Logic/Script.h"
//#include "Logic/LogicSystem.h"
#include "Graphics/Camera.h"
#include "Prefabs.h"
#include "Singleton.h"

// Entity Factory code
namespace PE
{
	/*!***********************************************************************************
	 \brief Enity factory struct
	 
	*************************************************************************************/
	class EntityFactory : public Singleton<EntityFactory>
	{
		friend class Singleton<EntityFactory>;
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
				REGISTERCOMPONENT() macro, where the first param is the class/struct

				e.g. REGISTERCOMPONENT(Position);
		 
		 \param[in] name 	The ID for the component
		 \param[in] creator Component creator class that is only used for the size of the
		 					class/struct
		*************************************************************************************/
		template <typename T>
		void AddComponentCreator(const ComponentID& r_name, const size_t& r_creator)
		{
			r_name;
			m_componentMap[EntityManager::GetInstance().GetComponentID<T>()] = r_creator;
			EntityManager::GetInstance().AddToPool<T>();
		}
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
		 \brief Assigns components to an entity using an inisializer list of componentIDs
		 
		 \tparam T 			The ComponentID(std::string) data type or 
		 					a c-style string (const char*) can be used
		 \param[in] id 		The id of the entity to assign the components
		 \param[in] var 	The initializer list of components
		*************************************************************************************/
		void Assign(EntityID id, const std::initializer_list<ComponentID>& var);

		/*!***********************************************************************************
		 \brief Assigns components to an entity using an vector list of componentIDs

		 \tparam T 			The ComponentID(std::string) data type or
							a c-style string (const char*) can be used
		 \param[in] id 		The id of the entity to assign the components
		 \param[in] var 	The initializer list of components
		*************************************************************************************/
		//template<typename T>
		//void Assign(EntityID id, const std::vector<T>& r_var);

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
		void AssignComponent(EntityID id, const std::string& r_name, int componentData);

		/*!***********************************************************************************
		 \brief Create an enity from Prefab object
		 
		 \param[in] prefab 	The name of the prefab to load from
		 \return EntityID 	The created entity's ID
		*************************************************************************************/
		EntityID CreateFromPrefab(const char* p_prefab);

		/*!***********************************************************************************
		 \brief 	Loads a component including data into an entity, if the entity doesn't
		 			have this component added, it will assign it before copying the data over.
		 
		 \param[in] id 			The ID of the entity to load the component into
		 \param[in] component 	The name of the component to load into the entity
		 \param[in] data 		The data to load into the entity
		 \return true 			Successfully copied the component
		 \return false 			Failed to copy the component
		*************************************************************************************/
		bool LoadComponent(EntityID id, const ComponentID& r_component, void* p_data);


		// ----- Private Variables ----- //
	private:
		typedef bool(EntityFactory::*fnptrVoidptrConstruct)(const EntityID& r_id, void* p_data);
		typedef std::map<ComponentID, size_t, Comparer> ComponentMapType; // component map typedef
		ComponentMapType m_componentMap;								   // component map (ID, ptr to creator)
		PE::EntityManager* p_entityManager{ nullptr };				   // pointer to entity manager
		std::map<ComponentID, fnptrVoidptrConstruct, Comparer> m_initializeComponent;
		Prefab m_prefabs;

	// ----- Private Methods ----- //
	private:

	// ----- Components Handling ----- //

		/*!***********************************************************************************
		 \brief Initializes/copy the component of the specified entity
		 
		 \param[in] id 	 The ID of the entity to initialize/copy the data to
		 \param[in] data The component casted to a void pointer (universal way of passing the
		 				 data)
		 \return true 	 Successfully copied/initialized
		 \return false 	 Failed to copy/initialize
		*************************************************************************************/
		bool InitializeRigidBody(const EntityID& r_id, void* p_data);
		bool InitializeCollider(const EntityID& r_id, void* p_data);
		bool InitializeTransform(const EntityID& r_id, void* p_data);
		bool InitializePlayerStats(const EntityID& r_id, void* p_data);
		bool InitializeRenderer(const EntityID& r_id, void* p_data);
		bool InitializeScriptComponent(const EntityID& r_id, void* p_data);
		bool InitializeCamera(const EntityID& r_id, void* p_data);

		/*!***********************************************************************************
		 \brief Loads all the component initializers into m_componentMap
		 
		*************************************************************************************/
		void LoadComponents();
	};


	
	//-------------------- Templated function implementations --------------------//

	template<typename... CT>
	EntityID EntityFactory::CreateEntity()
	{
		EntityID ret = p_entityManager->NewEntity();
		if constexpr (sizeof...(CT))
		{
			([&]
				{
					p_entityManager->Assign<CT>(ret);
				}
			(), ...);
		}
		p_entityManager->UpdateVectors(ret);
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
		p_entityManager->UpdateVectors(ret);
		return ret;
	}



	//template<typename T>
	//void EntityFactory::Assign(EntityID id, const std::vector<T>& r_var)
	//{
	//	if (typeid(T) != typeid(ComponentID) && typeid(T) != typeid(const char*))
	//	{
	//		engine_logger.AddLog(true, "T was not a string!!", __FUNCTION__);
	//		engine_logger.FlushLog();
	//		throw;
	//	}
	//	for (const T& r_type : r_var)
	//	{
	//		if (m_componentMap.at(r_type) != m_componentMap.end())
	//		{
	//			p_entityManager->Assign(id, r_type);
	//		}
	//	}
	//	p_entityManager->UpdateVectors(id);
	//}


	template<typename ... T>
	void EntityFactory::Copy(EntityID id, T ... component)
	{
		if (!p_entityManager->IsEntityValid(id))
		{
			engine_logger.AddLog(true, "Entity was not valid!!", __FUNCTION__);
			engine_logger.FlushLog();
			throw;
		}
		if constexpr (sizeof...(T))
		{
			([&]
				{
					p_entityManager->Copy(id, component);
				}
			(), ...);
		}
		p_entityManager->UpdateVectors(id);
	}

}

