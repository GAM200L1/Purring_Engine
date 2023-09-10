#include "prpch.h"
#include "EventHandler.h"

namespace temp
{
	std::unique_ptr<EventHandler> EventHandler::s_Instance = nullptr;

	EventHandler* EventHandler::GetInstance() 
	{
		if (!s_Instance)
			s_Instance = std::make_unique<EventHandler>();

		return s_Instance.get();
	}
}