/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     InputHandler.cpp
 \creation date:       13-08-2023
 \last updated:        16-09-2023
 \author:              Hans (You Yang) ONG

 \par      email:      youyang.o\@digipen.edu

 \brief    This file contains the implementation details of the mouse input callbacks.
           The functions in this file handle mouse movements and button presses.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/


/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
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
    window;

    PE::MouseMovedEvent mme;

    mme.x = static_cast<int>(xpos);
    mme.y = static_cast<int>(ypos);

    PE::SEND_MOUSE_EVENT(mme)
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Callback function for mouse button actions. Outputs which mouse button was pressed to the console.
/// </summary>
----------------------------------------------------------------------------- */
void check_mouse_buttons(GLFWwindow* window, int button, int action, int mods)
{
    window; mods;

    switch (action)
    {
    case GLFW_PRESS:
    {
        PE::MouseButtonPressedEvent mbpe;
        mbpe.button = (int)button;
        PE::SEND_MOUSE_EVENT(mbpe)
        break;
    }
    case GLFW_RELEASE:
    {
        PE::MouseButtonReleaseEvent mbre;
        mbre.button = (int)button;
        PE::SEND_MOUSE_EVENT(mbre)
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
    window;
    PE::MouseScrolledEvent mse;
    mse.xOffset = static_cast<int>(xoffset);
    mse.yOffset = static_cast<int>(yoffset);
    PE::SEND_MOUSE_EVENT(mse)
}


/*-----------------------------------------------------------------------------
/// <summary>
/// Callback function for keyboard actions. Outputs which key was pressed or released to the console.
/// </summary>
----------------------------------------------------------------------------- */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    window; scancode; mods;

    switch(action)
    {
     case GLFW_PRESS:
      {
         PE::KeyPressedEvent kpe;
         kpe.keycode = key;
         kpe.repeat = false;
         PE::SEND_KEY_EVENT(kpe)
         break;
      }
     case GLFW_RELEASE:
     {
         PE::KeyReleaseEvent kre;
         kre.keycode = key;
         PE::SEND_KEY_EVENT(kre)
         break;
     }
     case GLFW_REPEAT:
     {
         PE::KeyPressedEvent kpe;
         kpe.keycode = key;
         kpe.repeat = true;
         PE::SEND_KEY_EVENT(kpe)
         break;
     }
    }
    
}
