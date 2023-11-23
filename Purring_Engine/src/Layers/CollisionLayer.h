/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CollisionLayer.h
 \date     22-11-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief	   This file contains the implementation a collision layer system that provides
           the functionalities to control collision between layers.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include "Singleton.h"
#include <bitset>
#include  <string>

namespace PE
{
    constexpr unsigned TOTAL_COLLISION_LAYERS = 10;

    class CollisionLayer
    {
    public:
        CollisionLayer(unsigned collisionLayerIndex);

        inline std::bitset<TOTAL_COLLISION_LAYERS> GetCollisionLayerSignature() { return m_collisionLayerIndex; }

        inline bool IsCollidingWith(unsigned collisionLayerIndex) { return m_collisionLayerSignature[collisionLayerIndex]; }

        void FlipCollisionLayerBit(unsigned collisionLayerIndex);

        inline unsigned GetCollisionLayerIndex() { return m_collisionLayerIndex; }

        inline std::string GetCollisionLayerName() { return m_collisionLayerName; }

        void SetCollisionLayerName(std::string collisionLayerName);

        // serialize layer

        // deserialize layer

        unsigned m_collisionLayerIndex;
        std::string m_collisionLayerName;
        std::bitset <TOTAL_COLLISION_LAYERS> m_collisionLayerSignature;
    };

    class CollisionLayerManager : public Singleton<CollisionLayerManager>
    {
    public:
        friend class Singleton<CollisionLayerManager>;

        inline std::shared_ptr<CollisionLayer> GetCollisionLayer(unsigned collisionLayerIndex) { return m_collisionLayers[collisionLayerIndex]; }

        // serialize for each scene

        // deserialize for each scene
        
    private:
        // private constructor for singleton
        CollisionLayerManager();

        std::map<unsigned, std::shared_ptr<CollisionLayer>> m_collisionLayers;
    };
}