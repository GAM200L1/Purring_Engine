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
#include <GL/glew.h> // for access to OpenGL API declarations 
#include <glm/gtc/constants.hpp>    // pi()
#include <glm/gtc/matrix_transform.hpp> // ortho()

#include "Logging/Logger.h" // ----- @TODO: Fix the include paths... ------
#include "RendererManager.h" // <cstddef>, <GLFW/glfw3.h>, <glm/glm.hpp>, <vector>

extern Logger engine_logger;

namespace PE
{
    namespace Graphics
    {
        RendererManager::RendererManager(GLFWwindow* p_window) 
        {
            p_windowRef = p_window;
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
            glViewport(0, 0, width, height);

            // Set background color to black
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

            // Draw game objects in the scene
            DrawScene(width, height);

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
    } // End of Graphics namespace
} // End of PE namespace