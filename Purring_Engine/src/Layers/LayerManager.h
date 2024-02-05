/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     LayerManager.h
 \date     10-01-2024

 \author               FOONG Jun Wei
 \par      email:      f.junwei@digipen.edu

 \brief	   This file contains the implementation a collision layer system that provides
           the functionalities to control collision between layers.

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

    class LayerManager : public Singleton<LayerManager>
    {
    public:
        friend class Singleton<LayerManager>;

        class LayerView
        {};
        
    public:
        LayerManager();
    public:
        void RemoveEntity(const EntityID& r_id);
        void AddEntity(const EntityID& r_id);
        void UpdateEntity(const EntityID& r_id);
		const std::array<Layer, MAX_LAYERS>& GetLayers(const ComponentID& r_components);
		const LayerState& GetLayerState() { return m_layerStates; }
		void SetLayerState(const LayerState& lyr) { m_layerStates = lyr; }
		bool GetLayerState(size_t pos) { return m_layerStates[pos]; }
		void SetLayerState(size_t pos, bool set) { m_layerStates[pos] = set; }
		void ResetLayerCache();
    private:
        void CreateCached(const ComponentID& r_components);
    private:

    private:
        std::map<ComponentID, InteractionLayers, Comparer2> m_cachedLayers;
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
			Iterator(std::vector<EntityID>::const_iterator beg, std::vector<EntityID>::const_iterator en)
			{
				ite = beg;
				end = en;
				try
				{ 
					while (ite != end && !EntityManager::GetInstance().Get<EntityDescriptor>(*ite).isAlive) ++ite;
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
					if (ite != end && EntityManager::GetInstance().Get<EntityDescriptor>(*ite).isAlive)
						break;
				} while (ite != end);
				return *this;
			}

			// The stored vector of entities for this iterator
			std::vector<EntityID>::const_iterator ite;
			// end of the vector
			std::vector<EntityID>::const_iterator end;
			
		};

		// ----- Constructors ------ //

		/*!***********************************************************************************
		\brief Construct a new Scene View object

		*************************************************************************************/
		InternalView(const Layer& layer)
		{
			startIte = layer.begin();
			endIte = layer.end();
		}
		// ----- Public Methods ----- //


		/*!***********************************************************************************
		\brief Generates a begin iterator to the first entity that matches the scope

		\return const Iterator the iterator for the first "legal" entity
		*************************************************************************************/
		const Iterator begin() const	// cannot follow coding conventions due to c++ begin() & end() standards
		{
			return Iterator(startIte, endIte);
		}

		/*!***********************************************************************************
		\brief Generates the last entity

		\return const Iterator the iterator for the last "legal" entity
		*************************************************************************************/
		const Iterator end() const		// cannot follow coding conventions due to c++ begin() & end() standards
		{
			return Iterator(endIte, endIte);
		}

		std::vector<EntityID>::const_iterator startIte;
		// end of the vector
		std::vector<EntityID>::const_iterator endIte;
	};
}