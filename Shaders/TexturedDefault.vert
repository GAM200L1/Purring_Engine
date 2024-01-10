/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     TexturedDefault.vert
 \date     12-12-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file implements a minimal vertex shader that takes in a position 
            in NDC coordinates and texture coordinates and passes it to the next 
            hardware stage unchanged.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#version 450 core

layout (location = 0) in vec2 aVertexPosition; // IN vertex position
layout (location = 1) in vec4 aColor;          // IN RGBA color
layout (location = 2) in vec2 aTextureCoord;   // IN texture coordinate
layout (location = 3) in float aTextureIndex;  // IN texture index in array

layout (location = 0) out vec4 vColor;         // OUT RGBA color
layout (location = 1) out vec2 vTextureCoord;  // OUT texture coordinate
layout (location = 2) out float vTextureIndex; // OUT texture index in array

void main(void) {
    gl_Position = vec4(aVertexPosition, 0.0, 1.0);
    vColor = aColor;
	vTextureCoord = aTextureCoord;
	vTextureIndex = aTextureIndex;
}