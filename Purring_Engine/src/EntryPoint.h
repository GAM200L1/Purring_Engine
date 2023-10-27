/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EntryPoint.h
 \date     16-09-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief	   This file serves as the entry point for the Purring Engine application.
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
#include <crtdbg.h>


extern PE::CoreApplication* PE::CreateApplication();

int main(int argc, char** argv)
{
  argc; argv;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	auto app = PE::CreateApplication();
	app->Initialize();
	app->Run();
	app->DestroySystems();
	delete app;

	// detect memory leaks
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
}
