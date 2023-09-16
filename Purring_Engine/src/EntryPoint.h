/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EntryPoint.h
 \creation date:       To check
 \last updated:        16-09-2023
 \author:              Brandon HO Jun Jie

 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file serves as the entry point for the Purring Engine application.
		   It includes the main function that creates an instance of CoreApplication,
		   initializes its systems, runs the application, and finally cleans up resources.
		   It also includes memory leak detection utilities.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#pragma once
#define GLFW_INCLUDE_NONE

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "CoreApplication.h"

// Detect memory leaks
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

extern PE::CoreApplication* PE::CreateApplication();

/*-----------------------------------------------------------------------------
/// <summary>
/// The entry point of the application. Responsible for creating the CoreApplication
/// instance, initializing its systems, running the main game loop, and finally
/// destroying the systems before application termination. It also includes diagnostics
/// to detect memory leaks.
/// </summary>
----------------------------------------------------------------------------- */
int main(int argc, char** argv)
{
	auto app = PE::CreateApplication();			// Create CoreApplication instance
	app->InitSystems();							// Initialize all registered systems
	app->Run();									// Execute the main game loop
	app->DestroySystems();						// Cleanup and destroy systems

	// Detect memory leaks
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();
}
