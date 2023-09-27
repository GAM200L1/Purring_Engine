/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     WindowManager.h
 \creation date:       13-08-2023
 \last updated:        16-09-2023
 \author:              Hans (You Yang) ONG

 \par      email:      youyang.o@digipen.edu

 \brief    This file contains the implementation of the WindowManager class.
           WindowManager handles the initialization, maintenance, and cleanup
           of a GLFW p_window, along with the relevant callbacks and p_window operations.

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
    class WindowManager
    {
    public:
        WindowManager();
        ~WindowManager();

        GLFWwindow* InitWindow(int width, int height, const char* p_title);
        void UpdateTitle(GLFWwindow* p_window, double fps);
        void Cleanup();
        virtual std::string GetName() { return "Windows Manager"; }

        //p_window callback functions
        void static window_resize_callback(GLFWwindow* p_window, int width, int height);
        void static window_pos_callback(GLFWwindow* p_window, int width, int height);
        void static window_close_callback(GLFWwindow* p_window);
        void static window_focus_callback(GLFWwindow* p_window, int focus);


        void OnWindowEvent(const PE::Event<PE::WindowEvents>& r_event);
        void OnMouseEvent(const PE::Event<PE::MouseEvents>& r_event);
        void OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event);
    };
}
