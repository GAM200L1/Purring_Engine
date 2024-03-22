#include "prpch.h"
#include "CustomCursor.h"
#include <Windows.h>
#include <direct.h>
#include "WindowManager.h"


namespace PE
{
    void CustomCursor::Initialize()
    {
        const char* windowTitle = WindowManager::GetInstance().GetWindowTitle();

        HWND hWnd = FindWindow(nullptr, TEXT(windowTitle));

        HCURSOR hCursor = LoadCursorFromFile(TEXT("../Assets/Cursor/CustomCursor_32px.cur"));

        // debug
        if (!hCursor)
        {
            std::cerr << "Failed to load custom cursor from file." << std::endl;
        }
        else
        {
            std::cout << "Custom cursor loaded successfully." << std::endl;
        }

        if (hCursor && hWnd)
        {
            SetClassLongPtr(hWnd, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(hCursor));
            std::cout << "Custom cursor working." << std::endl;
        }
        else
        {
            std::cerr << "Failed to set custom cursor." << std::endl;
            if (!hWnd)
            {
                std::cerr << "Main window handle not found." << std::endl;
            }
        }

    }

    void CustomCursor::Cleanup()
    {
        ///empty
    }

}