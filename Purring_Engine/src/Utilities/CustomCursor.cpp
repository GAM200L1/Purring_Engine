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
        }
    }

    void CustomCursor::Show()
    {
        if (customCursor)
        {
            GLFWwindow* window = glfwGetCurrentContext();
            glfwSetCursor(window, customCursor);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);  // Hide the system cursor
            std::cout << "[CustomCursor::Show] Custom cursor set and system cursor hidden for window: " << window << std::endl;
        }
        else
        {
            std::cerr << "[CustomCursor::Show] Error: Custom cursor not initialized or NULL." << std::endl;
        }
    }


    void CustomCursor::Hide()
    {
        GLFWwindow* window = glfwGetCurrentContext();
        glfwSetCursor(window, NULL);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  // Show the system cursor
        std::cout << "[CustomCursor::Hide] Reverted to system cursor for window: " << window << std::endl;
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
