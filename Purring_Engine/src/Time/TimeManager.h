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

#include "Singleton.h"
#include <chrono>
#include <set>
#include <array>

namespace PE
{
	constexpr auto TotalSystems = 4; // hardcoded?
	enum SystemType
	{
		INPUT = 0,
		PHYSICS,
		COLLISION,
		GRAPHICS,
		SYSTEMCOUNT
	};

	class TimeManager : public Singleton<TimeManager> {
	public:
		friend class Singleton<TimeManager>;
		// system
		void SystemStartFrame();
		void SystemEndFrame(int system);

		// global
		void StartFrame();
		void EndFrame();

		void EngineStart();

		float GetFrameTime() const { return m_frameTime; }
		float GetDeltaTime() const { return m_deltaTime; }
		float GetRunTime() const { return m_engineRunTime; }
		std::chrono::high_resolution_clock::time_point const& GetStartTime() { return m_startFrame; }

		float GetSystemFrameTime(int system) const { return m_systemFrameTime[system]; }

	private:
		TimeManager();

	private:
		// system time
		std::array<float, TotalSystems> m_systemFrameTime; // stores all system frame time, may change to vector
		std::chrono::high_resolution_clock::time_point m_systemStartFrame, m_systemEndFrame; // system time

		// global time
		std::chrono::high_resolution_clock::time_point m_startFrame, m_endFrame, m_previousStartFrame, m_engineStartTime;
		float m_engineRunTime;
		float m_frameTime; // total frame time
		float m_deltaTime; // time between last and current frame

		// holds duration in seconds
		std::chrono::duration<float> m_durationInSeconds;
	};
}