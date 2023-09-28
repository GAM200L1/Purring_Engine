/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     WindowManager.h
 \date     28-08-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu
 \par      code %:     <remove if sole author>
 \par      changes:    <remove if sole author>

 \brief	   This file contains the implementation of the WindowManager class.
           WindowManager handles the initialization, maintenance, and cleanup
           of a GLFW window, along with the relevant callbacks and window operations.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Graphics/GLHeaders.h"
#include "FrameRateTargetControl.h"
#include "InputSystem.h"
#include "Events/EventHandler.h"
#include "System.h"

namespace PE
{
    /*!***********************************************************************************
     \brief     WindowManager class for handling GLFW window management and events.
    *************************************************************************************/
    class WindowManager
    {
    public:
        /*!***********************************************************************************
         \brief     Constructor for initializing GLFW library.
        *************************************************************************************/
        WindowManager();

        /*!***********************************************************************************
         \brief     Destructor for cleaning up GLFW resources.
        *************************************************************************************/
        ~WindowManager();

        /*!***********************************************************************************
         \brief     Initialize a new GLFW window.
         \param     width Window width.
         \param     height Window height.
         \param     title Window title.
         \return    GLFWwindow* Pointer to the created window.
        *************************************************************************************/
        GLFWwindow* InitWindow(int width, int height, const char* p_title);

        /*!***********************************************************************************
         \brief     Update the window title to show current FPS.
         \param     window Pointer to the GLFW window.
         \param     fps Current frames per second.
        *************************************************************************************/
        void UpdateTitle(GLFWwindow* p_window, double fps);

        /*!***********************************************************************************
         \brief     Cleanup resources and terminate GLFW library.
        *************************************************************************************/
        void Cleanup();

        /*!***********************************************************************************
         \brief     Get the system's name for debugging and identification.
         \return    std::string The name of the system.
        *************************************************************************************/
        virtual std::string GetName() { return "Windows Manager"; }
/*                                                                                          Window Callback Functions
--------------------------------------------------------------------------------------------------------------------- */
        /*!***********************************************************************************
         \brief     Callback for window resize events.
         \param     window Pointer to the resized window.
         \param     width New window width.
         \param     height New window height.
        *************************************************************************************/
        void static window_resize_callback(GLFWwindow* p_window, int width, int height);

        /*!***********************************************************************************
         \brief     Callback for window position change events.
         \param     window Pointer to the moved window.
         \param     width New window x-position.
         \param     height New window y-position.
        *************************************************************************************/
        void static window_pos_callback(GLFWwindow* p_window, int width, int height);

        /*!***********************************************************************************
         \brief     Callback for window close events.
         \param     window Pointer to the closing window.
        *************************************************************************************/
        void static window_close_callback(GLFWwindow* p_window);

        /*!***********************************************************************************
         \brief     Callback for window focus events.
         \param     window Pointer to the window receiving or losing focus.
         \param     focus 1 if gaining focus, 0 if losing focus.
        *************************************************************************************/
        void static window_focus_callback(GLFWwindow* p_window, int focus);



/*                                                                                          Event Handling Functions
--------------------------------------------------------------------------------------------------------------------- */
        /*!***********************************************************************************
         \brief     Handle window-specific events.
         \param     e Event containing window-specific details.
        *************************************************************************************/
        void OnWindowEvent(const PE::Event<PE::WindowEvents>& r_event);

        /*!***********************************************************************************
         \brief     Handle mouse-specific events.
         \param     e Event containing mouse-specific details.
        *************************************************************************************/
        void OnMouseEvent(const PE::Event<PE::MouseEvents>& r_event);

        /*!***********************************************************************************
         \brief     Handle keyboard-specific events.
         \param     e Event containing keyboard-specific details.
        *************************************************************************************/
        void OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event);
    };
}
