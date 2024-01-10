/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GraphicsUtilities.cpp
 \date:    18-12-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief    This file contains helper functions for graphics, such as converting
           PE::vec* types to glm::vec* types, and generating transformation matrices.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"

#include "GraphicsUtilities.h"

namespace PE
{
    namespace Graphics
    {

        void RetrieveSpecifications()
        {
            // Retrieve the maximum number of vertices and indices the GPU can handle
            glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVertexCount);
            glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndicesCount);

            // Get maximum number of texture binding points
            glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureBindings);

            // Get maximum texture size
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        }

        void PrintSpecifications()
        {
            // Declare variables to store specs info
            GLint majorVersion, minorVersion, maxViewportDims[2];
            GLboolean isdoubleBuffered;

            // Get and store values
            glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
            glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
            glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewportDims);
            glGetBooleanv(GL_DOUBLEBUFFER, &isdoubleBuffered);

            // Print out specs
            std::cout << "GPU Vendor: " << glGetString(GL_VENDOR)
                << "\nGL Renderer: " << glGetString(GL_RENDERER)
                << "\nGL Version: " << glGetString(GL_VERSION)
                << "\nGL Shader Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
                << "\nGL Major Version: " << majorVersion
                << "\nGL Minor Version: " << minorVersion
                << "\nCurrent OpenGL Context is " << (isdoubleBuffered ? "double buffered" : "single buffered")
                << "\nMaximum Vertex Count: " << maxVertexCount
                << "\nMaximum Indices Count: " << maxIndicesCount
                << "\nMaximum texture binding points: " << maxTextureBindings
                << "\nGL Maximum texture size: " << maxTextureSize
                << "\nMaximum Viewport Dimensions: " << maxViewportDims[0] << " x " << maxViewportDims[1] << "\n" << std::endl;
        }


        void APIENTRY GLDebugOutput(GLenum source, GLenum type, unsigned int id,
            GLenum severity, GLsizei, const char* message, const void*)
        {
            // ignore non-significant error/warning codes
            if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

            std::cout << "---------------\n";
            std::cout << "Debug message (" << id << "): " << message << "\n";

            switch (source)
            {
            case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
            case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
            case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
            }

            std::cout << "\n";

            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
            case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
            case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
            case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
            case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
            }

            std::cout << "\n";

            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
            case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
            case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
            }
            std::cout << "\n---------------\n" << std::endl;
        }


        glm::mat4 GenerateTransformMatrix(float const width, float const height,
            float const orientation, float const positionX, float const positionY) 
        {
            // Get rotation
            GLfloat sin_angle{ glm::sin(orientation) };
            GLfloat cos_angle{ glm::cos(orientation) };

            return glm::mat4{
                width * cos_angle, width * sin_angle, 0.f, 0.f,
                height * -sin_angle, height * cos_angle, 0.f, 0.f,
                0.f,        0.f,       1.f, 0.f,
                positionX, positionY, 0.f, 1.f
            };
        }


        glm::mat4 GenerateTransformMatrix(glm::vec2 const& rightVector,
            glm::vec2 const& upVector, glm::vec2 const& centerPosition)
        {
            return glm::mat4{
                rightVector.x, rightVector.y, 0.f,    0.f,
                upVector.x,   upVector.y,   0.f,    0.f,
                0.f,    0.f,    1.f,    0.f,
                centerPosition.x, centerPosition.y, 0.f, 1.f
            };
        }


        glm::mat4 GenerateInverseTransformMatrix(float const width, float const height,
            float const orientation, float const positionX, float const positionY)
        {
            glm::vec2 up{ -glm::sin(orientation), glm::cos(orientation) };
            glm::vec2 right{ up.y, -up.x };
            up *= 1.f / height, right *= 1.f / width;

            float upDotPosition{ up.x * positionX + up.y * positionY };
            float rightDotPosition{ right.x * positionX + right.y * positionY };

            // Return the inverse transform matrix (world to model)
            return glm::mat4{
                right.x, up.x, 0.f, 0.f,
                right.y, up.y, 0.f, 0.f,
                0.f,    0.f,   1.f, 0.f,
                -rightDotPosition, -upDotPosition, 0.f, 1.f
            };
        }
    } // End of Graphics namespace
} // End of PE namespace