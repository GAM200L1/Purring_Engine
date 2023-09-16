#pragma once
#define GLFW_INCLUDE_NONE

#include "CoreApplication.h"

// detect memory leaks
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

extern PE::CoreApplication* PE::CreateApplication();

int main(int argc, char** argv) {
	auto app = PE::CreateApplication();
	app->InitSystems();
	app->Run();
	app->DestroySystems();

	// detect memory leaks
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();
}