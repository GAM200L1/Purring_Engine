/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     RendererManager.cpp
 \creation date:       20-08-2023
 \last updated:        16-09-2023
 \author:              Krystal YAMIN

 \par      email:      krystal.y@digipen.edu

 \brief    This file contains the RendererManager class, which manages 
           the render passes and includes helper functions to draw debug shapes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"

#include <glm/gtc/constants.hpp>    // pi()
#include <glm/gtc/matrix_transform.hpp> // ortho()

#include "Logging/Logger.h" // ----- @TODO: Fix the include paths... ------
#include "RendererManager.h" // <cstddef>, <GLFW/glfw3.h>, <glm/glm.hpp>, <vector>

#include "Editor/Editor.h"

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
                std::cerr << "Failed to initialize GLEW." << std::endl;
                exit(-1);
            }

            PrintSpecifications();

            p_windowRef = p_window;

            int width, height;
            glfwGetWindowSize(p_windowRef, &width, &height);
            create_framebuffer(width, height);

            Editor::GetInstance()->Init(p_window);
        }
        
        void RendererManager::InitializeSystem()
        {
            // Initialize the base meshes to use
            // Note: Making shallow copies of the meshData objects to store in the map.

            // Triangle mesh  
            MeshData triangleMesh{};
            triangleMesh.vertices.emplace_back(
                VertexData{
                    glm::vec2{-0.5f, -0.5f},
                    glm::vec3{1.f, 0.f, 0.f},
                    glm::vec2{0.f, 0.f}
                }
            );
            triangleMesh.vertices.emplace_back(
                VertexData{
                    glm::vec2{0.5f, -0.5f},
                    glm::vec3{0.f, 1.f, 0.f},
                    glm::vec2{1.f, 0.f}
                }
            );
            triangleMesh.vertices.emplace_back(
                VertexData{
                    glm::vec2{0.f, 0.5f},
                    glm::vec3{0.f, 0.f, 1.f},
                    glm::vec2{0.5f, 1.f}
                }
            );

            triangleMesh.indices.emplace_back(0);
            triangleMesh.indices.emplace_back(1);
            triangleMesh.indices.emplace_back(2);


            // Quad mesh  
            MeshData quadMesh{};
            quadMesh.vertices.emplace_back( // bottom-left
                VertexData{
                    glm::vec2{-0.5f, -0.5f},
                    glm::vec3{1.f, 0.f, 0.f},
                    glm::vec2{0.f, 0.f}
                }
            );
            quadMesh.vertices.emplace_back( // bottom-right
                VertexData{
                    glm::vec2{0.5f, -0.5f},
                    glm::vec3{0.f, 1.f, 0.f},
                    glm::vec2{1.f, 0.f}
                }
            );
            quadMesh.vertices.emplace_back( // top-right
                VertexData{
                    glm::vec2{0.5f, 0.5f},
                    glm::vec3{0.f, 0.f, 1.f},
                    glm::vec2{1.f, 1.f}
                }
            );
            quadMesh.vertices.emplace_back( // top-left
                VertexData{
                    glm::vec2{-0.5f, 0.5f},
                    glm::vec3{0.f, 1.f, 0.f},
                    glm::vec2{0.f, 1.f}
                }
            );

            quadMesh.indices.emplace_back(0);
            quadMesh.indices.emplace_back(1);
            quadMesh.indices.emplace_back(2);
            quadMesh.indices.emplace_back(2);
            quadMesh.indices.emplace_back(3);
            quadMesh.indices.emplace_back(0);

            m_meshes["triangle"] = triangleMesh;
            m_meshes["quad"] = quadMesh;

            m_meshes["triangle"].CreateVertexArrayObject();
            m_meshes["quad"].CreateVertexArrayObject();


            // Create a shader program
            // Store the vert shader
            const std::string vertexShaderString{ R"(            
            #version 450 core

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
            #version 450 core

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

            // Set viewport size
            int width, height;
            glfwGetFramebufferSize(p_windowRef, &width, &height);

            //// Rescale
            float windowWidth{}, windowHeight{};
            Editor::GetInstance()->GetWindowSize(windowWidth, windowHeight);
            rescale_framebuffer(windowWidth, windowHeight);
            glViewport(0, 0, windowWidth, windowHeight);

            // Set background color to black
            //glClearColor(1.f, 1.f, 1.f, 1.f);
            //glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer


            bind_framebuffer();

            glClearColor(1.f, 1.f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
            // Draw game objects in the scene
            DrawScene(windowWidth, windowHeight);

            unbind_framebuffer();

            Editor::GetInstance()->Render(texture_id);

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
        }


        void RendererManager::DrawScene(int const width, int const height)
        {
            float halfWidth{ static_cast<float>(width) * 0.5f };
            float halfHeight{ static_cast<float>(height) * 0.5f };
            glm::mat4 viewToNdc{
                glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f)
            };

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


        void RendererManager::GenerateCirclePoints(std::size_t const segments, std::vector<glm::vec2>& r_points)
        {
            r_points.clear();
            r_points.reserve(segments);
            float const angle{ glm::pi<float>() * 2.f / static_cast<float>(segments) };

            // Generate vertices in a circle
            for (size_t i{ 0 }; i < segments; ++i) {
                float const totalAngle{ static_cast<float>(i) * angle };
                r_points.emplace_back(glm::vec2(
                    glm::cos(totalAngle),
                    glm::sin(totalAngle)));
            }
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


        void RendererManager::create_framebuffer(int const WIDTH, int const HEIGHT)
        {
            glGenFramebuffers(1, &FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        // here we bind our framebuffer
        void RendererManager::bind_framebuffer()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        }

        // here we unbind our framebuffer
        void RendererManager::unbind_framebuffer()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // and we rescale the buffer, so we're able to resize the window
        void RendererManager::rescale_framebuffer(float width, float height)
        {
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
        }

    } // End of Graphics namespace
} // End of PE namespace