/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FrameRateTargetControl.cpp
 \creation date:       13-08-2023
 \last updated:        16-09-2023
 \author:              Hans (You Yang) ONG

 \par      email:      youyang.o\@digipen.edu

 \brief    This file contains the implementation details of the FrameRateTargetControl class.
           FrameRateTargetControl ensures frame-rate consistency, enabling control and monitoring
           of frame rates throughout the application runtime.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/


/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "FrameRateTargetControl.h"
#include <iostream>
#include <thread>
#include <chrono>

/*                                                                                                          class member implementations
--------------------------------------------------------------------------------------------------------------------- */
/*-----------------------------------------------------------------------------
/// <summary>
/// Constructor for the FrameRateTargetControl class.
/// Initializes tracking to default 60 FPS.
/// </summary>
----------------------------------------------------------------------------- */
FrameRateTargetControl::FrameRateTargetControl()
    : m_targetFrameTime(1.0 / 60.0),  // Default to 60 FPS
    m_lastFrameStartTime(0.0),
    m_frameCount(0),
    m_timeSinceLastFPSUpdate(0.0),
    m_currentFPS(0.0),
    m_deltaTime(0.0)
{}



/*-----------------------------------------------------------------------------
/// <summary>
/// Set the desired target frames per second.
/// </summary>
/// <param name="fps">Target frames per second.</param>
----------------------------------------------------------------------------- */
void FrameRateTargetControl::SetTargetFPS(unsigned int fps)
{
    m_targetFrameTime = 1.0 / static_cast<double>(fps);
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Capture the start time of the current frame and increment frame count.
/// </summary>
----------------------------------------------------------------------------- */
void FrameRateTargetControl::StartFrame()
{
    double currentFrameTime = glfwGetTime();
    m_deltaTime = currentFrameTime - m_lastFrameStartTime;
    m_lastFrameStartTime = currentFrameTime;
    m_frameCount++;
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Calculate and handle frame timings at the end of a frame.
/// Sleeps the thread if frame rendering is faster than target time.
/// </summary>
----------------------------------------------------------------------------- */
void FrameRateTargetControl::EndFrame()
{
    m_timeSinceLastFPSUpdate += m_deltaTime;
    if (m_timeSinceLastFPSUpdate > 1.0)  // Update FPS value once per second
    {
        m_currentFPS = static_cast<double>(m_frameCount) / m_timeSinceLastFPSUpdate;
        m_frameCount = 0;
        m_timeSinceLastFPSUpdate = 0.0;
    }

        // delta time
    double currentFrameTime = glfwGetTime() - m_lastFrameStartTime;
    //std::cout << m_currentFPS << ", " << m_targetFrameTime << ", " << m_deltaTime << ", "  << currentFrameTime << '\n';
    if (currentFrameTime < m_targetFrameTime)
    {
        double sleepTime = m_targetFrameTime - currentFrameTime;
        //std::cout << "Sleep: " << sleepTime << '\n';
        std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
    }
    
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Retrieve the current frames per second.
/// </summary>
/// <returns>Current FPS.</returns>
----------------------------------------------------------------------------- */
double FrameRateTargetControl::GetFPS() const
{
    return m_currentFPS;
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Updates the target frames per second (FPS) based on a key input.
/// The function maps specific key inputs to predetermined FPS values.
/// </summary>
/// <param name="key">The key input used to determine the target FPS.</param>
----------------------------------------------------------------------------- */
void FrameRateTargetControl::UpdateTargetFPSBasedOnKey(int key)
{
    switch (key)
    {
    case GLFW_KEY_1:
        SetTargetFPS(60);
        std::cout << "Changed target FPS to 60." << std::endl;
        break;
    case GLFW_KEY_2:
        SetTargetFPS(75);
        std::cout << "Changed target FPS to 75." << std::endl;
        break;
    case GLFW_KEY_3:
        SetTargetFPS(120);
        std::cout << "Changed target FPS to 120." << std::endl;
        break;
    case GLFW_KEY_4:
        SetTargetFPS(144);
        std::cout << "Changed target FPS to 144." << std::endl;
        break;
    case GLFW_KEY_5:
        SetTargetFPS(165);
        std::cout << "Changed target FPS to 165." << std::endl;
        break;
    case GLFW_KEY_6:
        SetTargetFPS(180);
        std::cout << "Changed target FPS to 180." << std::endl;
        break;
    case GLFW_KEY_7:
        SetTargetFPS(240);
        std::cout << "Changed target FPS to 240." << std::endl;
        break;
    case GLFW_KEY_8:
        SetTargetFPS(360);
        std::cout << "Changed target FPS to 360." << std::endl;
        break;
    default:
        SetTargetFPS(60);
        std::cout << "Changed target FPS to the default 60." << std::endl;
        break;
    }
}
