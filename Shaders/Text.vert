/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Text.vert
 \date     23-09-2023
 
 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu
 
 \brief     This file implements a minimal vertex shader that takes in a position, 
            color and texture coordinates. These parameters are passed to the next 
            hardware stage unchanged.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#version 460 core

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

out vec2 TexCoords;

uniform mat4  u_ViewProjection;

void main()
{
    gl_Position = u_ViewProjection * vec4(vertex.xy, 0.1, 1.0);
    TexCoords = vertex.zw;
}