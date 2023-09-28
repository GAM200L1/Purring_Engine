/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Components.h
 \date     03-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief     Contains the implementation of ComponentPool. 
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

// INCLUDES
#include <set>
#include <queue>
#include "Math/MathCustom.h"
// CONSTANT VARIABLES
constexpr size_t DEFAULT_ENTITY_CNT = 3000;		// default bytes allocated to components pool

namespace PE
{
    struct ComponentPool
    {
        // ----- Public Variables ----- //
        std::map<size_t, size_t> idxMap;  // map to decouple the entity ID from the internal index
        size_t elementSize{};             // the size of each element in the pool
        size_t size{};                    // the current size of the pool (entity count, should lineup to idxMap)
        size_t capacity{};                // the actual capacity of the pool

        // ----- Constructors ----- //
        /*!***********************************************************************************
         \brief Construct a new Component Pool object
                The size of the buffer allocated will be (elementsize * entcnt) bytes large
                e.g. element size == 24bytes, 16 entities
                24 * 16 = 384 bytes will be requested

         \param[in] elementsize     The size of the component
         \param[in] entcnt          The number of entities to allocate the buffer for(def = 16)
        *************************************************************************************/
        ComponentPool(size_t elementsize, size_t entcnt = DEFAULT_ENTITY_CNT)
        {
            elementSize = elementsize;
            capacity = entcnt;            
        }

        /*!***********************************************************************************
         \brief Destroy the Component Pool object
         
        *************************************************************************************/
        virtual ~ComponentPool() { };

        // ----- Public Getters ----- //
        /*!***********************************************************************************
         \brief Gets a void pointer to the specified entity's component in the pool

         \param[in] index   Entity's component to Get
         \return void*      nullptr -> the requested  component does not exist in this pool
                            (this entity does not have this component)
                            Otherwise retursn the start of that entity's component in memory
        *************************************************************************************/
        virtual void* Get(size_t index) = 0;

        // ----- Public Methods ----- //
        /*!***********************************************************************************
         \brief Takes in param numEntity to be the new number of elements to support within
                the pool

         \param[in] numEntity   The number of entities to resize to
         \return true           Sucessflly resized
         \return false          Failed to resize
        *************************************************************************************/
        virtual bool Resize(size_t numEntity) = 0;

        /*!***********************************************************************************
         \brief Removes an entity from this pool

         \param[in] index   The entity id
        *************************************************************************************/
        virtual void Remove(size_t index) = 0;


        /*!***********************************************************************************
         \brief Returns whether the pool has this entity's component

         \param[in] id  The entity to find in this pool
         \return true   This pool contains this entity's component
         \return false  This pool does not contain this entity's component
        *************************************************************************************/
        bool HasEntity(size_t id) const
        {
            return (this) ? idxMap.count(id) : false;
        }
    };

    /*!***********************************************************************************
     \brief Child class of component, specifies the behavior for individual pool types.
     
     \tparam T The type for the pool
    *************************************************************************************/
    template <typename T>
    class PoolData : public ComponentPool
    {
    // ----- Constructors ----- // 
    public:
        /*!***********************************************************************************
        \brief Construct a new Pool Data object
        
        \param[in] entcnt   The number of entities to create this pool for
        *************************************************************************************/
        PoolData(size_t entcnt = DEFAULT_ENTITY_CNT) : ComponentPool(sizeof(T), entcnt)
        {
            p_data = new T[entcnt];
        }

        /*!***********************************************************************************
         \brief Destroy the Pool Data object
         
        *************************************************************************************/
        ~PoolData()
        {
            delete[] p_data;
        }

    // ----- Public getters ----- // 
    public:
        /*!***********************************************************************************
         \brief Gets a void pointer to the specified entity's component in the pool 
         \param[in] index   Entity's component to Get
         \return void*      nullptr -> the requested  component does not exist in this pool
                            (this entity does not have this component)
                            Otherwise retursn the start of that entity's component in memory
        *************************************************************************************/
        void* Get(size_t index)
        {
            return reinterpret_cast<void*>(&(p_data[idxMap[index]]));
        }
        
    // ----- Public methods ----- // 
    public:
        /*!***********************************************************************************
         \brief Resizes the current pool (called by assign functions in EntityManager), when
                the pool is no longer large enough.
         
         \param[in] numEntity   The size to resize to
         \return true           Successfully resized
         \return false          Failed to resize
        *************************************************************************************/
        bool Resize(size_t numEntity)
        {
            T* p_tmp = new T[numEntity]();
            // allocation failed!!
            if (!p_tmp)
            {
                // @TODO add log message, error, not enough memory
                return false;
            }
            for (size_t i{}; i < capacity; ++i)
            {
                p_tmp[i] = p_data[i];
            }
            std::swap(p_tmp, p_data);
            capacity = numEntity;
            delete[] p_tmp;
            return true;
        }

        /*!***********************************************************************************
         \brief Removes an entity from this component pool
         
         \param[in] index   The entity to remove
        *************************************************************************************/
        void Remove(size_t index)
        {
            if (!idxMap.count(index))
                throw; // log in the future
            
            EntityID key{}, lastIdx{};
            for (const auto& pair : idxMap)
            {
                if (lastIdx <= pair.second)
                {
                    lastIdx = pair.second;
                    key = pair.first;
                }
            }
            // if index is not the last as well...
            if (key != index)
            {
                T tmp = p_data[lastIdx];
                p_data[idxMap[index]] = p_data[lastIdx];
                idxMap[key] = idxMap[index];
            }
            p_data[lastIdx] = T();
            idxMap.erase(index);
            --size;
        }
    // ----- Private Variables ----- //
    private:
        T* p_data{ nullptr };
    };
};

/*!***********************************************************************************
 \brief Registers a component to the component pools
 
*************************************************************************************/
#define REGISTERCOMPONENT(type) EntityFactory::GetInstance().AddComponentCreator<type>( #type, sizeof(type)  );
