#pragma once
#define GLFW_INCLUDE_NONE

#include "CoreApplication.h"

extern PE::CoreApplication* PE::CreateApplication();

int main(int argc, char** argv) {
	
	auto app = PE::CreateApplication();
	app->InitSystems();
	app->Run();
	app->DestroySystems();
}