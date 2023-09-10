#pragma once

#include "../Events/EventHandler.h"
#include <vector>
#include <string>

class ImGuiWindow
{
public:

	ImGuiWindow();
	virtual ~ImGuiWindow();
	static ImGuiWindow* GetInstance();

	void Render();
	void showConsole(bool* Active);
	void showObject(bool* Active);
	void addLog(std::string text);
	void addCommand(std::string text);
	void clearLog();
	void clearConsole();

	void OnKeyEvent(const temp::Event<temp::KeyEvents>& e);

private:
	bool Active;
	bool ObjectActive;
	bool consoleLogs;
	static std::unique_ptr<ImGuiWindow> s_Instance;
	std::vector<std::string> logOutput;
	std::vector<std::string> commandOutput;
	std::string input;
};