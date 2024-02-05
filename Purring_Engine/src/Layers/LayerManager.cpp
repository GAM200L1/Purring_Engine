/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     LayerManager.cpp
 \date     10-01-2024

 \author               FOONG Jun Wei
 \par      email:      f.junwei@digipen.edu

 \brief	   This file contains the implementation a collision layer system that provides
           the functionalities to control collision between layers.

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "LayerManager.h"
#include "ECS/SceneView.h"

namespace PE
{
    LayerManager::LayerManager()
    {
        m_layerStates.flip();
        for (const auto& id : SceneView<>())
        {
            //const EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(id);
            m_cachedLayers[ALL].UpdateLayers(id);
        }
        // m_layerStates.set(1, false);
    }

    void LayerManager::RemoveEntity(const EntityID& r_id)
    {
        //const EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(r_id);
        for (auto &[k, layer] : m_cachedLayers)
        {
            layer.UpdateLayers(r_id, false);
        }
    }

    void LayerManager::AddEntity(const EntityID& r_id)
    {
        if (!EntityManager::GetInstance().IsEntityValid(r_id))
            return;
        //const EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(r_id);
        ComponentID components;
        for (auto comp : EntityManager::GetInstance().GetComponentIDs(r_id))
        {
            components |= comp;
        }
       
        for (auto &[k, layer] : m_cachedLayers)
        {
            if (k == ALL  || (k & components).count() == k.count())
            {
                layer.UpdateLayers(r_id);
            }
        }
    }

    void LayerManager::UpdateEntity(const EntityID& r_id)
    {
        RemoveEntity(r_id);
        AddEntity(r_id);
    }

    const std::array<Layer, MAX_LAYERS>& LayerManager::GetLayers(const ComponentID& r_components)
    {
        if (!m_cachedLayers.count(r_components))
            CreateCached(r_components);
       
        return m_cachedLayers.at(r_components).GetLayers();
    }

    void LayerManager::ResetLayerCache()
    {
        for(auto&[k,l] : m_cachedLayers)
        {
            l.Clear();
        }
    }

    void LayerManager::CreateCached(const ComponentID& r_components)
    {
        m_cachedLayers[r_components] = InteractionLayers(EntityManager::GetInstance().GetEntitiesInPool(r_components));
    }
}