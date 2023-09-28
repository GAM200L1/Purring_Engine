#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     VertexData.h
 \date     20-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the VertexData class, which contains the position,
            color and texture coordinates of a vertex.
            
 
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
        public:
            glm::vec2 position{};	    //! xy coordinates of the vertex
            glm::vec3 color{};	        //! 3D vector representing the rgb values of the vertex
            glm::vec2 textureCoord{};	//! 2D texture coordinates of the vertex
        };
    } // End of Graphics namespace
} // End of PE namespace
