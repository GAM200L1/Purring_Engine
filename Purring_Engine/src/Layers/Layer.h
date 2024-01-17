/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Layer.h
 \date     10-01-2024

 \author               FOONG Jun Wei
 \par      email:      f.junwei@digipen.edu

 \brief	   This file contains the implementation a collision layer system that provides
           the functionalities to control collision between layers.

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include "ECS/Entity.h"
constexpr size_t MAX_LAYERS = 10;
namespace PE
{
    using Layer = std::vector<EntityID>;
    using LayerState = std::bitset<MAX_LAYERS>;

    class InteractionLayers
    {
    public:

    public:
        InteractionLayers();
        InteractionLayers(const std::vector<EntityID>& r_eid);
        operator const std::array<Layer, MAX_LAYERS>& () const { return layers; }
        const std::array<Layer, MAX_LAYERS>& GetLayers() const { return layers; }
    public:
        void UpdateLayers(const EntityID& r_id, bool add = true);
        void UpdateLayers(const EntityID& r_id, const int& r_newLayer);
        
    private:

    private:

    private:
        //LayerState layerState;
        std::array<Layer, MAX_LAYERS> layers;
    };
}