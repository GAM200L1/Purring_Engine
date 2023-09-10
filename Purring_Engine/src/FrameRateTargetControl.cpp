/*!*****************************************************************************
    @file       FrameRateTargetControl.cpp
    @author     Hans (You Yang) ONG
    @co-author
    @par        DP email: youyang.o\@digipen.edu
    @par        Course: CSD2401, Section A
    @date       130823

    @brief      This file contains the implementation details of the FrameRateTargetControl class.
                FrameRateTargetControl ensures frame-rate consistency, enabling control and monitoring
                of frame rates throughout the application runtime.

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/


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
    m_currentFPS(0.0)
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
    m_lastFrameStartTime = glfwGetTime();
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
    double currentFrameTime = glfwGetTime() - m_lastFrameStartTime;

    m_timeSinceLastFPSUpdate += currentFrameTime;
    if (m_timeSinceLastFPSUpdate > 1.0)  // Update FPS value once per second
    {
        m_currentFPS = static_cast<double>(m_frameCount) / m_timeSinceLastFPSUpdate;
        m_frameCount = 0;
        m_timeSinceLastFPSUpdate = 0.0;
    }

    if (currentFrameTime < m_targetFrameTime)
    {
        double sleepTime = m_targetFrameTime - currentFrameTime;
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
