#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     VertexData.h
 \date     20-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the VertexData class, which contains the position
            and texture coordinates of a vertex.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/


/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glm/glm.hpp>

namespace PE
{
    namespace Graphics
    {
        class VertexData
        {
            // ----- Public variables ----- // 
        public:
            glm::vec2 position{};	    //! xy coordinates of the vertex
            glm::vec2 textureCoord{};	//! 2D texture coordinates of the vertex

            // ----- Constructors ----- // 
        public:
            /*!***********************************************************************************
             \brief Constructor that takes in the vertex position and texture coordinates.

             \param[in] r_position Vertex position in model space.
             \param[in] r_textureCoord 2D texture coordinates of the vertex.
            *************************************************************************************/
            VertexData(glm::vec2 const& r_position = {0.f, 0.f}, glm::vec2 const& r_textureCoord = { 0.f, 0.f });
        };
    } // End of Graphics namespace
} // End of PE namespace
