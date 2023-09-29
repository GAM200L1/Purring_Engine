/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FrameRateTargetControl.h
 \creation date:       13-08-2023
 \last updated:        16-09-2023
 \author:              Hans (You Yang) ONG

 \par      email:      youyang.o\@digipen.edu

 \brief    This file contains the implementation details of the FrameRateTargetControl class.
           FrameRateTargetControl ensures frame-rate consistency, enabling control and monitoring
           of frame rates throughout the application runtime.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Graphics/GLHeaders.h"

namespace PE
{
    /*!***********************************************************************************
     \brief     FrameRateTargetControl class for managing target frame rates.
    *************************************************************************************/
    class FrameRateTargetControl
    {
    public:
        /*!***********************************************************************************
         \brief     Constructor for initializing frame rate target variables.
        *************************************************************************************/
        FrameRateTargetControl();

        /*!***********************************************************************************
         \brief     Set the target frames per second.
         \param     fps The desired FPS.
        *************************************************************************************/
        void SetTargetFPS(unsigned int fps);

        /*!***********************************************************************************
         \brief     Ends the current frame and updates FPS counter.
        *************************************************************************************/
        void EndFrame();
        
        /*!***********************************************************************************
         \brief     Gets the current frames per second.
         \return    double Current FPS.
        *************************************************************************************/
        double GetFPS() const;

        /*!***********************************************************************************
         \brief     Update the target FPS based on a key input.
         \param     key The input key.
        *************************************************************************************/
        void UpdateTargetFPSBasedOnKey(int key);

    private:
        double m_targetFrameTime;
        double m_frameCount;
        float m_timeSinceLastFPSUpdate;
        double m_currentFPS;

        // not needed
        double m_deltaTime;
    };
}
