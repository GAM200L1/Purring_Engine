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
	/*!***********************************************************************************
	 \brief     Constructs a TimeManager object and initializes member variables.

	 \tparam T  This function does not use a template.
	 \return    The constructor initializes the object's frame time to a default value based on 60 FPS.
	*************************************************************************************/
	TimeManager::TimeManager()
	{
		// might need to init the other variables
		m_frameTime = 1.f / 60.f; // default fps
	}



	/*!***********************************************************************************
	 \brief     Records the system time at the start of the frame.

	 \tparam T  This function does not use a template.
	 \return    void  This function does not return a value but sets the m_systemStartFrame member variable to the current system time.
	*************************************************************************************/
	void TimeManager::SystemStartFrame()
	{
		m_systemStartFrame = std::chrono::high_resolution_clock::now();
	}
	

	/*!***********************************************************************************
	 \brief     Records the system time at the end of the frame and calculates the frame time for a specific system.

	 \tparam T  This function does not use a template.
	 \param[in] system           The identifier for the specific system whose frame time is being calculated.
	 \return    void             This function does not return a value but updates the m_systemEndFrame and m_systemFrameTime member variables.
	*************************************************************************************/
	void TimeManager::SystemEndFrame(int system)
	{
		m_systemEndFrame = std::chrono::high_resolution_clock::now();

		// frame time for each system
		m_durationInSeconds = (m_systemEndFrame - m_systemStartFrame);
		m_systemFrameTime[system] = m_durationInSeconds.count();
	}



	/*!***********************************************************************************
	 \brief     Records the time at the start of a new frame and calculates delta time and engine run time.

	 \tparam T  This function does not use a template.
	 \return    void  This function does not return a value but updates various time-related member variables including delta time and total engine run time.
	*************************************************************************************/
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



	/*!***********************************************************************************
	 \brief     Records the time at the end of the current frame and calculates the total frame time.

	 \tparam T  This function does not use a template.
	 \return    void  This function does not return a value but updates the total frame time by calculating the duration between the start and end of the frame.
	*************************************************************************************/
	void TimeManager::EndFrame()
	{
		// get current time
		m_endFrame = std::chrono::high_resolution_clock::now();

		// get total time for frame
		m_durationInSeconds = (m_endFrame - m_startFrame);
		m_frameTime = m_durationInSeconds.count();
	}



	/*!***********************************************************************************
	 \brief     Records the starting time of the engine.

	 \tparam T  This function does not use a template.
	 \return    void  This function does not return a value but initializes the engine's starting time by capturing the current high-resolution time.
	*************************************************************************************/
	void TimeManager::EngineStart()
	{
		m_engineStartTime = std::chrono::high_resolution_clock::now();
	}

}
