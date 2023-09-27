/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Textured.vert
 \date     16-09-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file implements a minimal vertex shader that takes in a position, 
            color and texture coordinates. These parameters are passed to the next 
            hardware stage unchanged.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#version 460 core

layout (location = 0) in vec2 aVertexPosition; // IN vertex position
layout (location = 1) in vec2 aTextureCoord;   // IN texture coordinate

layout (location = 0) out vec2 vTextureCoord;  // OUT texture coordinate

uniform mat4 uModelToNdc;

void main(void) {
    gl_Position = uModelToNdc * vec4(aVertexPosition, 0.0, 1.0);
	vTextureCoord = aTextureCoord;
}