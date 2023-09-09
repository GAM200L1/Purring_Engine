/*!*****************************************************************************
    @file       InputHandler.h
    @author     Hans (You Yang) ONG
    @co-author
    @par        DP email: youyang.o\@digipen.edu
    @par        Course: CSD2401, Section A
    @date       130823

    @brief      This file contains the implementation details of the mouse input callbacks.
                Functions in this file handle mouse movements and button presses.

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include <GLFW/glfw3.h>


// Mouse callback functions
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void check_mouse_buttons(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Keyboard callback function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


