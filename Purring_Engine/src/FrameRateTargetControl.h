/*!*****************************************************************************
    @file       FrameRateTargetControl.h
    @author     Hans (You Yang) ONG
    @co-author
    @par        DP email: youyang.o\@digipen.edu
    @par        Course: CSD2401, Section A
    @date       130823

    @brief      This file contains the declaration of the FrameRateTargetControl class.
                FrameRateTargetControl ensures frame-rate consistency, enabling control and monitoring
                of frame rates throughout the application runtime.

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Graphics/GLHeaders.h"

namespace PE
{
    class FrameRateTargetControl
    {
    public:
        FrameRateTargetControl();

        void SetTargetFPS(unsigned int fps);
        void EndFrame();
        double GetFPS() const;
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
