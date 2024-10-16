/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Instanced.vert
 \date     16-09-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file implements a minimal vertex shader that takes in a position, 
            color and texture coordinates. These parameters are passed to the next 
            hardware stage unchanged.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#version 450 core

layout (location = 0) in vec2 aVertexPosition; // IN vertex position
layout (location = 1) in vec2 aTextureCoord;   // IN texture coordinate
layout (location = 2) in vec2 aTextureCoordDynamic;

layout (location = 3) in float aIsTextured;    // IN texture boolean
layout (location = 4) in vec4 aColor;          // IN vertex color
layout (location = 5) in mat4 aModelToWorld;   // IN transformation matrix

layout (location = 0) out vec4 vColor;         // OUT color
layout (location = 1) out vec2 vTextureCoord;  // OUT texture coordinate
layout (location = 2) out float vIsTextured;    // OUT texture boolean

uniform mat4 uWorldToNdc;   // World to NDC matrix

void main(void) {
    gl_Position = uWorldToNdc * aModelToWorld * vec4(aVertexPosition, 0.0, 1.0);
    vColor = aColor;
    aTextureCoord;
	vTextureCoord = aTextureCoordDynamic;
    vIsTextured = aIsTextured;
}