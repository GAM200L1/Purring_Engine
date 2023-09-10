#include "prpch.h"
#include "ImGuiWindow.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

std::unique_ptr<ImGuiWindow> ImGuiWindow::s_Instance = nullptr;
static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    return 0;
}
ImGuiWindow::ImGuiWindow() {
   Active = false;
   consoleLogs = true;
   ADD_KEY_EVENT_LISTENER(temp::KeyEvents::KeyPressed, ImGuiWindow::OnKeyEvent, this)

}

ImGuiWindow::~ImGuiWindow()
{
}

ImGuiWindow* ImGuiWindow::GetInstance()
{
    if (!s_Instance)
        s_Instance = std::make_unique<ImGuiWindow>();

    return s_Instance.get();
}

void ImGuiWindow::Render()
{


    if (Active) 
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
        showConsole(&Active);
    }

    if (ObjectActive) 
    {
        showObject(&ObjectActive);
    }
}

void ImGuiWindow::showConsole(bool* Active) 
{

    if (!ImGui::Begin("Debug Console Log", Active))
    {
        ImGui::End();
    }
    else
    {
        if (consoleLogs) {
            if (ImGui::Button("clear log"))
            {
                clearLog();
                addLog("Log Cleared");
            }
            ImGui::Separator();

            //show logs
            if (ImGui::BeginChild("ScrollArea", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) {


                for (std::string i : logOutput)
                {
                    ImGui::Text(i.c_str());
                }
                if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                    ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();
            ImGui::End();
        }
        else {
            if (ImGui::Button("clear console"))
            {
                clearConsole();
                addCommand("console Cleared");
            }
            ImGui::Separator();

            float const spacing = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
            if (ImGui::BeginChild("ScrollArea", ImVec2(0, -spacing), true, ImGuiWindowFlags_HorizontalScrollbar)) {


                for (std::string i : commandOutput)
                {
                    ImGui::Text(i.c_str());
                }
                if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                    ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();

            ImGui::Separator();

            bool reclaim_focus = false;
            ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
            if (ImGui::InputText("Type stuff", &input, input_text_flags))
            {
                addCommand(input);
                input = "";
                reclaim_focus = true;
            }

            // Auto-focus on window apparition
            ImGui::SetItemDefaultFocus();
            if (reclaim_focus)
                ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
            ImGui::End();
        }
    }

}

void ImGuiWindow::showObject(bool* Active)
{
    if (!ImGui::Begin("test", Active))
    {
        ImGui::End();
    }
    else 
    {
        if (ImGui::Button("Create Object"))
        {
            addCommand("Object Created");
        }

        ImGui::End();
    }
}

void ImGuiWindow::addLog(std::string text)
{
    logOutput.push_back(text);
}

void ImGuiWindow::addCommand(std::string text)
{
    commandOutput.push_back(text);
}

void ImGuiWindow::clearLog()
{
    logOutput.clear();
}

void ImGuiWindow::clearConsole()
{
    commandOutput.clear();
}

void ImGuiWindow::OnKeyEvent(const temp::Event<temp::KeyEvents>& e)
{
    temp::KeyPressedEvent KPE;

    //dynamic cast
    if (e.GetType() == temp::KeyEvents::KeyPressed)
    {
        KPE = dynamic_cast<const temp::KeyPressedEvent&>(e);
    }

    if (KPE.keycode == GLFW_KEY_F1) 
    {
        if (Active)
            consoleLogs = !consoleLogs;
        Active = true;
    }

    if (KPE.keycode == GLFW_KEY_F2)
    {
            ObjectActive = !ObjectActive;
    }
}

