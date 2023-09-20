#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Renderer.h
 \date     20-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the Renderer class, which acts as a base 
            component to add to gameobjects to be rendered.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "Graphics/GLHeaders.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "ECS/Components.h"
#include "VertexData.h"
#include "Texture.h"
#include "Math/Transform.h"

namespace PE
{
    namespace Graphics
    {
        //! Types of primitives 
        enum class EnumMeshType : unsigned char
        {
            QUAD,
            TRIANGLE,
            DEBUG_SQUARE,
            DEBUG_CIRCLE,
            DEBUG_LINE
        };

        // Renderer component. Attach one to each gameobject to be drawn.
        class Renderer : public Component
        {
            // ----- Public functions ----- //
        public:
            inline glm::vec4 const& GetColor() const { return m_color; }
            inline Graphics::EnumMeshType GetMeshType() const { return m_meshType; }
            inline bool GetEnabled() const { return m_enabled; }
            inline std::string const& GetTextureKey() const { return m_textureKey; }

            void Renderer::SetColor(glm::vec4 const& newColor)
            {
                m_color.r = glm::clamp(newColor.r, 0.f, 1.f);
                m_color.g = glm::clamp(newColor.g, 0.f, 1.f);
                m_color.b = glm::clamp(newColor.b, 0.f, 1.f);
                m_color.a = glm::clamp(newColor.a, 0.f, 1.f);
            }


            void Renderer::SetColor(float const r = 1.f, float const g = 1.f, float const b = 1.f, float const a = 1.f)
            {
                m_color.r = glm::clamp(r, 0.f, 1.f);
                m_color.g = glm::clamp(g, 0.f, 1.f);
                m_color.b = glm::clamp(b, 0.f, 1.f);
                m_color.a = glm::clamp(a, 0.f, 1.f);
            }


            void Renderer::SetEnabled(bool const newEnabled)
            {
                m_enabled = newEnabled;
            }


            void Renderer::SetTextureKey(std::string const& r_newKey)
            {
                m_textureKey = r_newKey;
            }

        private:
            bool m_enabled{ true }; // Set to true to render the object, false not to.
            glm::vec4 m_color{ 1.f, 0.f, 0.f, 0.5f }; // RGBA values of a color in a range of 0 to 1
            Graphics::EnumMeshType m_meshType{ EnumMeshType::QUAD }; // Type of mesh
            std::string m_textureKey{ "" }; // Key for the corresponding texture in the resource manager
            // obj factory will set a hidden layer value
        };
    } // End of Graphics namespace
} // End of PE namespace