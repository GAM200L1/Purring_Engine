#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Renderer.h
 \date     30-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the Renderer class, which acts as a base 
            component to add to gameobjects to be rendered.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "Graphics/GLHeaders.h"

#include <glm/glm.hpp>  // glm::vec4, glm::clamp


#include "Texture.h"
#include "Math/Transform.h"
#include "Data/json.hpp"

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
         \brief Types of meshes that can be rendered. The value of the enum corresponds 
                to the index of a mesh object stored in r_meshes in the renderer manager.         
        *************************************************************************************/
        enum class EnumMeshType : unsigned char
        {
            QUAD,
            TEXTURED_QUAD,
            DEBUG_SQUARE,
            DEBUG_CIRCLE,
            DEBUG_LINE,
            DEBUG_POINT,
            MESH_COUNT,
            NONE
        };

        /*!***********************************************************************************
         \brief Component to attach to gameobjects to be drawn. This components contains 
                information about the color and texture of the object to be rendered.         
        *************************************************************************************/
        class Renderer
        {
            // ----- Public functions ----- //
        public:
            /*!***********************************************************************************
            \brief Get the RGBA color values of the object as a glm::vec4 object.
            
            \return glm::vec4 const& - RGBA values of the color of the object.
            *************************************************************************************/
            inline glm::vec4 const& GetColor() const { return m_color; }

            /*!***********************************************************************************
             \brief Get the type of mesh to use to render this object.
              
             \return Graphics::EnumMeshType The type of mesh to use to render this object.
            *************************************************************************************/
            inline Graphics::EnumMeshType GetMeshType() const { return m_meshType; }

            /*!***********************************************************************************
             \brief Gets whether the renderer is enabled. 
             
             \return true - The renderer is enabled and this object will be displayed.
             \return false - The renderer is disabled and this object will not be displayed.
            *************************************************************************************/
            inline bool GetEnabled() const { return m_enabled; }


            /*!***********************************************************************************
             \brief Gets the texture key string. If the object does not have texture on it, the
                    key should be empty ("").
             
             \return std::string const& 
            *************************************************************************************/
            inline std::string const& GetTextureKey() const { return m_textureKey; }

            /*!***********************************************************************************
             \brief Gets the minimum uv coordinates of the renderer component

             \return glm::vec2 const& Min uv coordinates
            *************************************************************************************/
            inline glm::vec2 const& GetUVCoordinatesMin() const { return m_minUV; }

            /*!***********************************************************************************
             \brief Gets the maximum uv coordinates of the renderer component

             \return glm::vec2 const& Max uv coordinates
            *************************************************************************************/
            inline glm::vec2 const& GetUVCoordinatesMax() const { return m_maxUV;  }


            /*!***********************************************************************************
             \brief Sets the RGBA color of the object. If the object has a texture on it, 
                    this tints the color of the texture.
             
             \param[in] r Red component of the color to set the object to (from [0, 1]).
             \param[in] g Green component of the color to set the object to (from [0, 1]).
             \param[in] b Blue component of the color to set the object to (from [0, 1]).
             \param[in] a Alpha component of the color to set the object to (from [0, 1]).
            *************************************************************************************/
            void Renderer::SetColor(float const r = 1.f, float const g = 1.f, float const b = 1.f, float const a = 1.f);
            
            /*!***********************************************************************************
             \brief Sets the alpha of the color of the object. If the object has a texture on it, 
                    this sets the opacity of the texture.

             \param[in] alpha Alpha component of the color to set the object to (from [0, 1]).
            *************************************************************************************/
            void Renderer::SetAlpha(float const alpha);

            /*!***********************************************************************************
             \brief Sets whether the renderer is enabled. If set to true, this object will be 
                    visible in the scene.
             
             \param[in] newEnabled Whether the renderer should be enabled.
            *************************************************************************************/
            void Renderer::SetEnabled(bool const newEnabled);

            /*!***********************************************************************************
             \brief Set the texture key associated with this object. If the key passed in does 
                    not exist, the texture key will be cleared.
             
             \param[in] r_newKey Texture key to set the texture key of this renderer to.
            *************************************************************************************/
            void Renderer::SetTextureKey(std::string const& r_newKey);

            /*!***********************************************************************************
             \brief Set Minimum UV coordinates of the renderer texture.

             \param[in] maxUV Minimum UV coordinates to set m_minUV to
            *************************************************************************************/
            void Renderer::SetUVCoordinatesMin(vec2 const& minUV);

            /*!***********************************************************************************
             \brief Set Maximum UV coordinates of the renderer texture.

             \param[in] maxUV Maximum UV coordinates to set m_maxUV to
            *************************************************************************************/
            void Renderer::SetUVCoordinatesMax(vec2 const& maxUV);

            /*!***********************************************************************************
             \brief Serializes the data attached to this renderer.
            *************************************************************************************/
            nlohmann::json ToJson(size_t id) const;

            /*!***********************************************************************************
             \brief Deserializes data from a JSON file and loads it as values to set this 
                    component to.

             \param[in] j JSON object containing the values to load into the renderer component.
            *************************************************************************************/
            static Renderer FromJson(const nlohmann::json& j);

        protected:
            bool m_enabled{ true }; // Set to true to render the object, false not to.
            glm::vec4 m_color{ 0.5f, 0.5f, 0.5f, 0.5f }; // RGBA values of a color in a range of 0 to 1.
            Graphics::EnumMeshType m_meshType{ EnumMeshType::QUAD }; // Type of mesh. 
            std::string m_textureKey{ "" }; // Key for the corresponding texture in the resource manager.
            glm::vec2 m_minUV{ 0.f, 0.f };
            glm::vec2 m_maxUV{ 1.f, 1.f };
        };
    } // End of Graphics namespace
} // End of PE namespace