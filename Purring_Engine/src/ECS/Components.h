/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Components.h
 \date     03-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief     Contains the implementation of ComponentPool. As well as various test
            components.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

// INCLUDES
#include "prpch.h"

// CONSTANT VARIABLES
constexpr size_t DEFAULT_ENTITY_CNT = 16;		// default bytes allocated to components pool

namespace Engine
{

    struct ComponentPool
    {
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
            m_elementSize = elementsize;
            m_capacity = entcnt;
            p_data = new char[m_elementSize * m_capacity];
            if (!p_data)
                throw;
        }

        /*!***********************************************************************************
         \brief Destroy the Component Pool object
                Frees all dynamically allocated data, in this case, the pool itself

        *************************************************************************************/
        ~ComponentPool()
        {
            delete[] p_data;
        }

        /*!***********************************************************************************
         \brief Takes in param numEntity to be the new number of elements to support within
                the pool

         \param[in] numEntity   The number of entities to resize to
         \return true           Sucessflly resized
         \return false          Failed to resize
        *************************************************************************************/
        bool resize(size_t numEntity)
        {
            char* p_tmp = new char[numEntity * m_elementSize];
            // allocation failed!!
            if (!p_tmp)
            {
                // @TODO add log message, error, not enough memory
                return false;
            }
            std::swap(p_tmp, p_data);
            m_capacity = numEntity;
            delete[] p_tmp;
            return true;
        }

        /*!***********************************************************************************
         \brief Gets a void pointer to the specified entity's component in the pool

         \param[in] index   Entity's component to get
         \return void*      nullptr -> the requested  component does not exist in this pool
                            (this entity does not have this component)
                            Otherwise retursn the start of that entity's component in memory
        *************************************************************************************/
        inline void* get(size_t index)
        {
            return (!m_idxMap.count(index)) ? nullptr : (p_data + m_idxMap[index] * m_elementSize);
        }

        /*!***********************************************************************************
         \brief Gets a reference to the component in memory

         \tparam T              The component typing (the safety is handled by EntityManager)
         \param[in,out] index   The Entity's ID to search
         \return T&             The reference to the coponent in memory
        *************************************************************************************/
        template <typename T>
        T& get(size_t index)
        {
            return static_cast<T>(get(index));
        }

        /*!***********************************************************************************
         \brief Removes an entity from this pool

         \param[in] index   The entity id
        *************************************************************************************/
        void remove(size_t index)
        {
            if (!m_idxMap.count(index))
                throw; // log in the future
            m_idxMap.erase(index);
        }


        /*!***********************************************************************************
         \brief Returns whether the pool has this entity's component

         \param[in] id  The entity to find in this pool
         \return true   This pool contains this entity's component
         \return false  This pool does not contain this entity's component
        *************************************************************************************/
        bool HasEntity(size_t id) const
        {
            return (this) ? m_idxMap.count(id) : false;
        }

        char* p_data{ nullptr };
        std::map<size_t, size_t> m_idxMap;  // map to decouple the entity ID from the internal index
        std::queue<size_t> m_removed;
        size_t m_elementSize{};
        size_t m_size{};
        size_t m_capacity{};
    };

    class Component {}; // no implementation needed

    class ComponentCreator
    {
    public:
        virtual ~ComponentCreator() {};
        virtual const size_t& GetSize() const = 0;
    };


    template<typename type>
    class ComponentCreatorType : public ComponentCreator
    {
    public:
        ComponentCreatorType(size_t sz) : m_size(sz) { }
        virtual const size_t& GetSize() const { return m_size; }
    private:
        const size_t m_size;
    };
};

#define REGISTERCOMPONENT(type, size) Engine::g_entityFactory->AddComponentCreator( #type, new Engine::ComponentCreatorType<type>( size ) );

class test : public Engine::Component
{
public:
    float x, y;
};