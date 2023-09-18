#pragma once
#define GLFW_INCLUDE_NONE

#include "CoreApplication.h"

extern PE::CoreApplication* PE::CreateApplication();

int main(int argc, char** argv) {
  argc; argv;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	auto app = PE::CreateApplication();
	app->InitSystems();
	app->Run();
	app->DestroySystems();
	delete app;

	// detect memory leaks
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
}