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
           of a GLFW window, along with the relevant callbacks and window operations.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Graphics/GLHeaders.h"
#include "FrameRateTargetControl.h"
#include "InputHandler.h"
#include "Events/EventHandler.h"
#include "System.h"

namespace PE
{
    class WindowManager
    {
    public:
        WindowManager();
        ~WindowManager();

        GLFWwindow* InitWindow(int width, int height, const char* title);
        void UpdateTitle(GLFWwindow* window, double fps);
        void Cleanup();
        virtual std::string GetName() { return "Windows Manager"; }

        //window callback functions
        void static window_resize_callback(GLFWwindow* window, int width, int height);
        void static window_pos_callback(GLFWwindow* window, int width, int height);
        void static window_close_callback(GLFWwindow* window);
        void static window_focus_callback(GLFWwindow* window, int focus);


        void OnWindowEvent(const PE::Event<PE::WindowEvents>& e);
        void OnMouseEvent(const PE::Event<PE::MouseEvents>& e);
        void OnKeyEvent(const PE::Event<PE::KeyEvents>& e);
    };
}
