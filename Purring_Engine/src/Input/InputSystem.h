/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     InputSystem.h
 \date     25-09-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu
 \par      code %:     <remove if sole author>
 \par      changes:    <remove if sole author>

 \co-author            Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu
 \par      code %:     <remove if sole author>
 \par      changes:    <remove if sole author>

 \brief    This file contains the implementation details of the mouse input callbacks.
           The functions in this file handle mouse movements and button presses.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Graphics/GLHeaders.h"

#include "System.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
namespace PE 
{
    class InputSystem : public System
    {
        // ----- Constructors ----- // 
    public:
        /*!***********************************************************************************
         \brief Constructor for the Input System
        *************************************************************************************/
        InputSystem();
        /*!***********************************************************************************
         \brief Destructor for the Input System
        *************************************************************************************/
        virtual ~InputSystem();
        // ----- GLFW callbacks ----- // 
    public:
        /*!***********************************************************************************
         \brief Callback function for mouse movement. Outputs the current mouse position to the console.
         \param [In] GLFWwindow* window     glfw window
         \param [In] double xpos            xpos of mouse
         \param [In] double ypos            ypos of mouse
        *************************************************************************************/
        void static mouse_callback(GLFWwindow* window, double xpos, double ypos);
        /*!***********************************************************************************
         \brief Callback function for mouse button actions. Outputs which mouse button was pressed to the console.
         \param [In] GLFWwindow* window     glfw window
         \param [In] int button             Button code of button pressed
         \param [In] int action             what button was pressed
         \param [In] int mods               modifiers
        *************************************************************************************/
        void static check_mouse_buttons(GLFWwindow* window, int button, int action, int mods);
        /*!***********************************************************************************
         \brief Callback function for mouse scroll actions. Outputs the scroll direction to the console.
         \param [In] GLFWwindow* window     glfw window
         \param [In] double xoffset         xoffset of mouse scroll
         \param [In] double yoffset         yoffset of mouse scroll
        *************************************************************************************/
        void static scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        /*!***********************************************************************************
         \brief Callback function for keyboard actions. Outputs which key was pressed or released to the console.
         \param [In] GLFWwindow* window     glfw window
         \param [In] int key                key code of key pressed
         \param [In] int scancode           scan code of key pressed
         \param [In] int action             what button was pressed
         \param [In] int mods               modifiers
        *************************************************************************************/
        void static key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

        // ----- Public Methods ----- // 
    public:
        /*!***********************************************************************************
         \brief Initialize function for the Input System
        *************************************************************************************/
        void InitializeSystem();
        /*!***********************************************************************************
         \brief Update Function for the Input System
         \param [In] float deltaTime, time taken for the current frame
        *************************************************************************************/
        void UpdateSystem(float deltaTime);
        /*!***********************************************************************************
         \brief Clear any allocated memory for the Input System
        *************************************************************************************/
        void DestroySystem();
        /*!***********************************************************************************
         \brief Return Input System
        *************************************************************************************/
        
        // ----- Public Getter/Setter ----- // 
    public:
        /*!***********************************************************************************
         \brief Return the type of system
        *************************************************************************************/
        std::string GetName();
        /*!***********************************************************************************
         \brief To set the buffer time before counting a key press as hold
         \param [In] float buffer time
        *************************************************************************************/
        void SetHoldBufferTime(float s);

        // ----- Private Variables ----- // 
    private:
        static std::vector<KeyPressedEvent> m_KeyDown; // vector holding current held down buttons
        static std::vector<MouseButtonHoldEvent> m_MouseDown; // vector holding current held down buttons
        static float m_bufferTime; // has to be static to be used by the callbacks
    };
}