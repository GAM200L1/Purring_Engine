/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     InputSystem.cpp
 \date     9/25/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief    This file contains the implementation details of the mouse input callbacks.
           The functions in this file handle mouse movements and button presses.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/


/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "InputSystem.h"
#include <iostream>
#include "Logging/Logger.h"

#include "Events/EventHandler.h"


namespace PE 
{
    //static declarations
    float InputSystem::m_bufferTime = 0.12f;
    std::vector<KeyPressedEvent> InputSystem::m_KeyDown;

    //system functions
    void InputSystem::InitializeSystem(){}
    InputSystem::InputSystem(){}
    InputSystem::~InputSystem(){}
    void InputSystem::DestroySystem() {}

    void InputSystem::mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        //unreferenced variable
        window;

        //creation of event and sending
        PE::MouseMovedEvent mme;

        mme.x = static_cast<int>(xpos);
        mme.y = static_cast<int>(ypos);

        PE::SEND_MOUSE_EVENT(mme)
    }

    void InputSystem::check_mouse_buttons(GLFWwindow* window, int button, int action, int mods)
    {
        //unrefereced variables
        window; mods;

        switch (action)
        {
        case GLFW_PRESS:
        {
            //creation of event and sending
            PE::MouseButtonPressedEvent mbpe;
            mbpe.button = (int)button;
            mbpe.repeat = m_bufferTime; // will implement hold for mouse next time
            PE::SEND_MOUSE_EVENT(mbpe)
                break;
        }
        case GLFW_RELEASE:
        {
            //creation of event and sending
            PE::MouseButtonReleaseEvent mbre;
            mbre.button = (int)button;
            PE::SEND_MOUSE_EVENT(mbre)
                break;
        }

        }
    }


    void InputSystem::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        //unreferenced variable
        window;
        //creation of event and sending
        PE::MouseScrolledEvent mse;
        mse.xOffset = xoffset;
        mse.yOffset = yoffset;
        PE::SEND_MOUSE_EVENT(mse)
    }



    void InputSystem::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        //unreferenced variables
        window; scancode; mods;

        switch (action)
        {
        case GLFW_PRESS:
        {
            //creation of event
            PE::KeyTriggeredEvent kte;
            PE::KeyPressedEvent kpe;
            kte.keycode = kpe.keycode = key;
            //setting a buffer before the keypressed becomes a repeat
            kpe.repeat = m_bufferTime;
            //saving the keypressed event
            m_KeyDown.push_back(kpe);
            //sending of event
            PE::SEND_KEY_EVENT(kte)
                break;
        }
        case GLFW_RELEASE:
        {
            //creation of release event
            PE::KeyReleaseEvent kre;
            kre.keycode = key;
            //remove the keypressed event from the saved vector
            if (!m_KeyDown.empty())
            {
                for (std::vector<KeyPressedEvent>::iterator it = std::begin(m_KeyDown); it != std::end(m_KeyDown);)
                {
                    if (it->keycode == kre.keycode)
                    {
                        it = m_KeyDown.erase(it);
                    }
                    else
                        ++it;
                }
            }
            //send event
            PE::SEND_KEY_EVENT(kre)
            break;
        }
        }

    }

    void InputSystem::UpdateSystem(float deltaTime)
    {
        //every frame reduce the buffer for repeat
        for (KeyPressedEvent& kpe : m_KeyDown)
        {
            if (kpe.repeat >= 0)
            {
                kpe.repeat -= deltaTime;
            }
            else
            {
                //if buffer is less than 0 send the event as a on hold event
                PE::SEND_KEY_EVENT(kpe)
            }
        }
    }

    std::string InputSystem::GetName()
    {
        return "InputSystem";
    }

    void InputSystem::SetHoldBufferTime(float s)
    {
        m_bufferTime = s;
    }


}