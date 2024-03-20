#include "prpch.h"
#include "CustomCursor.h"
#include "Graphics/GLHeaders.h"
#include "ResourceManager/ResourceManager.h"
#include "stb_image.h"

namespace PE
{
    GLFWcursor* customCursor = nullptr;

    void CustomCursor::Initialize()
    {
        int width, height;
        GLFWimage cursorImg = ResourceManager::LoadImageFromFile("../Assets/Cursor/CustomCursor_32px.png", &width, &height);

        if (!cursorImg.pixels)
        {
            std::cerr << "Failed to load cursor image." << std::endl;
            return;
        }

        // create the cursor
        customCursor = glfwCreateCursor(&cursorImg, 0, 0);

        stbi_image_free(cursorImg.pixels);

        if (!customCursor)
        {
            std::cerr << "Failed to create custom cursor." << std::endl;
            return;
        }

        GLFWwindow* window = glfwGetCurrentContext();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);  // Hide the system cursor
    }

    void CustomCursor::SetVisible(bool visible)
    {
        GLFWwindow* window = glfwGetCurrentContext();
        if (!window)
        {
            std::cerr << "[CustomCursor::SetVisible] Error: No current GLFW window." << std::endl;
            return;
        }

        if (visible)
        {
            if (customCursor)
            {
                glfwSetCursor(window, customCursor);
                std::cout << "[CustomCursor::SetVisible] Custom cursor set for window: " << window << std::endl;
            }
            else
            {
                std::cerr << "[CustomCursor::SetVisible] Error: Custom cursor not initialized or NULL." << std::endl;
            }
        }
        else
        {
            glfwSetCursor(window, NULL);
            std::cout << "[CustomCursor::SetVisible] Reverted to system cursor for window: " << window << std::endl;
        }
    }

    // Clean up the custom cursor
    void CustomCursor::Cleanup()
    {
        if (customCursor)
        {
            glfwDestroyCursor(customCursor);    // Destroy the custom cursor
            customCursor = nullptr;             // Reset the pointer to nullptr
        }
    }
}
