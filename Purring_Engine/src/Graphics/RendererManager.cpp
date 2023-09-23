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

#include "Logging/Logger.h" 
#include "RendererManager.h"
#include "ResourceManager/ResourceManager.h"

// ECS Components
#include "ECS/Entity.h"
#include "ECS/EntityFactory.h"
#include "ECS/SceneView.h"

// ImGui
#include "Editor/Editor.h"

// Physics and collision
#include "Physics/Colliders.h"

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
            CreateFrameBuffer(width, height);

            Editor::GetInstance()->Init(p_window);
        }
        
        void RendererManager::InitializeSystem()
        {
            // Print the specs
            PrintSpecifications();

            // Create the framebuffer to render to ImGui window
            int width, height;
            glfwGetWindowSize(p_windowRef, &width, &height);
            CreateFrameBuffer(width, height);
            m_cachedWindowWidth = static_cast<float>(width), 
                m_cachedWindowHeight = static_cast<float>(height);

            // Initialize the base meshes to use
            m_meshes.resize(static_cast<size_t>(EnumMeshType::MESH_COUNT));
            InitializeTriangleMesh(m_meshes[static_cast<unsigned char>(EnumMeshType::TRIANGLE)]);
            InitializeQuadMesh(m_meshes[static_cast<unsigned char>(EnumMeshType::QUAD)]);
            InitializeCircleMesh(32, m_meshes[static_cast<unsigned char>(EnumMeshType::DEBUG_CIRCLE)]);
            InitializeSquareMesh(m_meshes[static_cast<unsigned char>(EnumMeshType::DEBUG_SQUARE)]);
            InitializeLineMesh(m_meshes[static_cast<unsigned char>(EnumMeshType::DEBUG_LINE)]);
            InitializePointMesh(m_meshes[static_cast<unsigned char>(EnumMeshType::DEBUG_POINT)]);

            // Load a shader program
            ResourceManager::GetInstance()->LoadShadersFromFile(m_defaultShaderProgramKey, "../Shaders/Textured.vert", "../Shaders/Textured.frag");
            
            // Load a texture
            ResourceManager::GetInstance()->LoadTextureFromFile(m_defaultTextureName, "../Assets/Textures/Cat1_128x128.png");

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
            Editor::GetInstance()->GetWindowSize(windowWidth, windowHeight);

            // If the window size has changed
            if (m_cachedWindowWidth != windowWidth || m_cachedWindowHeight != windowHeight) 
            {
                m_cachedWindowWidth = windowWidth, m_cachedWindowHeight = windowHeight;

                // Update the frame buffer
                GLsizei const windowWidthInt{ static_cast<GLsizei>(windowWidth) };
                GLsizei const windowHeightInt{ static_cast<GLsizei>(windowHeight) };
                ResizeFrameBuffer(windowWidthInt, windowHeightInt);
                glViewport(0, 0, windowWidthInt, windowHeightInt);
            }

            // Set background color to black
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

            // Enable alpha blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Bind the RBO for rendering to the ImGui window
            BindFrameBuffer();

            // Set the background color of the ImGui window to white
            glClearColor(1.f, 1.f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

            // Compute the view to NDC matrix
            float halfWidth{ windowWidth * 0.5f };
            float halfHeight{ windowHeight * 0.5f };
            glm::mat4 worldToNdc{
                glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f) 
                * m_mainCamera.GetWorldToViewMatrix()
            };


            DrawScene(worldToNdc); // Draw objects in the scene
            DrawDebug(worldToNdc); // Draw debug gizmos in the scene

            // Unbind the RBO for rendering to the ImGui window
            UnbindFrameBuffer();

            Editor::GetInstance()->Render(m_imguiTextureId);

            // Disable alpha blending
            glDisable(GL_BLEND);

            // Poll for and process events
            glfwPollEvents(); // should be called before glfwSwapbuffers

            // Swap front and back buffers
            glfwSwapBuffers(p_windowRef);
        }


        void RendererManager::DestroySystem()
        {
            // Release the buffer objects in each mesh
            for (auto& mesh : m_meshes) {
                mesh.Cleanup();
            }
            m_meshes.clear();

            // Delete the framebuffer object
            glDeleteTextures(1, &m_imguiTextureId);
            glDeleteFramebuffers(1, &m_frameBufferObjectIndex);
        }


        void RendererManager::DrawScene(glm::mat4 const& r_worldToNdc)
        {
            auto shaderProgramIterator{ ResourceManager::GetInstance()->ShaderPrograms.find(m_defaultShaderProgramKey) };

            // Check if shader program is valid
            if (shaderProgramIterator == ResourceManager::GetInstance()->ShaderPrograms.end())
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Shader program " + m_defaultShaderProgramKey + " does not exist.", __FUNCTION__);
                return;
            }

            // Make draw call for each game object with a renderer component
            for (EntityID id : SceneView<Renderer, Transform>())
            {
                Renderer const& renderer{ g_entityManager->Get<Renderer>(id) };
                Transform const& transform{ g_entityManager->Get<Transform>(id) };
                Draw(renderer, *(shaderProgramIterator->second), GL_TRIANGLES,
                    r_worldToNdc * transform.GetTransformMatrix());
            }
        }


        void RendererManager::DrawDebug(glm::mat4 const& r_worldToNdc)
        {
            auto shaderProgramIterator{ ResourceManager::GetInstance()->ShaderPrograms.find(m_defaultShaderProgramKey) };

            // Check if shader program is valid
            if (shaderProgramIterator == ResourceManager::GetInstance()->ShaderPrograms.end())
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Shader program " + m_defaultShaderProgramKey + " does not exist.", __FUNCTION__);
                return;
            }

            // Set the width and size of lines and points
            glLineWidth(3.f);
            glPointSize(10.f);

            // Draw each of the colliders
            for (EntityID id : SceneView<Collider>()) 
            {
                Collider& collider{ g_entityManager->Get<Collider>(id) };

                std::visit([&](auto& col)
                    {
                        DrawCollider(col, r_worldToNdc, 
                            *(shaderProgramIterator->second));
                    }, collider.colliderVariant);
            }

            // Draw a point and line for each rigidbody representing the position and velocity
            for (EntityID id : SceneView<RigidBody, Transform>())
            {
                RigidBody const& rigidbody{ g_entityManager->Get<RigidBody>(id) };
                Transform const& transform{ g_entityManager->Get<Transform>(id) };

                glm::vec2 glmPosition{ transform.position.x, transform.position.y };

                // Draw a line that represents the velocity
                DrawDebugLine(glm::vec2{ rigidbody.m_velocity.x, rigidbody.m_velocity.y }, 
                    glmPosition, r_worldToNdc, *(shaderProgramIterator->second));

                // Draw a point at the center of the object
                DrawDebugPoint(glmPosition, r_worldToNdc, *(shaderProgramIterator->second));
            }

            glPointSize(1.f);
            glLineWidth(1.f);
        }


        void RendererManager::Draw(EnumMeshType meshType, glm::vec4 const& r_color, 
            ShaderProgram& r_shaderProgram, GLenum const primitiveType, glm::mat4 const& r_modelToNdc)
        {
            r_shaderProgram.Use();

            // Check if mesh index is valid
            unsigned char meshIndex{ static_cast<unsigned char>(meshType) };
            if (meshIndex >= m_meshes.size())
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Mesh type is invalid.", __FUNCTION__);
                return;
            }

            m_meshes[meshIndex].BindMesh();

            // Pass the model to NDC transform matrix as a uniform variable
            r_shaderProgram.SetUniform("uModelToNdc", r_modelToNdc);

            // Pass the color of the quad as a uniform variable
            r_shaderProgram.SetUniform("uColor", r_color);

            glDrawElements(primitiveType, static_cast<GLsizei>(m_meshes[meshIndex].indices.size()),
                GL_UNSIGNED_SHORT, NULL);

            // Unbind everything
            m_meshes[meshIndex].UnbindMesh();
            r_shaderProgram.UnUse();
        }


        void RendererManager::Draw(Renderer const& r_renderer, ShaderProgram& r_shaderProgram,
            GLenum const primitiveType, glm::mat4 const& r_modelToNdc)
        {
            r_shaderProgram.Use();

            // Check if mesh index is valid
            unsigned char meshIndex{ static_cast<unsigned char>(r_renderer.GetMeshType()) };
            if (meshIndex >= m_meshes.size())
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Mesh type is invalid.", __FUNCTION__);
                return;
            }

            m_meshes[meshIndex].BindMesh();

            // Pass the model to NDC transform matrix as a uniform variable
            r_shaderProgram.SetUniform("uModelToNdc",
                r_modelToNdc
            );

            // Pass the color of the quad as a uniform variable
            r_shaderProgram.SetUniform("uColor", r_renderer.GetColor());

            // Attempt to retrieve and bind the texture
            std::shared_ptr<Graphics::Texture> p_texture{};

            if (r_renderer.GetTextureKey().empty()) 
            {
                r_shaderProgram.SetUniform("uIsTextured", false);
            }
            else 
            {
                auto textureIterator{ ResourceManager::GetInstance()->Textures.find(r_renderer.GetTextureKey()) };

                // Check if shader program is valid
                if (textureIterator == ResourceManager::GetInstance()->Textures.end())
                {
                    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                    engine_logger.SetTime();
                    engine_logger.AddLog(false, "Texture " + r_renderer.GetTextureKey() + " does not exist.", __FUNCTION__);

                    r_shaderProgram.SetUniform("uIsTextured", false);
                }
                else 
                {
                    p_texture = textureIterator->second;
                    unsigned int textureUnit{ 0 };
                    p_texture->Bind(textureUnit);
                    r_shaderProgram.SetUniform("uTextureSampler2d", textureUnit);
                    r_shaderProgram.SetUniform("uIsTextured", true);
                }
            }

            glDrawElements(primitiveType, static_cast<GLsizei>(m_meshes[meshIndex].indices.size()),
                GL_UNSIGNED_SHORT, NULL);

            // Unbind everything
            m_meshes[meshIndex].UnbindMesh();
            r_shaderProgram.UnUse();

            if (p_texture != nullptr)
            {
                p_texture->Unbind();
            }
        }   


        void RendererManager::DrawCollider(AABBCollider const& r_aabbCollider,
            glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
            glm::vec4 const& r_color)
        {
            glm::mat4 modelToWorld
            {
                GenerateTransformMatrix(r_aabbCollider.max.x - r_aabbCollider.min.x, // width
                    r_aabbCollider.max.y - r_aabbCollider.min.y, // height
                    0.f, // orientation
                    (r_aabbCollider.min.x + r_aabbCollider.max.x) * 0.5f, // x position
                    (r_aabbCollider.min.y + r_aabbCollider.max.y) * 0.5f) // y position
            };

            Draw(EnumMeshType::DEBUG_SQUARE, r_color,
                r_shaderProgram, GL_LINES, r_worldToNdc * modelToWorld);                
        }


        void RendererManager::DrawCollider(CircleCollider const& r_circleCollider,
            glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
            glm::vec4 const& r_color)
        {
            glm::mat4 modelToWorld
            {
                GenerateTransformMatrix(r_circleCollider.radius, // width
                    r_circleCollider.radius, 0.f, // height, orientation
                    r_circleCollider.center.x, r_circleCollider.center.y) // x, y position
            };

            Draw(EnumMeshType::DEBUG_CIRCLE, r_color,
                r_shaderProgram, GL_LINES, r_worldToNdc * modelToWorld);
        }


        void RendererManager::DrawDebugLine(
            glm::vec2 const& r_vector, glm::vec2 const& r_startPosition,
            glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
            glm::vec4 const& r_color)
        {
            glm::mat4 modelToWorld
            {
                GenerateTransformMatrix(r_vector,           // right direction of line
                    glm::vec2{ -r_vector.y, r_vector.x },   // up direction of line (does not affect the line)
                    r_startPosition + (r_vector * 0.5f))    // position
            };

            Draw(EnumMeshType::DEBUG_LINE, r_color,
                r_shaderProgram, GL_LINES, r_worldToNdc * modelToWorld);
        }


        void RendererManager::DrawDebugPoint(glm::vec2 const& r_position,
            glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
            glm::vec4 const& r_color)
        {
            glm::vec4 ndcPosition{ r_position, 0.f, 1.f };
            ndcPosition = r_worldToNdc * ndcPosition;

            Draw(EnumMeshType::DEBUG_POINT, r_color,
                r_shaderProgram, GL_POINTS, glm::mat4{
                    glm::vec4{ 0.f, 0.f, 0.f, 0.f },
                    glm::vec4{ 0.f, 1.f, 0.f, 0.f },
                    glm::vec4{ 0.f, 0.f, 1.f, 0.f },
                    ndcPosition });
        }

        void RendererManager::InitializeCircleMesh(std::size_t const segments, MeshData& r_mesh)
        { 
            r_mesh.vertices.clear();
            r_mesh.vertices.reserve(segments);
            r_mesh.indices.clear();
            r_mesh.indices.reserve(segments);

            float const angle{ glm::pi<float>() * 2.f / static_cast<float>(segments) };

            // Generate vertices in a circle
            for (int i{ 0 }; i < (int)segments; ++i) {
                float const totalAngle{ static_cast<float>(i) * angle };
                r_mesh.vertices.emplace_back(
                    glm::vec2{glm::cos(totalAngle), glm::sin(totalAngle)},
                    glm::vec2{0.f, 0.f});

                r_mesh.indices.emplace_back(((i - 1) < 0 ? (short)segments - 1 : (short)i - 1));
                r_mesh.indices.emplace_back((short)i);
            }

            // Generate VAO
            r_mesh.CreateVertexArrayObject();
        }


        void RendererManager::InitializeTriangleMesh(MeshData& r_mesh)
        {
            // Add vertex positions, colors and tex coords
            r_mesh.vertices.clear();
            r_mesh.vertices.reserve(3);

            // bottom-left
            r_mesh.vertices.emplace_back(glm::vec2{-0.5f, -0.5f}, glm::vec2{0.f, 0.f});
            // bottom-right
            r_mesh.vertices.emplace_back(glm::vec2{0.5f, -0.5f}, glm::vec2{1.f, 0.f});
            // top-center
            r_mesh.vertices.emplace_back(glm::vec2{0.f, 0.5f}, glm::vec2{0.5f, 1.f});


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

            // bottom-left
            r_mesh.vertices.emplace_back(glm::vec2{-0.5f, -0.5f}, glm::vec2{0.f, 0.f});
            // bottom-right
            r_mesh.vertices.emplace_back(glm::vec2{0.5f, -0.5f}, glm::vec2{1.f, 0.f});
            // top-right
            r_mesh.vertices.emplace_back(glm::vec2{0.5f, 0.5f}, glm::vec2{1.f, 1.f});
            // top-left
            r_mesh.vertices.emplace_back(glm::vec2{-0.5f, 0.5f}, glm::vec2{0.f, 1.f});


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


        void RendererManager::InitializeSquareMesh(MeshData& r_mesh)
        {
            // Add vertex positions, colors and tex coords
            r_mesh.vertices.clear();
            r_mesh.vertices.reserve(4);

            // bottom-left
            r_mesh.vertices.emplace_back(glm::vec2{-0.5f, -0.5f}, glm::vec2{0.f, 0.f});
            // bottom-right
            r_mesh.vertices.emplace_back(glm::vec2{0.5f, -0.5f}, glm::vec2{1.f, 0.f});
            // top-right
            r_mesh.vertices.emplace_back(glm::vec2{0.5f, 0.5f}, glm::vec2{1.f, 1.f});
            // top-left
            r_mesh.vertices.emplace_back(glm::vec2{-0.5f, 0.5f}, glm::vec2{0.f, 1.f});

            // Add indices
            r_mesh.indices.clear();
            r_mesh.indices.reserve(8);

            r_mesh.indices.emplace_back(0);
            r_mesh.indices.emplace_back(1);
            r_mesh.indices.emplace_back(1);
            r_mesh.indices.emplace_back(2);
            r_mesh.indices.emplace_back(2);
            r_mesh.indices.emplace_back(3);
            r_mesh.indices.emplace_back(3);
            r_mesh.indices.emplace_back(0);

            // Generate VAO
            r_mesh.CreateVertexArrayObject();
        }


        void RendererManager::InitializeLineMesh(MeshData& r_mesh)
        {
            // Add vertex positions, colors and tex coords
            r_mesh.vertices.clear();
            r_mesh.vertices.reserve(2);

            r_mesh.vertices.emplace_back(glm::vec2{-0.5f, 0.f}, glm::vec2{0.f, 0.f});
            r_mesh.vertices.emplace_back(glm::vec2{0.5f, 0.f}, glm::vec2{1.f, 0.f});

            // Add indices
            r_mesh.indices.clear();
            r_mesh.indices.reserve(2);

            r_mesh.indices.emplace_back(0);
            r_mesh.indices.emplace_back(1);

            // Generate VAO
            r_mesh.CreateVertexArrayObject();
        }


        void RendererManager::InitializePointMesh(MeshData& r_mesh)
        {
            // Add vertex positions, colors and tex coords
            r_mesh.vertices.clear();
            r_mesh.vertices.reserve(1);

            r_mesh.vertices.emplace_back( glm::vec2{0.f, 0.f}, glm::vec2{0.f, 0.f} );

            // Add indices
            r_mesh.indices.clear();
            r_mesh.indices.resize(1);

            // Generate VAO
            r_mesh.CreateVertexArrayObject();
        }


        glm::mat4 RendererManager::GenerateTransformMatrix(float const width, float const height,
            float const orientation, float const positionX, float const positionY) 
        {
            // Get scale matrix
            glm::mat4 scale_matrix{
                width,  0.f,    0.f, 0.f,
                0.f,    height, 0.f, 0.f,
                0.f,    0.f,    1.f, 0.f,
                0.f,    0.f,    0.f, 1.f
            };

            // Get rotation matrix
            GLfloat sin_angle{ glm::sin(orientation) };
            GLfloat cos_angle{ glm::cos(orientation) };
            glm::mat4 rotation_matrix{
                cos_angle,  sin_angle, 0.f, 0.f,
                -sin_angle, cos_angle, 0.f, 0.f,
                0.f,        0.f,       1.f, 0.f,
                0.f,        0.f,       0.f, 1.f
            };

            // Get translation matrix
            glm::mat4 translation_matrix{
                1.f,    0.f,    0.f,    0.f,
                0.f,    1.f,    0.f,    0.f,
                0.f,    0.f,    1.f,    0.f,
                positionX, positionY, 0.f, 1.f
            };

            return translation_matrix * rotation_matrix * scale_matrix;
        }


        glm::mat4 RendererManager::GenerateTransformMatrix(glm::vec2 const& horizontalVector,
            glm::vec2 const& verticalVector, glm::vec2 const& centerPosition)
        {
            return glm::mat4{
                horizontalVector.x, horizontalVector.y, 0.f,    0.f,
                verticalVector.x,   verticalVector.y,   0.f,    0.f,
                0.f,    0.f,    1.f,    0.f,
                centerPosition.x, centerPosition.y,     0.f,    1.f
            };
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