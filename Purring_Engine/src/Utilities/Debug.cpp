/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Debug.cpp
 \date:    21-12-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief    This file contains the NewRendererManager class, which manages the renderers being used

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"

#include "Debug.h"

#include "System.h"

// Graphics
#include "Graphics/NewRendererManager.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/Renderer.h" // EnumMeshType
#include "Graphics/GraphicsUtilities.h" 

namespace PE
{    
    namespace Debug
    {
#ifndef TEST_BATCH_RENDERER // original renderer

        // if using the original renderer, don't define these functions

        void DrawPoint(vec2 const&, vec4 const&) {}

        void DrawLine(vec2 const&, vec2 const&, vec4 const&) {}

        void DrawRay(vec2 const&, vec2 const&, vec4 const&) {}

        void DrawRectangle(vec2 const&, float const,
            float const, float const, vec4 const&) {}

        void DrawAABB(vec2 const&, vec2 const&, vec4 const&) {}

        void DrawCircle(vec2 const&, float const, vec4 const&) {}

        void DrawCross(vec2 const&, float const, float const, vec4 const&) {}

#else

        void DrawPoint(vec2 const& r_position, vec4 const& r_color)
        {
            const_cast<Graphics::NewRendererManager*>(GETRENDERERMANAGER())->AddDebugShape(
                Graphics::RenderData{ Graphics::EnumMeshType::DEBUG_POINT, 
                    0.f, 0.f, 0.f,                                          // width, height, orientation
                    glm::vec2{r_position.x, r_position.y},                  // convert the position
                    glm::vec4{r_color.x, r_color.y, r_color.z, r_color.w} } // convert the color
            );
        }


        void DrawLine(vec2 const& r_from, vec2 const& r_to, vec4 const& r_color)
        {
            // compute vector
            vec2 peVector{r_to - r_from};

            // position is the midpoint
            vec2 midpoint{ (r_from + r_to) * 0.5f };

            const_cast<Graphics::NewRendererManager*>(GETRENDERERMANAGER())->AddDebugShape(
                Graphics::RenderData{ Graphics::EnumMeshType::DEBUG_LINE,
                    peVector.Length(), 0.f,                                 // width, height
                    Graphics::ComputeOrientation(peVector.x, peVector.y),   // orientation
                    glm::vec2{midpoint.x, midpoint.y},                      // convert the position
                    glm::vec4{r_color.x, r_color.y, r_color.z, r_color.w} } // convert the color
            );
        }


        void DrawRay(vec2 const& r_from, vec2 const& r_direction, vec4 const& r_color)
        {
            // position is the midpoint
            vec2 midpoint{ r_from + (r_direction * 0.5f) };

            const_cast<Graphics::NewRendererManager*>(GETRENDERERMANAGER())->AddDebugShape(
                Graphics::RenderData{ Graphics::EnumMeshType::DEBUG_LINE,
                    r_direction.Length(), 0.f,                                  // width, height
                    Graphics::ComputeOrientation(r_direction.x, r_direction.y), // orientation
                    glm::vec2{midpoint.x, midpoint.y},                      // convert the position
                    glm::vec4{r_color.x, r_color.y, r_color.z, r_color.w} } // convert the color
            );
        }


        void DrawRectangle(vec2 const& r_center, float const orientation, float const width, float const height, vec4 const& r_color)
        {
            const_cast<Graphics::NewRendererManager*>(GETRENDERERMANAGER())->AddDebugShape(
                Graphics::RenderData{ Graphics::EnumMeshType::DEBUG_SQUARE,
                    width, height, orientation, // width, height, orientation
                    glm::vec2{r_center.x, r_center.y},                      // convert the position
                    glm::vec4{r_color.x, r_color.y, r_color.z, r_color.w} } // convert the color
            );
        }


        void DrawAABB(vec2 const& r_max, vec2 const& r_min, vec4 const& r_color)
        {
            vec2 midpoint{ (r_max + r_min) * 0.5f };

            const_cast<Graphics::NewRendererManager*>(GETRENDERERMANAGER())->AddDebugShape(
                Graphics::RenderData{ Graphics::EnumMeshType::DEBUG_SQUARE,
                    r_max.x - r_min.x, r_max.y - r_min.y, 0.f, // width, height, orientation
                    glm::vec2{midpoint.x, midpoint.y},                      // convert the position
                    glm::vec4{r_color.x, r_color.y, r_color.z, r_color.w} } // convert the color
            );
        }


        void DrawCircle(vec2 const& r_center, float const radius, vec4 const& r_color)
        {
            float diameter{ radius * 2.f };

            const_cast<Graphics::NewRendererManager*>(GETRENDERERMANAGER())->AddDebugShape(
                Graphics::RenderData{ Graphics::EnumMeshType::DEBUG_CIRCLE,
                    diameter, diameter, 0.f,                                // width, height, orientation
                    glm::vec2{r_center.x, r_center.y},                      // convert the position
                    glm::vec4{r_color.x, r_color.y, r_color.z, r_color.w} } // convert the color
            );
        }


        void DrawCross(vec2 const& r_center, float const width, float const height, vec4 const& r_color)
        {
            glm::vec2 glmCenter { r_center.x, r_center.y };
            glm::vec4 glmColor { r_color.x, r_color.y, r_color.z, r_color.w };

            // Draw the horizontal line
            const_cast<Graphics::NewRendererManager*>(GETRENDERERMANAGER())->AddDebugShape(
                Graphics::RenderData{ Graphics::EnumMeshType::DEBUG_LINE,
                    width, 0.f, 0.f,        // width, height, orientation
                    glmCenter, glmColor }   // position, color
            );

            // Draw the vertical line
            const_cast<Graphics::NewRendererManager*>(GETRENDERERMANAGER())->AddDebugShape(
                Graphics::RenderData{ Graphics::EnumMeshType::DEBUG_LINE,
                    height, 0.f, 90.f,      // width, height, orientation
                    glmCenter, glmColor }   // position, color
            );
        }
#endif // !TEST_BATCH_RENDERER

    } // End of Debug namespace
} // End of PE namespace