/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     InputSystem.cpp
 \date     25-09-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu
 \par      code %:     <remove if sole author>
 \par      changes:    <remove if sole author>

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

    /*!***********************************************************************************
     \brief     GLFW callback function for mouse movements. Creates and sends a MouseMovedEvent
                based on the cursor's x and y position.

     \tparam T          This function does not use a template.
     \param[in] window  Pointer to the GLFWwindow that received the event.
     \param[in] xpos    The x-coordinate of the cursor position.
     \param[in] ypos    The y-coordinate of the cursor position.
     \return void       Does not return a value.
    *************************************************************************************/
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



    /*!***********************************************************************************
     \brief     GLFW callback function for mouse button events. Creates and sends either a
                MouseButtonPressedEvent or MouseButtonReleaseEvent based on the button action.

     \tparam T          This function does not use a template.
     \param[in] window  Pointer to the GLFWwindow that received the event. 
     \param[in] button  The mouse button that was pressed or released.
     \param[in] action  The type of mouse button action (GLFW_PRESS or GLFW_RELEASE).
     \param[in] mods    Bit field describing which modifier keys were held down. 
     \return void       Does not return a value.
    *************************************************************************************/
    void InputSystem::check_mouse_buttons(GLFWwindow* window, int button, int action, int mods)
    {
        //unrefereced variables
        mods;

        switch (action)
        {
        case GLFW_PRESS:
        {
            double x, y;
            //creation of event and sending
            PE::MouseButtonPressedEvent mbpe;
            glfwGetCursorPos(window, &x, &y);
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



    /*!***********************************************************************************
     \brief     GLFW callback function for scroll wheel events. Creates and sends a MouseScrolledEvent
                based on the scroll wheel's x and y offsets.

     \tparam T          This function does not use a template.
     \param[in] window  Pointer to the GLFWwindow that received the event.
     \param[in] xoffset The x-offset of the scroll wheel.
     \param[in] yoffset The y-offset of the scroll wheel.
     \return void       Does not return a value.
    *************************************************************************************/
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



    /*!***********************************************************************************
     \brief     GLFW callback function for keyboard events. Creates and sends KeyTriggeredEvent,
                KeyPressedEvent, and KeyReleaseEvent based on the key event triggered.

     \tparam T          This function does not use a template.
     \param[in] window  Pointer to the GLFWwindow that received the event.
     \param[in] key     The key that triggered the event.
     \param[in] scancode The scancode of the key that triggered the event.
     \param[in] action  The action (GLFW_PRESS, GLFW_RELEASE) that triggered the event.
     \param[in] mods    Additional modifiers to the key event. (Currently is unused in the function)
     \return void       Does not return a value.
    *************************************************************************************/
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



    /*!***********************************************************************************
     \brief     Updates the InputSystem by decrementing the buffer time for each key pressed
                event. If the buffer time is less than 0, sends an "on-hold" event for the key.

     \param[in] deltaTime  The time elapsed since the last frame, used for updating the buffer time.
     \return void          Does not return a value.
    *************************************************************************************/
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



    /*!***********************************************************************************
     \brief     Retrieves the name of the InputSystem.

     \return std::string  The name of the InputSystem ("InputSystem").
    *************************************************************************************/
    std::string InputSystem::GetName()
    {
        return "InputSystem";
    }



    /*!***********************************************************************************
     \brief     Sets the buffer time for key hold events in the InputSystem.

     \param[in] s  The time in seconds to set as the hold buffer time.
     \return void  Does not return a value.
    *************************************************************************************/
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

    void InputSystem::ConvertGLFWToTransform(GLFWwindow* window, double& x, double& y)
    {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        x = x - static_cast<double>(windowWidth) * 0.5;
        y = static_cast<double>(windowHeight) * 0.5 - y;
    }
        

    void InputSystem::GetCursorViewportPosition(GLFWwindow* window, double& x, double& y)
    {
        glfwGetCursorPos(window, &x, &y);
        ConvertGLFWToTransform(window, x, y);
    }

}
