#include "prpch.h"

#include "CoreEngine.h"

PE::CoreEngine::CoreEngine() : m_Running{false}, m_lastFrameTime{0.0}
{

}

PE::CoreEngine::~CoreEngine()
{

}

void PE::CoreEngine::Initialize()
{

}

void PE::CoreEngine::AddSystem(System* system)
{
		system;
}

void PE::CoreEngine::Run()
{
	// main app loop

	while (m_Running)
	{

	}
}
