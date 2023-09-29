/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     TimeManager.cpp
 \date     18-09-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the TimeManager class that handles all time related
		   functionalities for the engine.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "TimeManager.h"

namespace PE
{
	TimeManager::TimeManager()
	{
		// might need to init the other variables
		m_frameTime = 1.f / 60.f; // default fps
	}
	void TimeManager::SystemStartFrame()
	{
		m_systemStartFrame = std::chrono::high_resolution_clock::now();
	}
	
	void TimeManager::SystemEndFrame(int system)
	{
		m_systemEndFrame = std::chrono::high_resolution_clock::now();

		// frame time for each system
		m_durationInSeconds = (m_systemEndFrame - m_systemStartFrame);
		m_systemFrameTime[system] = m_durationInSeconds.count();
	}

	void TimeManager::StartFrame()
	{
		// get current time
		m_startFrame = std::chrono::high_resolution_clock::now();

		// calculate delta time based on previous frame
		m_durationInSeconds = (m_startFrame - m_previousStartFrame);
		m_deltaTime = m_deltaTime == 0.f ? 1.f / 60.f : m_durationInSeconds.count();
		m_previousStartFrame = m_startFrame;

		// calculate total run time
		m_durationInSeconds = (m_startFrame - m_engineStartTime);
		m_engineRunTime = m_durationInSeconds.count();
	}

	void TimeManager::EndFrame()
	{
		// get current time
		m_endFrame = std::chrono::high_resolution_clock::now();

		// get total time for frame
		m_durationInSeconds = (m_endFrame - m_startFrame);
		m_frameTime = m_durationInSeconds.count();
	}

	void TimeManager::EngineStart()
	{
		m_engineStartTime = std::chrono::high_resolution_clock::now();
	}
}
