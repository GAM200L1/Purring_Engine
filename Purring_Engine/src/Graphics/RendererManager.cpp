/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     RendererManager.cpp
 \date     20-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the RendererManager class, which manages 
           the render passes and includes helper functions to draw debug shapes.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "prpch.h"

#include <glm/gtc/constants.hpp>    // pi()
#include <glm/gtc/matrix_transform.hpp> // ortho()

#include "Logging/Logger.h" 
#include "RendererManager.h"

#include "Imgui/ImGuiWindow.h"

#include "Imgui/ImGuiWindow.h"

extern Logger engine_logger;

namespace PE
{
    namespace Graphics
    {
        RendererManager::RendererManager(GLFWwindow* p_window) 
        {
            // Initialize GLEW
            if (glewInit() != GLEW_OK)
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(true, "Failed to initialize GLEW.", __FUNCTION__);
                throw;
            }

            p_windowRef = p_window;

            int width, height;
            glfwGetWindowSize(p_windowRef, &width, &height);
            create_framebuffer(width, height);

            ImGuiWindow::GetInstance()->Init(p_window);
        }
        
        void RendererManager::InitializeSystem()
        {
            // Print the specs
            PrintSpecifications();

            // Create the framebuffer to render to ImGui window
            int width, height;
            glfwGetWindowSize(p_windowRef, &width, &height);
            CreateFrameBuffer(width, height);

            ImGuiWindow::GetInstance()->Init(p_windowRef);

            // Initialize the base meshes to use
            InitializeTriangleMesh(m_meshes["triangle"]);
            InitializeQuadMesh(m_meshes["quad"]);

            // Create a shader program
            // Store the vert shader
            const std::string vertexShaderString{ R"(            
            #version 460 core

            layout (location = 0) in vec2 aVertexPosition;
            layout (location = 1) in vec3 aVertexColor;

            layout (location = 0) out vec3 vColor;

            uniform mat4 uModelToNdc;

            void main(void) {
                gl_Position = uModelToNdc * vec4(aVertexPosition, 0.0, 1.0);
                vColor = aVertexColor;
            }
        )" };

            // Store the fragment shader
            const std::string fragmentShaderString{ R"( 
            #version 460 core

            layout(location = 0) in vec3 vColor;

            layout(location = 0) out vec4 fFragColor;

            void main(void) {
                fFragColor = vec4(vColor, 1.0);
            }
        )" };

            m_shaderPrograms["basic"] = new ShaderProgram{};
            m_shaderPrograms["basic"]->CompileLinkValidateProgram(vertexShaderString, fragmentShaderString);


            // Add a triangle and quad as renderable objects
            m_renderableObjects.emplace_back(Renderer{});
            m_renderableObjects.emplace_back(Renderer{});

            m_renderableObjects[1].meshName = "triangle";
            m_renderableObjects[1].transform.width = 100.f;
            m_renderableObjects[1].transform.height = 400.f;
            m_renderableObjects[1].transform.orientation = 45.f;
            m_renderableObjects[1].transform.position = glm::vec2{ 200.f, 400.f };


            engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
            engine_logger.SetTime();
            engine_logger.AddLog(false, "RendererManager initialized!", __FUNCTION__);
        }


        void RendererManager::UpdateSystem(float deltaTime)
        {
            if (!p_windowRef)
            {
                return;
            }
            deltaTime; // Prevent warnings

            // Get the size of the ImGui window to render in
            float windowWidth{}, windowHeight{};
            ImGuiWindow::GetInstance()->GetWindowSize(windowWidth, windowHeight);

            GLsizei const windowWidthInt{ static_cast<GLsizei>(windowWidth) };
            GLsizei const windowHeightInt{ static_cast<GLsizei>(windowHeight) };
            ResizeFrameBuffer(windowWidthInt, windowHeightInt);
            glViewport(0, 0, windowWidthInt, windowHeightInt);

            // Set background color to black
            //glClearColor(1.f, 1.f, 1.f, 1.f);
            //glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

            // Bind the RBO for rendering to the ImGui window
            BindFrameBuffer();

            // Set the background color of the ImGui window to white
            glClearColor(1.f, 1.f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

            // Draw game objects in the scene
            DrawScene(windowWidth, windowHeight);

            // Unbind the RBO for rendering to the ImGui window
            UnbindFrameBuffer();

            ImGuiWindow::GetInstance()->Render(m_imguiTextureId);

            // Poll for and process events
            glfwPollEvents(); // should be called before glfwSwapbuffers

            // Swap front and back buffers
            glfwSwapBuffers(p_windowRef);
        }


        void RendererManager::DestroySystem()
        {
            // Release the buffer objects in each mesh
            for (auto& mesh : m_meshes) {
                mesh.second.Cleanup();
            }
            m_meshes.clear();

            // Delete the shader programs
            for (auto& shaderProgram : m_shaderPrograms) {
                shaderProgram.second->DeleteProgram();
                delete shaderProgram.second;
            }
            m_shaderPrograms.clear();

            // Delete the framebuffer object
            glDeleteTextures(1, &m_imguiTextureId);
            glDeleteFramebuffers(1, &m_frameBufferObjectIndex);
        }


        void RendererManager::DrawScene(float const width, float const height)
        {
            float halfWidth{ width * 0.5f };
            float halfHeight{ height * 0.5f };
            glm::mat4 viewToNdc{
                glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f)
            };

            // Call glDrawElements for each renderable object
            for (auto& renderable : m_renderableObjects) {
                m_shaderPrograms[renderable.shaderProgramName]->Use();

                m_meshes[renderable.meshName].BindMesh();

                m_shaderPrograms[renderable.shaderProgramName]->SetUniform(
                    "uModelToNdc",
                    viewToNdc * m_mainCamera.GetWorldToViewMatrix() * renderable.transform.GetTransformMatrix()
                );

                glDrawElements(GL_TRIANGLES,
                    static_cast<GLsizei>(m_meshes[renderable.meshName].indices.size()),
                    GL_UNSIGNED_SHORT, NULL);

                m_meshes[renderable.meshName].UnbindMesh();

                m_shaderPrograms[renderable.shaderProgramName]->UnUse();
            }
        }


        void RendererManager::InitializeCircleMesh(std::size_t const segments, MeshData& r_mesh)
        { 
            // ------------------------------------ THIS FUNCTION IS NOT DONE

            r_mesh.vertices.clear();
            r_mesh.vertices.reserve(segments);
            float const angle{ glm::pi<float>() * 2.f / static_cast<float>(segments) };

            // Generate vertices in a circle
            for (size_t i{ 0 }; i < segments; ++i) {
                float const totalAngle{ static_cast<float>(i) * angle };
                r_mesh.vertices.emplace_back(
                    VertexData{
                        glm::vec2{glm::cos(totalAngle), glm::sin(totalAngle)},
                        glm::vec3{1.f, 0.f, 0.f},
                        glm::vec2{0.f, 0.f}
                    });
            }
        }


        void RendererManager::InitializeTriangleMesh(MeshData& r_mesh)
        {
            // Add vertex positions, colors and tex coords
            r_mesh.vertices.clear();
            r_mesh.vertices.reserve(3);

            r_mesh.vertices.emplace_back(
                VertexData{
                    glm::vec2{-0.5f, -0.5f},
                    glm::vec3{1.f, 0.f, 0.f},
                    glm::vec2{0.f, 0.f}
                }
            );
            r_mesh.vertices.emplace_back(
                VertexData{
                    glm::vec2{0.5f, -0.5f},
                    glm::vec3{0.f, 1.f, 0.f},
                    glm::vec2{1.f, 0.f}
                }
            );
            r_mesh.vertices.emplace_back(
                VertexData{
                    glm::vec2{0.f, 0.5f},
                    glm::vec3{0.f, 0.f, 1.f},
                    glm::vec2{0.5f, 1.f}
                }
            );

            // Add indices
            r_mesh.indices.clear();
            r_mesh.indices.reserve(3);

            r_mesh.indices.emplace_back(0);
            r_mesh.indices.emplace_back(1);
            r_mesh.indices.emplace_back(2);

            // Generate VAO
            r_mesh.CreateVertexArrayObject();
        }


        void RendererManager::InitializeQuadMesh(MeshData& r_mesh)
        {
            // Add vertex positions, colors and tex coords
            r_mesh.vertices.clear();
            r_mesh.vertices.reserve(4);

            r_mesh.vertices.emplace_back( // bottom-left
                VertexData{
                    glm::vec2{-0.5f, -0.5f},
                    glm::vec3{1.f, 0.f, 0.f},
                    glm::vec2{0.f, 0.f}
                }
            );
            r_mesh.vertices.emplace_back( // bottom-right
                VertexData{
                    glm::vec2{0.5f, -0.5f},
                    glm::vec3{0.f, 1.f, 0.f},
                    glm::vec2{1.f, 0.f}
                }
            );
            r_mesh.vertices.emplace_back( // top-right
                VertexData{
                    glm::vec2{0.5f, 0.5f},
                    glm::vec3{0.f, 0.f, 1.f},
                    glm::vec2{1.f, 1.f}
                }
            );
            r_mesh.vertices.emplace_back( // top-left
                VertexData{
                    glm::vec2{-0.5f, 0.5f},
                    glm::vec3{0.f, 1.f, 0.f},
                    glm::vec2{0.f, 1.f}
                }
            );

            // Add indices
            r_mesh.indices.clear();
            r_mesh.indices.reserve(6);

            r_mesh.indices.emplace_back(0);
            r_mesh.indices.emplace_back(1);
            r_mesh.indices.emplace_back(2);
            r_mesh.indices.emplace_back(2);
            r_mesh.indices.emplace_back(3);
            r_mesh.indices.emplace_back(0);

            // Generate VAO
            r_mesh.CreateVertexArrayObject();
        }


        void RendererManager::PrintSpecifications()
        {
            // Declare variables to store specs info
            GLint majorVersion, minorVersion, maxVertexCount, maxIndicesCount, maxTextureSize, maxViewportDims[2];
            GLboolean isdoubleBuffered;

            // Get and store values
            glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
            glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
            glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVertexCount);
            glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndicesCount);
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
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
                << "\nGL Maximum texture size: " << maxTextureSize
                << "\nMaximum Viewport Dimensions: " << maxViewportDims[0] << " x " << maxViewportDims[1] << "\n" << std::endl;
        }


        void RendererManager::CreateFrameBuffer(int const bufferWidth, int const bufferHeight)
        {
            // Create a frame buffer
            glGenFramebuffers(1, &m_frameBufferObjectIndex);
            glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObjectIndex);

            // Attach a texture to the framebuffer
            glGenTextures(1, &m_imguiTextureId);
            glBindTexture(GL_TEXTURE_2D, m_imguiTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bufferWidth, bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_imguiTextureId, 0);

            // Check if the framebuffer was created successfully
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(true, "Framebuffer is not complete.", __FUNCTION__);
                throw;
            }

            // Unbind all the buffers created
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }


        void RendererManager::BindFrameBuffer()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObjectIndex);
        }


        void RendererManager::UnbindFrameBuffer()
        {
            GLint currentFrameBuffer{};
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFrameBuffer);

            // Unbind the framebuffer object (if it is currently bound)
            if (m_frameBufferObjectIndex == static_cast<GLuint>(currentFrameBuffer)) 
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }


        void RendererManager::ResizeFrameBuffer(GLsizei const newWidth, GLsizei const newHeight)
        {
            glBindTexture(GL_TEXTURE_2D, m_imguiTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_imguiTextureId, 0);
        }

    } // End of Graphics namespace
} // End of PE namespace