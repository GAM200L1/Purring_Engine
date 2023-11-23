/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CollisionLayer.cpp
 \date     22-11-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief	   This file contains the implementation a collision layer system that provides
           the functionalities to control collision between layers.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "CollisionLayer.h"

namespace PE
{
    //-------------Collision Layer-----------------//
    CollisionLayer::CollisionLayer(unsigned collisionLayerIndex)
        : m_collisionLayerIndex{ collisionLayerIndex }, m_collisionLayerName{ "Layer " + std::to_string(collisionLayerIndex) }
	{
        m_collisionLayerSignature.set();
	}

    void CollisionLayer::FlipCollisionLayerBit(unsigned collisionLayerIndex)
    {
        m_collisionLayerSignature.flip(collisionLayerIndex);
    }

    void CollisionLayer::SetCollisionLayerName(std::string collisionLayerName)
    {
        m_collisionLayerName = collisionLayerName;
    }

    //-------------Collision Manager-----------------//
    CollisionLayerManager::CollisionLayerManager()
	{
        for (unsigned i{}; i < TOTAL_COLLISION_LAYERS; ++i)
        {
            m_collisionLayers[i] = std::make_shared<CollisionLayer>(i);
        }
	}


}