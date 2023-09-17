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

class FrameRateTargetControl
{
public:
    FrameRateTargetControl();

    void SetTargetFPS(unsigned int fps);
    void StartFrame();
    void EndFrame();
    double GetFPS() const;
    void UpdateTargetFPSBasedOnKey(int key);

private:
    double m_targetFrameTime;
    double m_lastFrameStartTime;
    double m_frameCount;
    double m_timeSinceLastFPSUpdate;
    double m_currentFPS;
    double m_deltaTime;
};
