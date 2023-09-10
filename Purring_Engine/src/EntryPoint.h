#pragma once
//#define GLEW_STATIC

#include "CoreApplication.h"

extern PE::CoreApplication* PE::CreateApplication();

int main(int argc, char** argv) {
	
	auto app = PE::CreateApplication();
	app->Run();
	delete app;
}