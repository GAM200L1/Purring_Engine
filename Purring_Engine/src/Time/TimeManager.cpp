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
#include "prpch.h"
#include "TimeManager.h"

namespace PE
{
	TimeManager::TimeManager()
		: m_systemFrameTime{},
		  m_systemStartFrame{},
		  m_systemEndFrame{},
		  m_startFrame{},
		  m_endFrame{},
		  m_previousStartFrame{},
		  m_engineStartTime{},
		  m_engineRunTime{},
		  m_frameTime { 1.0 / 60.0 }, // default fps
		  m_deltaTime{ 1.0 / 60.0 },
		  m_durationInSeconds {}
	{}

	void TimeManager::SystemStartFrame(int system)
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

	/// <summary>
	/// This function gets the time for the start of the frame and also updates delta time between the previous frame
	/// and updates total runtime of engine
	/// </summary>
	void TimeManager::StartFrame()
	{
		// get current time
		m_startFrame = std::chrono::high_resolution_clock::now();

		// calculate delta time based on previous frame
		m_durationInSeconds = (m_startFrame - m_previousStartFrame);
		m_deltaTime = m_durationInSeconds.count();
		m_previousStartFrame = m_startFrame;

		// calculate total run time
		m_durationInSeconds = (m_startFrame - m_engineStartTime);
		m_engineRunTime = m_durationInSeconds.count();
	}

	/// <summary>
	/// This function calculates the time taken for the whole frame to complete
	/// </summary>
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