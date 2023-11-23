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
        /*!***********************************************************************************
         \brief Constructor for Collision Layer

         \param[in] collisionLayerIndex the index of the collision layer.
        *************************************************************************************/
        CollisionLayer(unsigned collisionLayerIndex);

        /*!***********************************************************************************
         \brief Gets the collision layer signature of the layer.

         \return bitset containing the collision signature of the layer.
        *************************************************************************************/
        inline std::bitset<TOTAL_COLLISION_LAYERS>& GetCollisionLayerSignature() { return m_collisionLayerSignature; }

        /*!***********************************************************************************
         \brief Gets the bit of an index in the collision layer signature.

         \param[in] collisionLayerIndex Layer index to check bit from.

         \return bit of the specified index in the collision layer signature.
        *************************************************************************************/
        inline bool IsCollidingWith(unsigned collisionLayerIndex) { return m_collisionLayerSignature[collisionLayerIndex]; }

        /*!***********************************************************************************
         \brief Flips the bit of an index in the collision layer signature.

         \param[in] collisionLayerIndex Layer index to flip bit from.
        *************************************************************************************/
        void FlipCollisionLayerBit(unsigned collisionLayerIndex);

        /*!***********************************************************************************
         \brief Gets the collision layer index of the layer.

         \return index of the collision layer.
        *************************************************************************************/
        inline unsigned GetCollisionLayerIndex() { return m_collisionLayerIndex; }

        /*!***********************************************************************************
         \brief Gets the name of the collision layer.

         \return name of the collision layer.
        *************************************************************************************/
        inline std::string const& GetCollisionLayerName() { return m_collisionLayerName; }

        /*!***********************************************************************************
         \brief Sets the name of the collision layer.

         \param[in] collisionLayerName Name of the collision layer to set.
        *************************************************************************************/
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

        /*!***********************************************************************************
         \brief Gets the collision layer with the specified index

         \param[in] collisionLayerIndex Layer index to retrieve from the manager.

         \return shared_ptr to Collision layer with the specified index.
        *************************************************************************************/
        inline std::shared_ptr<CollisionLayer> GetCollisionLayer(unsigned collisionLayerIndex) { return m_collisionLayers[collisionLayerIndex]; }

        /*!***********************************************************************************
         \brief Gets the collision layers container from the manager.

         \return a reference to the Collision layers container.
        *************************************************************************************/
        std::vector<std::shared_ptr<CollisionLayer>>& GetCollisionLayers() { return m_collisionLayers; }

        // serialize for each scene

        // deserialize for each scene
        
    private:
        // private constructor for singleton
        CollisionLayerManager();

        std::vector<std::shared_ptr<CollisionLayer>> m_collisionLayers;
    };
}