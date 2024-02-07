/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Layer.h
 \date     10-01-2024

 \author               FOONG Jun Wei
 \par      email:      f.junwei@digipen.edu

 \brief	   This file contains the implementation of a layer.

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

constexpr size_t MAX_LAYERS = 11;
constexpr unsigned MAX_COMP = 32;

// Typedefs
typedef unsigned long long EntityID;								// typedef for storing the unique ID of the entity, same as size_t
typedef std::bitset<MAX_COMP> ComponentID;					// ComponentID type, internally it is a bitset



namespace PE
{
    using Layer = std::vector<EntityID>;
    using LayerState = std::bitset<MAX_LAYERS>;

    class InteractionLayers
    {
    public:
        
    public:
        /*!***********************************************************************************
         \brief Default Construct a new Interaction Layers object
         
        *************************************************************************************/
        InteractionLayers();

        /*!***********************************************************************************
         \brief Conversion construct a new Interaction Layers object from a vector of entities
         
         \param[in] r_eid  
        *************************************************************************************/
        InteractionLayers(const std::vector<EntityID>& r_eid);

        /*!***********************************************************************************
         \brief Operator () overload for returning the layers object
         
         \return const std::array<Layer, MAX_LAYERS>& 
        *************************************************************************************/
        operator const std::array<Layer, MAX_LAYERS>& () const { return layers; }

        /*!***********************************************************************************
         \brief Get the Layers object
         
         \return const std::array<Layer, MAX_LAYERS>& 
        *************************************************************************************/
        const std::array<Layer, MAX_LAYERS>& GetLayers() const { return layers; }

        /*!***********************************************************************************
         \brief clears the layers 
         
        *************************************************************************************/
        void Clear() { for (auto& l : layers) { l.clear(); } }
    public:

        /*!***********************************************************************************
        \brief Updates the entity in this layers object
        
        \param[in] r_id Entity to affect
        \param[in] add  Flag to use the add logic or the remove logic (defaults to true)
        *************************************************************************************/
        void UpdateLayers(const EntityID& r_id, bool add = true);

        /*!***********************************************************************************
         \brief Updates the entity in this layers object
         
         \param[in] r_id        Entity to affect
         \param[in] r_newLayer  Layer to add this entity to
        *************************************************************************************/
        void UpdateLayers(const EntityID& r_id, const int& r_newLayer);
        
    private:
        std::array<Layer, MAX_LAYERS> layers;
    };
}