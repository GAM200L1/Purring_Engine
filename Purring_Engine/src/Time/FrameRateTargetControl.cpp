/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FrameRateTargetControl.cpp
 \date     28-08-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu
 \par      code %:     <remove if sole author>
 \par      changes:    <remove if sole author>

 \co-author            Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu
 \par      code %:     <remove if sole author>
 \par      changes:    <remove if sole author>

 \brief	   This file encompasses the implementation of the FrameRateTargetControl class that
           facilitates consistent frame-rate management across the application.
           Functionalities:
           1. Set a target FPS (Frames Per Second).
           2. Control the actual FPS to closely align with the target.
           3. Retrieve the current FPS.
           4. Update the target FPS based on keyboard input.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/


/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "Time/TimeManager.h"
#include "FrameRateTargetControl.h"
#include <thread>
#include <chrono>

namespace PE
{
    /*!***********************************************************************************
     \brief     Default constructor for FrameRateTargetControl.
                Initializes frame rate control variables.

     \tparam T          This function does not use a template.
     \return            Does not return a value, as this is a constructor.
    *************************************************************************************/
    FrameRateTargetControl::FrameRateTargetControl()
        : m_targetFrameTime(1.0 / 60.0),  // Default to 60 FPS
        m_frameCount(0),
        m_timeSinceLastFPSUpdate(0.0),
        m_currentFPS(0.0),
        m_deltaTime(0.0)
    {}



    /*!***********************************************************************************
     \brief     Set the target FPS for the frame rate controller.

     \tparam T          This function does not use a template.
     \param[in] fps     The target frames per second.
     \return void       Does not return a value.
    *************************************************************************************/
    void FrameRateTargetControl::SetTargetFPS(unsigned int fps)
    {
        m_targetFrameTime = 1.0 / static_cast<double>(fps);
    }



    /*!***********************************************************************************
     \brief     End of frame calculations and FPS control.

     \tparam T          This function does not use a template.
     \return void       Does not return a value.

     This function performs the following operations:
     1. Updates the frame count.
     2. Calculates the time since the last FPS update.
     3. Updates the current FPS once per second.
     4. Controls the FPS to match the target frame time.

     Note: This method relies on TimeManager::GetInstance().GetDeltaTime() for time calculation.
    *************************************************************************************/
    void FrameRateTargetControl::EndFrame()
    {
        ++m_frameCount;

        m_timeSinceLastFPSUpdate += TimeManager::GetInstance().GetDeltaTime();
        if (m_timeSinceLastFPSUpdate > 1.f)  // Update FPS value once per second
        {
            m_currentFPS = m_frameCount;
            m_frameCount = 0;
            m_timeSinceLastFPSUpdate = 0.f;
        }

        // FPS control
        std::chrono::duration<float> duration;
        do 
        {
            duration = std::chrono::high_resolution_clock::now() - TimeManager::GetInstance().GetStartTime();
            //std::cout << " Actual: " << glfwGetTime() << '\n';
        }
        while (duration.count() < m_targetFrameTime);
    }



    /*!***********************************************************************************
     \brief     Retrieve the current Frames Per Second (FPS).

     \tparam T          This function does not use a template.
     \return double     The current frames per second.
    *************************************************************************************/
    double FrameRateTargetControl::GetFPS() const
    {
        return m_currentFPS;
    }



    /*!***********************************************************************************
     \brief     Update the target FPS based on the pressed key.

     \tparam T          This function does not use a template.
     \param[in] key     The key code of the pressed key.
     \return void       Does not return a value.
    *************************************************************************************/
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
}
