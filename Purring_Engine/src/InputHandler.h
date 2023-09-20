/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     InputHandler.h
 \creation date:       13-08-2023
 \last updated:        16-09-2023
 \author:              Hans (You Yang) ONG

 \par      email:      youyang.o\@digipen.edu

 \brief    This file contains the implementation details of the mouse input callbacks.
           The functions in this file handle mouse movements and button presses.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Graphics/GLHeaders.h"

// Mouse callback functions
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void check_mouse_buttons(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Keyboard callback function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
