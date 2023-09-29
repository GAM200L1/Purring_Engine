/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     VertexData.cpp
 \date     23-09-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the VertexData class, which contains the position 
            and texture coordinates of a vertex.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "VertexData.h"

namespace PE
{
    namespace Graphics
    {
        VertexData::VertexData(glm::vec2 const& r_position, glm::vec2 const& r_textureCoord) :
            position{r_position}, textureCoord{ r_textureCoord }
        { /* Empty by design */ }
    } // End of Graphics namespace
} // End of PE namespace
