/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     ColoredDefault.frag
 \date     12-12-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file implements a fragment shader that renders fragments based on the
            uniform color or the texture passed in. 
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#version 450 core

layout (location = 0) in vec4 vColor;     	// IN RGBA color

layout (location = 0) out vec4 fFragColor;	// OUT RGBA color

void main(void) {
	fFragColor = vColor;
}
