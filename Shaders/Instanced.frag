/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Instanced.frag
 \date     16-09-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file implements a fragment shader that renders fragments based on the
            uniform color or the texture passed in. 
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#version 450 core

layout (location = 0) in vec4 vColor;         // IN color
layout (location = 1) in vec2 vTextureCoord;   // IN texture coordinate

layout (location = 0) out vec4 fFragColor;	// OUT RGBA color

uniform sampler2D uTextureSampler2d;   // Texture sampler to access texture image
uniform bool uIsTextured; // Set to true to sample texture coordinates, false to just use the color

void main(void) {
	if(uIsTextured)
	{
		// Sample the texture using the texture coordinates
		fFragColor = texture(uTextureSampler2d, vTextureCoord);
		fFragColor *= vColor;
	} 
	else 
	{
		fFragColor = vColor;
	}
}
