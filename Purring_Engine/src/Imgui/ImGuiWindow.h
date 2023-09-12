#pragma once

#include "../Events/EventHandler.h"
#include <vector>
#include <string>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>
#include <iostream>


	class ImGuiWindow 
	{
	public:
		ImGuiWindow();
		virtual ~ImGuiWindow();
		static ImGuiWindow* GetInstance();

		void Init();
		void Render();
		void showLogs(bool* Active);
		void showConsole(bool* Active);
		void showObject(bool* Active);
		void setDockingPort();
		void addLog(std::string text);
		void addCommand(std::string text);
		void clearLog();
		void clearConsole();

		void OnKeyEvent(const temp::Event<temp::KeyEvents>& e);

	private:
		bool logsActive;
		bool ObjectActive;
		bool consoleActive;
		static std::unique_ptr<ImGuiWindow> s_Instance;
		std::vector<std::string> logOutput;
		std::vector<std::string> consoleOutput;
		std::string input;
		ImVec2 imGuiWindowSize;
		int currentSelectedIndex;
		std::vector<std::string> items;
	};