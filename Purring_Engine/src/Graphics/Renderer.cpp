#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Renderer.cpp
 \date     20-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the Renderer class, which acts as a base 
            component to add to gameobjects to be rendered.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "Renderer.h"

namespace PE
{
    namespace Graphics
    {
        void Renderer::SetColor(glm::vec4 const& newColor)
        {
            m_color.r = glm::clamp(newColor.r, 0.f, 1.f);
            m_color.g = glm::clamp(newColor.g, 0.f, 1.f);
            m_color.b = glm::clamp(newColor.b, 0.f, 1.f);
            m_color.a = glm::clamp(newColor.a, 0.f, 1.f);
        }


        void Renderer::SetColor(float const r, float const g, float const b, float const a)
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
    } // End of Graphics namespace
} // End of PE namespace