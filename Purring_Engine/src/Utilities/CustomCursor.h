#pragma once
#include "Graphics/GLHeaders.h"

namespace PE
{
    class CustomCursor
    {
    public:
        static void Initialize();
        static void Show();
        static void SetVisible(bool visible);

        static void Hide();
        static void Cleanup();

        //static GLFWimage cursorImg;
    };
}
