/*!
@file    my-tutorial-1.frag
@author  brandonjunjie.ho@digipen.edu
@date    6/3/2023

@brief
This file contains the code for the fragment shader.

*//*__________________________________________________________________________*/

#version 460 core

layout(location = 0) in vec3 vColor;

layout(location = 0) out vec4 fFragColor;

void main(void) {
    fFragColor = vec4(vColor, 1.0);
}
