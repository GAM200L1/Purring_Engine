/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     TimeManager.h
 \date     28-10-2023

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
#include "FrameRateTargetControl.h"
#include "System.h"


namespace PE
{
	inline SystemID& operator++(SystemID& systemID) {
		systemID = static_cast<SystemID>(systemID != SystemID::SYSTEMCOUNT ? static_cast<int>(systemID) + 1 : 0);
		return systemID;
	}

	inline SystemID operator++(SystemID& systemID, int) {
		SystemID result = systemID;
		++systemID;
		return result;
	}

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
		void SystemStartFrame(SystemID system);

		/*!***********************************************************************************
		 \brief Conclude the frame time for a particular subsystem.
		 \param[in] system The subsystem to conclude frame time for.
		*************************************************************************************/
		void SystemEndFrame(SystemID system);

		void UpdateSystemFrameUsage();

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

		/*!***********************************************************************************
		 \brief Start fixed time accumulator
		*************************************************************************************/
		void StartAccumulator();

		/*!***********************************************************************************
		 \brief Update fixed time accumulator
		*************************************************************************************/
		bool UpdateAccumulator();

		/*!***********************************************************************************
		 \brief End fixed time accumulator
		*************************************************************************************/
		void EndAccumulator();

		/*!***********************************************************************************
		 \brief Set accumulator limit

		 \param[in] value Value to set accumulator limit
		*************************************************************************************/
		void SetAccumulatorLimit(float value);

		/*!***********************************************************************************
		 \brief Set fixed time step

		 \param[in] value Value to set fixed time step
		*************************************************************************************/
		void SetFixedTimeStep(float value);

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
		 \brief Get the fixed time step

		 \return fixed time step
		*************************************************************************************/
		float GetFixedTimeStep() const { return m_fixedTimeStep; }

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
		 \brief Get the frame usage for a specific subsystem.

		 \param[in] system The subsystem identifier.

		 \return The frame usage for the specified subsystem in seconds.
		*************************************************************************************/
		float GetSystemFrameUsage(SystemID system) const { return m_systemFrameUsage[system]; }

		FrameRateController m_frameRateController;
		// ----- Private Methods and Members ----- //
	private:
		/*!***********************************************************************************
		 \brief Private constructor to enforce Singleton pattern.
		*************************************************************************************/
		TimeManager();


		// ----- Private Variables ----- //
	private:
		// system time
		std::array<std::chrono::high_resolution_clock::time_point, TotalSystems> m_systemStartFrame;
		std::array<float, TotalSystems> m_systemFrameTime;					// stores all system frame time
		std::array<float, TotalSystems> m_systemAccumulatedFrameTime;		// stores all accumulated system frame time
		std::array<float, TotalSystems> m_systemFrameUsage;					// stores all system frame usage
		//std::chrono::high_resolution_clock::time_point m_systemStartFrame, m_systemEndFrame;	// system time

		// global time
		std::chrono::high_resolution_clock::time_point m_startFrame, m_endFrame, m_previousStartFrame, m_engineStartTime;
		float m_engineRunTime;
		float m_frameTime;										// total frame time
		float m_deltaTime;										// time between last and current frame
		float m_accumulator;
		float m_accumulatorLimit;
		float m_fixedTimeStep;

		// holds duration in seconds
		std::chrono::duration<float> m_durationInSeconds;
	};

}
