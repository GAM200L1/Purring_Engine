#include "prpch.h"
#include "CustomCursor.h"
#include "Graphics/GLHeaders.h"
#include "ResourceManager/ResourceManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Define the static variable
void* CustomCursor::customCursor = nullptr;

void CustomCursor::Init()
{
    int width, height;
    unsigned char* imageData;

    auto& resourceManager = PE::ResourceManager::GetInstance();

    if (resourceManager.LoadCursorImage("CustomCursor_32px", "../Assets/Cursor"))
    {
        auto cursorTexture = resourceManager.GetTexture("CustomCursor_32px");

        // Load the image for the cursor
        GLFWimage cursorImage;
        cursorImage.width = cursorTexture->GetWidth();
        cursorImage.height = cursorTexture->GetHeight();
        cursorImage.pixels = stbi_load_from_memory(static_cast<stbi_uc const*>(cursorTexture->GetRawData()), cursorTexture->GetDataSize(), &width, &height, nullptr, 4);

        if (cursorImage.pixels)
        {
            customCursor = glfwCreateCursor(&cursorImage, 0, 0);

            // Free the image data after creating the cursor
            stbi_image_free(cursorImage.pixels);
        }
        else
        {
            // Handle the errororput default fcursonr?
        }
    }
}

void CustomCursor::SetCustomCursor()
{
    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetCursor(window, static_cast<GLFWcursor*>(customCursor));
}

void CustomCursor::RevertToDefaultCursor()
{
    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetCursor(window, nullptr);
}

unsigned char* LoadCursorImage(const char* filename, int* width, int* height)
{
    // Flip the image's y-axis to match OpenGL's texture coordinate system
    stbi_set_flip_vertically_on_load(true);

    // Load the image data
    int channels;
    unsigned char* data = stbi_load(filename, width, height, &channels, 4); // Force RGBA format
    if (data == nullptr)
    {
        std::cerr << "Failed to load cursor image: " << filename << std::endl;
    }
    return data;
}