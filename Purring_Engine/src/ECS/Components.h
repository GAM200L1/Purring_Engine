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
#include "prpch.h"
#include "Math/MathCustom.h"


// CONSTANT VARIABLES
constexpr size_t DEFAULT_ENTITY_CNT = 16;		// default bytes allocated to components pool

namespace PE
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

         \param[in] index   Entity's component to Get
         \return void*      nullptr -> the requested  component does not exist in this pool
                            (this entity does not have this component)
                            Otherwise retursn the start of that entity's component in memory
        *************************************************************************************/
        inline void* Get(size_t index)
        {
            return (!m_idxMap.count(index)) ? nullptr : (p_data + m_idxMap[index] * m_elementSize);
        }

        /*!***********************************************************************************
         \brief Gets a reference to the component in memory

         \tparam T              The component typing (the safety is handled by EntityManager)
         \param[in] index       The Entity's ID to search
         \return T&             The reference to the coponent in memory
        *************************************************************************************/
        template <typename T>
        T& Get(size_t index)
        {
            return static_cast<T>(Get(index));
        }

        /*!***********************************************************************************
         \brief Gets a reference to the component in memory (const) 

         \tparam T              The component typing (the safety is handled by EntityManager)
         \param[in] index       The Entity's ID to search
         \return const T&       The reference to the coponent in memory
        *************************************************************************************/
        template <typename T>
        const T& Get(size_t index) const
        {
            return static_cast<T>(Get(index));
        }

        /*!***********************************************************************************
         \brief Removes an entity from this pool

         \param[in] index   The entity id
        *************************************************************************************/
        void remove(size_t index)
        {
            if (!m_idxMap.count(index))
                throw; // log in the future
            memset(Get(index), 0, m_elementSize);
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
        std::queue<size_t> m_removed;       // keep track of removed indexes
        size_t m_elementSize{};             // the size of each element in the pool
        size_t m_size{};                    // the current size of the pool (entity count, should lineup to m_idxMap)
        size_t m_capacity{};                // the actual capacity of the pool
    };

    class Component {}; // no implementation needed

    class ComponentCreator
    {
    // ----- Public Methods -----//
    public:
        virtual const size_t& GetSize() const = 0;
    };


    template<typename type>
    class ComponentCreatorType : public ComponentCreator
    {
    // ----- Constructors -----//
    public:
        ComponentCreatorType(size_t sz) : m_size(sz) { }
    
    // ----- Public Methods -----//
    public:
        /*!***********************************************************************************
         \brief Get the Size of the component

         \return const size_t&  The size of the component
        *************************************************************************************/
        virtual const size_t& GetSize() const { return m_size; }

    // ----- Private Variables -----//
    private:
        const size_t m_size;    // stores the size of type
    };
};

#define REGISTERCOMPONENT(type, size) PE::g_entityFactory->AddComponentCreator( #type, new PE::ComponentCreatorType<type>( size ) );

class test : public PE::Component
{
public:
    float x, y;
};

namespace PE
{
    struct Transform : public Component
    {
        vec2 scale;
        float angle; // in radians
        vec2 position;

        mat3x3 GetMat3x3() const
        {
            mat3x3 scaleMat;
            scaleMat.Scale(scale.x, scale.y);
            mat3x3 rotMat;
            rotMat.RotateRad(angle);
            mat3x3 transMat;
            transMat.Translate(position.x, position.y);

            return transMat * rotMat * scaleMat;

        }
        mat4x4 GetMat4x4() const
        {
            return GetMat3x3().ConvertTo4x4();
        }
    };
}