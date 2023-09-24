/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Text.frag
 \date     23-09-2023
 
 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu
 
 \brief     This file implements a fragment shader that renders fragments based on the
            uniform color or the texture passed in. 
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#version 460 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}  
