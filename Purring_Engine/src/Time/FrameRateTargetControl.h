/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FrameRateTargetControl.h
 \creation date:       28-08-2023
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
    class FrameRateController
    {
    public:
        /*!***********************************************************************************
         \brief     Constructor for initializing frame rate target variables.
        *************************************************************************************/
        FrameRateController();

        /*!***********************************************************************************
         \brief     Set the target frames per second.
         \param     fps The desired FPS.
        *************************************************************************************/
        void SetTargetFPS(unsigned int fps);

        void SetMaxFpsHistory(unsigned int count);

        /*!***********************************************************************************
         \brief     Ends the current frame and updates FPS counter.
        *************************************************************************************/
        void EndFrame();
        
        /*!***********************************************************************************
         \brief     Gets the current frames per second.
         \return    double Current FPS.
        *************************************************************************************/
        inline float GetFps() const { return m_currentFps; }

        inline float GetMaxFps() const { return m_maxFps; }

        inline float GetMinFps() const { return m_minFps; }

        inline float GetAverageFps() const { return m_averageFps; }

        inline std::vector<float> const& GetFpsValues() const { return m_fpsValues; }

        /*!***********************************************************************************
         \brief     Update the target FPS based on a key input.
         \param     key The input key.
        *************************************************************************************/
        void UpdateTargetFPSBasedOnKey(int key);

    private:
        float m_targetFrameTime;
        float m_frameCount;
        float m_timeSinceLastFpsUpdate;
        float m_currentFps;
        
        std::vector<float> m_fpsValues;
        unsigned int m_maxFpsHistory;
        float m_averageFps;
        float m_maxFps;
        float m_minFps;
    };
}
