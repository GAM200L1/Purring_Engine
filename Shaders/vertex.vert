/*!
@file    my-tutorial-1.vert
@author  brandonjunjie.ho@digipen.edu
@date    6/3/2023

@brief
This file contains the code for the vertex shader.

*//*__________________________________________________________________________*/

#version 450 core

layout (location = 0) in vec2 aVertexPosition;
layout (location = 1) in vec3 aVertexColor;

layout (location = 0) out vec3 vColor;

uniform mat4 uModelToNdc;

void main(void) {
    gl_Position = uModelToNdc * vec4(aVertexPosition, 0.0, 1.0);
    vColor = aVertexColor;
}
