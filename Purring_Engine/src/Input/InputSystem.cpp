/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     InputSystem.cpp
 \date     25-09-2023

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
#include "Logging/Logger.h"
#include "Events/EventHandler.h"
#include "Time/TimeManager.h"
namespace PE 
{
    //static declarations
    float InputSystem::m_bufferTime = 0.12f;
    std::vector<KeyPressedEvent> InputSystem::m_KeyDown;
    std::vector<MouseButtonHoldEvent> InputSystem::m_MouseDown;
    std::map<int, float> InputSystem::m_KeyTriggered;


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
        mods;

        switch (action)
        {
        case GLFW_PRESS:
        {
            double x_d, y_d;
            //creation of event and sending
            PE::MouseButtonPressedEvent mbpe;
            glfwGetCursorPos(window, &x_d, &y_d);
            float x{static_cast<float>(x_d)}, y{static_cast<float>(y_d)};
            mbpe.x = static_cast<int>(x);
            mbpe.y = static_cast<int>(y);
            ConvertGLFWToTransform(window,x,y);
            mbpe.transX = static_cast<int>(x);
            mbpe.transY = static_cast<int>(y);
            PE::MouseButtonHoldEvent mbhe;
            mbpe.button = (int)button;
            mbhe.button = (int)button;
            mbhe.repeat = m_bufferTime; // will implement hold for mouse next time
            m_MouseDown.push_back(mbhe);
            PE::SEND_MOUSE_EVENT(mbpe)

                break;
        }
        case GLFW_RELEASE:
        {
            //creation of event and sending
            PE::MouseButtonReleaseEvent mbre;
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            mbre.x = static_cast<int>(x);
            mbre.y = static_cast<int>(y);
            mbre.button = (int)button;
            if (!m_MouseDown.empty())
            {
                for (std::vector<MouseButtonHoldEvent>::iterator it = std::begin(m_MouseDown); it != std::end(m_MouseDown);)
                {
                    if (it->button == mbre.button)
                    {
                        it = m_MouseDown.erase(it);
                    }
                    else
                        ++it;
                }
            }
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
        mse.xOffset = static_cast<float>(xoffset);
        mse.yOffset = static_cast<float>(yoffset);
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
            m_KeyTriggered[key] = static_cast<float>(TimeManager::GetInstance().GetFixedTimeStep() * 1.01);
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

        for (auto& mhe : m_MouseDown)
        {
            if (mhe.repeat >= 0)
            {
                mhe.repeat -= deltaTime;
            }
            else 
            {
                //if buffer is less than 0 send the event as a on hold event
                PE::SEND_MOUSE_EVENT(mhe)
            }
        }

        for (auto& [key, val] : m_KeyTriggered)
        {
            val -= TimeManager::GetInstance().GetFixedTimeStep();
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

    bool InputSystem::IsKeyTriggered(int keycode)
    {
        auto it = m_KeyTriggered.find(keycode);
        if (it != m_KeyTriggered.end())
        {
            if (m_KeyTriggered[keycode] <= 0)
                return false;

            return true;
        }
        else return false;
    }

    bool InputSystem::IsKeyHeld(int keycode)
    {
        if (!m_KeyDown.empty())
        {
            for (std::vector<KeyPressedEvent>::iterator it = std::begin(m_KeyDown); it != std::end(m_KeyDown);)
            {
                if (it->keycode == keycode)
                {
                    return true;
                }
                ++it;
            }
        }
        return false;
    }

    void InputSystem::ConvertGLFWToTransform(GLFWwindow* window, float& x, float& y)
    {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        x = x - static_cast<float>(windowWidth) * 0.5f;
        y = static_cast<float>(windowHeight) * 0.5f - y;
    }
        

    void InputSystem::GetCursorViewportPosition(GLFWwindow* window, float& x, float& y)
    {
        double x_d{}, y_d{};
        glfwGetCursorPos(window, &x_d, &y_d);
        x = static_cast<float>(x_d), y = static_cast<float>(y_d);
        ConvertGLFWToTransform(window, x, y);
    }

}
