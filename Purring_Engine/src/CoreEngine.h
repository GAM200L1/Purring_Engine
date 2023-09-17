#pragma once

#include "System.h"
#include <vector>

namespace PE
{
	// Core engine that manages every system in the application.
	class CoreEngine
	{
	public:
		CoreEngine();
		~CoreEngine();

		void Run();

		void AddSystem(System* system);

		void Initialize();

	private:

		// holds all the systems in the engine
		std::vector<System*> m_systemList;

		// is engine running
		bool m_Running{};
		double m_lastFrameTime{};

	};
}