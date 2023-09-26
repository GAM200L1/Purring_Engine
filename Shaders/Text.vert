/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Text.vert
 \date     23-09-2023
 
 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu
 
 \brief     This file implements a minimal vertex shader that takes in a position, 
            multiplies it with a uniform view projection matrix. It also passes the
            texture coordinates stored in zw of the vertex input to the next stage.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#version 450 core

layout (location = 0) in vec4 vertex; // Position stored in xy, Texture stored in zw

out vec2 TexCoords;

uniform mat4  u_ViewProjection;

void main()
{
    gl_Position = u_ViewProjection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}