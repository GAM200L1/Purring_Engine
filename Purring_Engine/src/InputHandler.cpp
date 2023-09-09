/*!*****************************************************************************
    @file       InputHandler.cpp
    @author     Hans (You Yang) ONG
    @co-author
    @par        DP email: youyang.o\@digipen.edu
    @par        Course: CSD2401, Section A
    @date       130823

    @brief      This file contains the implementation details of the mouse input callbacks.
                Functions in this file handle mouse movements and button presses.

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/



/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "InputHandler.h"
#include <iostream>
#include "Logging/Logger.h"

#include "Events/EventHandler.h"



/*-----------------------------------------------------------------------------
/// <summary>
/// Callback function for mouse movement. Outputs the current mouse position to the console.
/// </summary>
----------------------------------------------------------------------------- */
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //std::cout << "Mouse Position: x: " << xpos << ", y: " << ypos << std::endl;

    temp::MouseMovedEvent mme;

    mme.x = xpos;
    mme.y = ypos;

    temp::SEND_MOUSE_EVENT(mme)
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Callback function for mouse button actions. Outputs which mouse button was pressed to the console.
/// </summary>
----------------------------------------------------------------------------- */
void check_mouse_buttons(GLFWwindow* window, int button, int action, int mods)
{

    switch (action)
    {
    case GLFW_PRESS:
    {
        temp::MouseButtonPressedEvent mbpe;
        mbpe.button = (int)button;
        temp::SEND_MOUSE_EVENT(mbpe)
        break;
    }
    case GLFW_RELEASE:
    {
        temp::MouseButtonReleaseEvent mbre;
        mbre.button = (int)button;
        temp::SEND_MOUSE_EVENT(mbre)
        break;
    }

    }
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Callback function for mouse scroll actions. Outputs the scroll direction to the console.
/// </summary>
----------------------------------------------------------------------------- */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
        temp::MouseScrolledEvent mse;
        mse.xOffset = xoffset;
        mse.yOffset = yoffset;
        temp::SEND_MOUSE_EVENT(mse)
}


/*-----------------------------------------------------------------------------
/// <summary>
/// Callback function for keyboard actions. Outputs which key was pressed or released to the console.
/// </summary>
----------------------------------------------------------------------------- */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch(action)
    {
     case GLFW_PRESS:
      {
         temp::KeyPressedEvent kpe;
         kpe.keycode = key;
         kpe.repeat = false;
         temp::SEND_KEY_EVENT(kpe)
         break;
      }
     case GLFW_RELEASE:
     {
         temp::KeyReleaseEvent kre;
         kre.keycode = key;
         temp::SEND_KEY_EVENT(kre)
         break;
     }
     case GLFW_REPEAT:
     {
         temp::KeyPressedEvent kpe;
         kpe.keycode = key;
         kpe.repeat = true;
         temp::SEND_KEY_EVENT(kpe)
         break;
     }
    }
    
}

