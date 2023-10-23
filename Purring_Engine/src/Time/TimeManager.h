/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     TimeManager.h
 \date     18-09-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the TimeManager class that handles all time related
		   functionalities for the engine.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Singleton.h"
#include <chrono>
#include <set>
#include <array>

namespace PE
{
	constexpr auto TotalSystems = 7;
	/*!***********************************************************************************
	 \brief Enumeration for identifying different subsystems.
	*************************************************************************************/
	enum SystemType
	{
		LOGIC = 0,
		INPUT,
		PHYSICS,
		COLLISION,
		GRAPHICS,
		SYSTEMCOUNT
	};

	/*!***********************************************************************************
	 \brief A Singleton class for managing time in the engine.
	*************************************************************************************/
	class TimeManager : public Singleton<TimeManager>
	{

		// ----- Public Methods ----- //
	public:
		friend class Singleton<TimeManager>;

		/*!***********************************************************************************
		 \brief Initialize the frame time for a particular subsystem.
		*************************************************************************************/
		void SystemStartFrame();

		/*!***********************************************************************************
		 \brief Conclude the frame time for a particular subsystem.
		 \param[in] system The subsystem to conclude frame time for.
		*************************************************************************************/
		void SystemEndFrame(int system);

		/*!***********************************************************************************
		 \brief Begin global frame timing.
		*************************************************************************************/
		void StartFrame();

		/*!***********************************************************************************
		 \brief End global frame timing.
		*************************************************************************************/
		void EndFrame();

		/*!***********************************************************************************
		 \brief Initialize the engine's start time.
		*************************************************************************************/
		void EngineStart();


		// ----- Public Getters ----- //
	public:
		/*!***********************************************************************************
		 \brief Get the frame time for the last completed frame.
		 \return The last frame time in seconds.
		*************************************************************************************/
		float GetFrameTime() const { return m_frameTime; }

		/*!***********************************************************************************
		 \brief Get the time passed since the last frame.
		 \return The delta time in seconds.
		*************************************************************************************/
		float GetDeltaTime() const { return m_deltaTime; }

		/*!***********************************************************************************
		 \brief Get the total run time of the engine.
		 \return The run time in seconds.
		*************************************************************************************/
		float GetRunTime() const { return m_engineRunTime; }

		/*!***********************************************************************************
		 \brief Get the starting time point of the frame.
		 \return The high-resolution time point when the frame started.
		*************************************************************************************/
		std::chrono::high_resolution_clock::time_point const& GetStartTime() { return m_startFrame; }

		/*!***********************************************************************************
		 \brief Get the frame time for a specific subsystem.
		 \param[in] system The subsystem identifier.
		 \return The frame time for the specified subsystem in seconds.
		*************************************************************************************/
		float GetSystemFrameTime(int system) const { return m_systemFrameTime[system]; }

		// ----- Private Methods and Members ----- //
	private:
		/*!***********************************************************************************
		 \brief Private constructor to enforce Singleton pattern.
		*************************************************************************************/
		TimeManager();


		// ----- Private Variables ----- //
	private:
		// system time
		std::array<float, TotalSystems> m_systemFrameTime;		// stores all system frame time, may change to vector
		std::chrono::high_resolution_clock::time_point m_systemStartFrame, m_systemEndFrame;	// system time

		// global time
		std::chrono::high_resolution_clock::time_point m_startFrame, m_endFrame, m_previousStartFrame, m_engineStartTime;
		float m_engineRunTime;
		float m_frameTime;										// total frame time
		float m_deltaTime;										// time between last and current frame

		// holds duration in seconds
		std::chrono::duration<float> m_durationInSeconds;
	};

}
