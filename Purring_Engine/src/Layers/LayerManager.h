/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     LayerManager.h
 \date     10-01-2024

 \author               FOONG Jun Wei
 \par      email:      f.junwei@digipen.edu

 \brief	   This file contains the implementation of a Layering system manager.
		   It is able to group entities together.
		   With caches for different component combinations.

		   It is a overall replacement for the functionality of SceneView, adding 
		   support for the enabling and disabling of entire layers at once.
		   If a layer is disabled, all instances that use the LayerView will not 
		   see it when iterating through the containers (so those entities) should
		   not be updated in those systems.
		   
		   Note: To temporarily disable this behaviour for the LayerView, add true
		   to the paranthesis of the LayerView call e.g.
		   for (auto id : LayerView<>(true)) { ... }

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include "Singleton.h"
#include "Layer.h"
#include "ECS/Entity.h"

namespace PE
{
    /*!***********************************************************************************
    \brief Assits in using the bitset as a key when used as a map key

    *************************************************************************************/
    struct Comparer2
    {
        /*!***********************************************************************************
         \brief Compares the bitsets lhs < rhs value

         \param[in] b1 bitset lhs
         \param[in] b2 bitset rhs
         \return true 	lhs < rhs
         \return false  rhs > lhs
        *************************************************************************************/
        bool operator() (const std::bitset<MAX_COMP>& b1, const std::bitset<MAX_COMP>& b2) const 
        {
            return b1.to_ulong() < b2.to_ulong();
        }
    };

	/*!***********************************************************************************
	 \brief 
	 
	*************************************************************************************/
    class LayerManager : public Singleton<LayerManager>
    {
    public:
        friend class Singleton<LayerManager>;
        
    public:
		/*!***********************************************************************************
		\brief Construct a new Layer Manager object
				Constructs the default ALL components into cached vectors as well.
		*************************************************************************************/
        LayerManager();
    public:
		/*!***********************************************************************************
		\brief 		Removes an entity from all cached vectors it was contained in
		
		\param[in] r_id 	The id of the entity to remove
		*************************************************************************************/
        void RemoveEntity(const EntityID& r_id);

		/*!***********************************************************************************
		 \brief 	Adds an entity to all currently cached vectors (if it meets/contains all 
		 			the components of the cache)
		 
		 \param[in] r_id 	The id of the entity to add
		*************************************************************************************/
        void AddEntity(const EntityID& r_id);

		/*!***********************************************************************************
		 \brief 	Updates the cached layers according to the state of the input entity
		 
		 \param[in] r_id 	The entity to update in the cached vectors
		*************************************************************************************/
        void UpdateEntity(const EntityID& r_id);

		/*!***********************************************************************************
		 \brief Get the Layers object
		 
		 \param[in,out] r_components 
		 \return const std::array<Layer, MAX_LAYERS>& 
		*************************************************************************************/
		const std::array<Layer, MAX_LAYERS>& GetLayers(const ComponentID& r_components);

		/*!***********************************************************************************
		 \brief Get the Layer State object
		 
		 \return const LayerState& 
		*************************************************************************************/
		const LayerState& GetLayerState() { return m_layerStates; }

		/*!***********************************************************************************
		 \brief Set the Layer State object to the input LayerState object
		 
		 \param[in] lyr 	Copy the layer object
		*************************************************************************************/
		void SetLayerState(const LayerState& r_lyr) { m_layerStates = r_lyr; }

		/*!***********************************************************************************
		 \brief Get the state of that specific layer
		 
		 \param[in] pos 	The position to get
		 \return true 		The layer is enabled
		 \return false 		The layer is disabled
		*************************************************************************************/
		bool GetLayerState(size_t pos) { return m_layerStates[pos]; }

		/*!***********************************************************************************
		 \brief Set the Layer State 
		 		
		 \param[in] pos 	The layer to affect
		 \param[in] set 	The state to set the layer to
		*************************************************************************************/
		void SetLayerState(size_t pos, bool set) { m_layerStates[pos] = set; }

		/*!***********************************************************************************
		 \brief Resets the layer caches (will skip entityID 0 as that is the UI camera)
		 
		*************************************************************************************/
		void ResetLayerCache();
    private:
		/*!***********************************************************************************
		 \brief Create a Cached object
		 
		 \param[in,out] r_components 
		*************************************************************************************/
        void CreateCached(const ComponentID& r_components);
    private:

    private:
		// map to contained cached interactions layers
        std::map<ComponentID, InteractionLayers, Comparer2> m_cachedLayers;

		// stores the states (enabled/disabled) 
        LayerState m_layerStates;
    };


	template<typename... ComponentTypes>
	struct LayerView
	{
		// ----- Public Variables ----- // 
		// ptr to the entity manager
		
		// the components for this scope
		ComponentID componentsCombined;
		// flag for toggling whether all components are in scope
		bool all{ false };
		bool ignore{ false };

		/*!***********************************************************************************
		\brief		This iterator struct is a required interface to allow the use of foreach
					loops.

		*************************************************************************************/
		struct Iterator
		{
			/*!***********************************************************************************
			\brief Construct a new Iterator object

			\param[in] index 		The index to start from
			\param[in] components 	The components to scope to
			\param[in] all 		Whether or not the scope is to all copmonents
			*************************************************************************************/
			Iterator(bool index, const ComponentID& r_components, bool all, bool ignore) :
				all(all), ignore{ignore}
			{
				if (index)
				{
					ite = (all) ? LayerManager::GetInstance().GetLayers(ALL).begin() : LayerManager::GetInstance().GetLayers(r_components).begin();
				}
				else
				{
					ite = (all) ? LayerManager::GetInstance().GetLayers(ALL).end() : LayerManager::GetInstance().GetLayers(r_components).end();
				}

				end = (all) ? LayerManager::GetInstance().GetLayers(ALL).end() : LayerManager::GetInstance().GetLayers(r_components).end();
				if (!ignore)
				{
					while (ite != end && !LayerManager::GetInstance().GetLayerState().test(layer))
					{
						++ite;
						++layer;
					}
				}
			}

			/*!***********************************************************************************
			\brief Returns the Entity ID at the current iterator position.

			\return EntityID ID of the entity
			*************************************************************************************/
			const Layer& operator* () const
			{
				return *ite;
			}

			/*!***********************************************************************************
			\brief operator== overload, checks if the two iterators are the same.

			\param[in] rhs the iterator to compare to this iterator
			\return true 	iterators are the same
			\return false 	iterators are different
			*************************************************************************************/
			bool operator== (const Iterator& r_rhs) const
			{
				return (ite == end) || (ite == r_rhs.ite);
			}

			/*!***********************************************************************************
			\brief operator!= overload, checks if the two iterators are different

			\param[in] rhs the iterators to comapre to this iterator
			\return true 	the iterators are different
			\return false  the iterators are the same
			*************************************************************************************/
			bool operator!= (const Iterator& r_rhs) const
			{
				return (ite != end) || (ite != r_rhs.ite);
			}


			/*!***********************************************************************************
			\brief operator++ overload, it increments the iterator until it reaches a valid
					entity, or it reaches the limits of the iterator

			\return Iterator& reference to this, emulating normal operator++ behaviour
			*************************************************************************************/
			Iterator& operator++()
			{
				do
				{
					++ite;
					++layer;
					if (ignore || (layer < MAX_LAYERS && LayerManager::GetInstance().GetLayerState().test(layer)))
						break;
				} while (ite != end);

				return *this;
			}

			// The stored vector of entities for this iterator
			std::array<Layer, MAX_LAYERS>::const_iterator ite;
			// end of the vector
			std::array<Layer, MAX_LAYERS>::const_iterator end;
			// flag for toggling whether all components are in scope
			bool all{ false };
			bool ignore{ false };
			size_t layer{ 0 };
		};

		// ----- Constructors ------ //

		/*!***********************************************************************************
		\brief Construct a new Scene View object

		*************************************************************************************/
		LayerView(bool ignore = false) : ignore{ignore}
		{
			// checks if the number of components is zero
			if constexpr (sizeof...(ComponentTypes))
			{
				componentsCombined = EntityManager::GetInstance().GetComponentIDs<ComponentTypes ... >();
			}
			else // if no components are provided, it assume the user wants to scope to all components
			{
				all = true;
			}
		}
		// ----- Public Methods ----- //


		/*!***********************************************************************************
		\brief Generates a begin iterator to the first entity that matches the scope

		\return const Iterator the iterator for the first "legal" entity
		*************************************************************************************/
		const Iterator begin() const	// cannot follow coding conventions due to c++ begin() & end() standards
		{
			return Iterator(true, componentsCombined, all, ignore);
		}

		/*!***********************************************************************************
		\brief Generates the last entity

		\return const Iterator the iterator for the last "legal" entity
		*************************************************************************************/
		const Iterator end() const		// cannot follow coding conventions due to c++ begin() & end() standards
		{
			return Iterator(false, componentsCombined, all, ignore);
		}
	};

	struct InternalView
	{
		// ----- Public Variables ----- // 


		/*!***********************************************************************************
		\brief		This iterator struct is a required interface to allow the use of foreach
					loops.

		*************************************************************************************/
		struct Iterator
		{
			/*!***********************************************************************************
			\brief Construct a new Iterator object

			\param[in] index 		The index to start from
			\param[in] components 	The components to scope to
			\param[in] all 		Whether or not the scope is to all copmonents
			*************************************************************************************/
			Iterator(std::vector<EntityID>::const_iterator beg, std::vector<EntityID>::const_iterator en, bool ig)
			{
				ite = beg;
				end = en;
				ignore = ig;
				try
				{ 
					if (!ignore)
						while (ite != end && (!EntityManager::GetInstance().Get<EntityDescriptor>(*ite).isActive || !EntityManager::GetInstance().Get<EntityDescriptor>(*ite).isAlive)) ++ite;
				}
				catch (...)
				{
					ite = end;
				}
				
			}

			/*!***********************************************************************************
			\brief Returns the Entity ID at the current iterator position.

			\return EntityID ID of the entity
			*************************************************************************************/
			const EntityID& operator* () const
			{
				return *ite;
			}

			/*!***********************************************************************************
			\brief operator== overload, checks if the two iterators are the same.

			\param[in] rhs the iterator to compare to this iterator
			\return true 	iterators are the same
			\return false 	iterators are different
			*************************************************************************************/
			bool operator== (const Iterator& r_rhs) const
			{
				return (ite == end) || (ite == r_rhs.ite);
			}

			/*!***********************************************************************************
			\brief operator!= overload, checks if the two iterators are different

			\param[in] rhs the iterators to comapre to this iterator
			\return true 	the iterators are different
			\return false  the iterators are the same
			*************************************************************************************/
			bool operator!= (const Iterator& r_rhs) const
			{
				return (ite != end) || (ite != r_rhs.ite);
			}


			/*!***********************************************************************************
			\brief operator++ overload, it increments the iterator until it reaches a valid
					entity, or it reaches the limits of the iterator

			\return Iterator& reference to this, emulating normal operator++ behaviour
			*************************************************************************************/
			Iterator& operator++()
			{
				do
				{
					++ite;
					if (ite != end && (ignore || (EntityManager::GetInstance().Get<EntityDescriptor>(*ite).isActive && EntityManager::GetInstance().Get<EntityDescriptor>(*ite).isAlive)))
						break;
				} while (ite != end);
				return *this;
			}

			// The stored vector of entities for this iterator
			std::vector<EntityID>::const_iterator ite;
			// end of the vector
			std::vector<EntityID>::const_iterator end;
			bool ignore;
		};

		// ----- Constructors ------ //

		/*!***********************************************************************************
		\brief Construct a new Scene View object

		*************************************************************************************/
		InternalView(const Layer& layer, bool ig = false)
		{
			startIte = layer.begin();
			endIte = layer.end();
			ignore = ig;
		}
		// ----- Public Methods ----- //


		/*!***********************************************************************************
		\brief Generates a begin iterator to the first entity that matches the scope

		\return const Iterator the iterator for the first "legal" entity
		*************************************************************************************/
		const Iterator begin() const	// cannot follow coding conventions due to c++ begin() & end() standards
		{
			return Iterator(startIte, endIte, ignore);
		}

		/*!***********************************************************************************
		\brief Generates the last entity

		\return const Iterator the iterator for the last "legal" entity
		*************************************************************************************/
		const Iterator end() const		// cannot follow coding conventions due to c++ begin() & end() standards
		{
			return Iterator(endIte, endIte, ignore);
		}

		std::vector<EntityID>::const_iterator startIte;
		// end of the vector
		std::vector<EntityID>::const_iterator endIte;
		bool ignore{false};
	};
}