/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     TexturedDefault.frag
 \date     12-12-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file implements a fragment shader that renders fragments based on the
            uniform color or the texture passed in. 
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#version 450 core

layout (location = 0) in vec4 vColor;     		// IN RGBA color
layout (location = 1) in vec2 vTextureCoord;    // IN texture coordinate
layout (location = 2) flat in float vTextureIndex;   // IN texture index in array

layout (binding = 0) uniform sampler2D uTextureSamplers[32];

layout (location = 0) out vec4 fFragColor;	// OUT RGBA color

void main(void) {
	// Get the corresponding texture
	int textureIndex = int(vTextureIndex);

	// Sample the texture using the texture coordinates
	fFragColor = texture(uTextureSamplers[textureIndex], vTextureCoord);
	//fFragColor *= vec4(vTextureCoord, 0.0, 1.0);
	fFragColor *= vColor;
}
