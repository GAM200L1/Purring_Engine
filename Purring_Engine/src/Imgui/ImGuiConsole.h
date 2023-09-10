#pragma once

#include "../Events/EventHandler.h"
#include <vector>
#include <string>

class ImGuiConsole
{
public:

	ImGuiConsole();
	~ImGuiConsole();
	static ImGuiConsole* GetInstance();

	void Render();
	void showConsole(bool* Active);
	void addLog(std::string text);
	void addCommand(std::string text);
	void clearLog();
	void clearConsole();

	void OnKeyEvent(const temp::Event<temp::KeyEvents>& e);

private:
	bool Active;
	bool consoleLogs;
	static std::unique_ptr<ImGuiConsole> s_Instance;
	std::vector<std::string> logOutput;
	std::vector<std::string> commandOutput;
	std::string input;
};