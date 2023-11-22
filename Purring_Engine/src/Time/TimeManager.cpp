/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     TimeManager.cpp
 \date     28-10-2023

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
		m_accumulator = 0.f;
		m_accumulatorLimit = 1.f;
		m_fixedTimeStep = 1.f / 60.f;
	}
	void TimeManager::SystemStartFrame(SystemID system)
	{
		m_systemStartFrame[system] = std::chrono::high_resolution_clock::now();
	}
	
	void TimeManager::SystemEndFrame(SystemID system)
	{
		// frame time for each system
		m_durationInSeconds = (std::chrono::high_resolution_clock::now() - m_systemStartFrame[system]);

		if (system != GRAPHICS)
		{
			m_systemAccumulatedFrameTime[system] += m_durationInSeconds.count();
		}
		else // Graphics not using fixed time step
		{
			m_systemFrameTime[system] += m_durationInSeconds.count();
		}
	}

	void TimeManager::UpdateSystemFrameUsage()
	{
		// update system frame usage
		for (SystemID systemID{}; systemID < SYSTEMCOUNT; ++systemID)
		{
			m_systemFrameUsage[systemID] = m_systemFrameTime[systemID] / m_frameTime;

			//if ((m_systemPreviousFrameTime[systemID] / m_frameTime) > 1.f)
			//{
			//	m_systemPreviousFrameTime[systemID] = m_frameTime;
			//}
			m_systemFrameTime[systemID] = 0.f;
		}
		m_frameTime = 0.f;
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
		m_frameTime += m_durationInSeconds.count();
	}

	void TimeManager::EngineStart()
	{
		m_engineStartTime = std::chrono::high_resolution_clock::now();
	}

	void TimeManager::StartAccumulator()
	{
		m_accumulator += m_deltaTime;
		m_accumulator = (m_accumulator > m_accumulatorLimit) ? m_accumulatorLimit : m_accumulator;
	}

	bool TimeManager::UpdateAccumulator()
	{
		return m_accumulator >= m_fixedTimeStep;
	}

	void TimeManager::EndAccumulator()
	{
		m_accumulator -= m_fixedTimeStep;

		// if accumulator last loop iteration
		if (!(m_accumulator >= m_fixedTimeStep))
		{
			// save systemFrametime for fixed time systems and reset
			for (SystemID systemID{}; systemID < GRAPHICS; ++systemID)
			{
				m_systemFrameTime[systemID] += m_systemAccumulatedFrameTime[systemID];
				m_systemAccumulatedFrameTime[systemID] = 0.f;
			}
		}
	}

	void TimeManager::SetAccumulatorLimit(float value)
	{
		m_accumulatorLimit = value;
	}

	void TimeManager::SetFixedTimeStep(float value)
	{
		m_fixedTimeStep = value;
	}
}
