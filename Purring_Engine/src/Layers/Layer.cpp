/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Layer.cpp
 \date     10-01-2024

 \author               FOONG Jun Wei
 \par      email:      f.junwei@digipen.edu

 \brief	   This file contains the implementation a collision layer system that provides
           the functionalities to control collision between layers.

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "Layer.h"
#include "Logging/Logger.h"
extern Logger engine_logger;

namespace PE
{



    InteractionLayers::InteractionLayers(const std::initializer_list<EntityID>& r_eid)
    {
        layerState.flip();
        for (const auto& id : r_eid)
        {
            const EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(id);
            if (desc.interactionLayer >= MAX_LAYERS)
            {
                engine_logger.AddLog(true, "Entity had interaction layer larger than the limit!!", __FUNCTION__);
                throw;
            }
            layers[desc.interactionLayer].emplace_back(id);
        }
    }

    void InteractionLayers::UpdateLayers(const EntityID& r_id, bool add)
    {
        const EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(r_id);
        if (desc.interactionLayer >= MAX_LAYERS)
        {
            engine_logger.AddLog(true, "Entity had interaction layer larger than the limit!!", __FUNCTION__);
            throw;
        }
        if (add)
        { 
            if (layers[desc.interactionLayer].end() == std::find(layers[desc.interactionLayer].begin(), layers[desc.interactionLayer].end(), r_id))
            {
                layers[desc.interactionLayer].emplace_back(r_id);
            }
        }
        else
        {
            layers[desc.interactionLayer].erase(std::find(layers[desc.interactionLayer].begin(), layers[desc.interactionLayer].end(), r_id));
        }
    }

    void InteractionLayers::UpdateLayers(const EntityID& r_id, const int& r_newLayer)
    {
        EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(r_id);
        // if the new target layer is different, do the change
        if (desc.interactionLayer != r_newLayer)
        {
            // throw an error if the target new layer is larger than the limit
            if (r_newLayer >= MAX_LAYERS)
            {
                engine_logger.AddLog(true, "New layer larger than the limit!!", __FUNCTION__);
                throw;
            }
            //remove from old layer
            layers[desc.interactionLayer].erase(std::find(layers[desc.interactionLayer].begin(), layers[desc.interactionLayer].end(), r_id));
            
            // add to new target layer
            layers[r_newLayer].emplace_back(r_id);

            // set the layer to the new value
            desc.interactionLayer = r_newLayer;
        }
    }

}