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
#include "Singleton.h"
#include "Layer.h"

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
        const InteractionLayers& GetLayers(const ComponentID& r_components);
    private:
        void CreateCached(const ComponentID& r_components);
    private:

    private:
        std::map<ComponentID, InteractionLayers, Comparer2> m_cachedLayers;
        LayerState m_layerStates;
    };

}