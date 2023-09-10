/*!*****************************************************************************
    @file       WindowManager.h
    @author     Hans (You Yang) ONG
    @co-author
    @par        DP email: youyang.o\@digipen.edu
    @par        Course: CSD2401, Section A
    @date       130823

    @brief      This file contains the declaration of the WindowManager class.
                WindowManager provides interfaces for the initialization, maintenance,
                and cleanup of a GLFW window, along with relevant callbacks and window operations.

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "GLFW/glfw3.h"
#include "FrameRateTargetControl.h"
#include "InputHandler.h"
#include "Events/EventHandler.h"

class WindowManager
{
public:
    WindowManager();
    ~WindowManager();

    GLFWwindow* InitWindow(int width, int height, const char* title);
    void UpdateTitle(GLFWwindow* window, double fps);
    void Cleanup();

    //window callback functions
    void static window_resize_callback(GLFWwindow* window, int width, int height);
    void static window_pos_callback(GLFWwindow* window, int width, int height);
    void static window_close_callback(GLFWwindow* window);
    void static window_focus_callback(GLFWwindow* window, int focus);


    void OnWindowEvent(const temp::Event<temp::WindowEvents>& e);
    void OnMouseEvent(const temp::Event<temp::MouseEvents>& e);
    void OnKeyEvent(const temp::Event<temp::KeyEvents>& e);
};



