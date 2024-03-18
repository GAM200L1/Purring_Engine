#pragma once

class CustomCursor
{
public:
    static void Init();

    static void SetCustomCursor();

    static void RevertToDefaultCursor();

private:
    // For GLFW, it would be GLFWcursor* I need to change later
    static void* customCursor;
};
