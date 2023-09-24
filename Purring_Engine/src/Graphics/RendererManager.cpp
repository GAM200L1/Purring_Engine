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

#include "Editor/Editor.h"

// text
#include "Text.h"

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
            m_cachedWindowWidth = width, m_cachedWindowHeight = height;

            // Initialize the base meshes to use
            m_meshes.resize(5);
            InitializeTriangleMesh(m_meshes[static_cast<unsigned char>(EnumMeshType::TRIANGLE)]);
            InitializeQuadMesh(m_meshes[static_cast<unsigned char>(EnumMeshType::QUAD)]);
            InitializeCircleMesh(32, m_meshes[static_cast<unsigned char>(EnumMeshType::DEBUG_CIRCLE)]);
            InitializeSquareMesh(m_meshes[static_cast<unsigned char>(EnumMeshType::DEBUG_SQUARE)]);
            InitializeLineMesh(m_meshes[static_cast<unsigned char>(EnumMeshType::DEBUG_LINE)]);

            // Load a shader program
            ResourceManager::GetInstance()->LoadShadersFromFile(m_defaultShaderProgramKey, "../Shaders/Textured.vert", "../Shaders/Textured.frag");
            
            // Load a texture
            ResourceManager::GetInstance()->LoadTextureFromFile(m_defaultTextureName, "../Textures/Cat1_128x128.png");

            // Load a font
            ResourceManager::GetInstance()->LoadShadersFromFile("text", "../Shaders/Text.vert", "../Shaders/Text.frag");
            m_font.Init(ResourceManager::ShaderPrograms["text"]);

            // Add background objects
            AddBackgroundObject(width, height, glm::vec4{ 0.f, 0.f, 0.f, 1.f });
            AddBackgroundObject(width, height, m_defaultTextureName, glm::vec4{ 1.f, 1.f, 0.f, 0.5f });

            // Add a triangle and quad as renderable objects
            AddRendererObject(EnumMeshType::QUAD, 400.f, 400.f, 0.f,
                glm::vec2{ 0.f, 0.f }, m_defaultTextureName, glm::vec4{ 1.f, 0.f, 0.f, 0.5f });
            AddRendererObject(EnumMeshType::TRIANGLE, 100.f, 400.f, 45.f,
                glm::vec2{ 200.f, 400.f }, m_defaultTextureName, glm::vec4{ 0.f, 1.f, 0.f, 0.5f });

            AddRendererObject(EnumMeshType::QUAD, 100.f, 100.f, 20.f,
                glm::vec2{ -300.f, -300.f }, glm::vec4{ 1.f, 1.f, 0.f, 1.f});
            AddRendererObject(EnumMeshType::TRIANGLE, 200.f, 200.f, -10.f,
                glm::vec2{ -100.f, -100.f }, glm::vec4{ 0.f, 1.f, 1.f, 0.5f});

            AddDebugSquare(400.f, 200.f, 30.f, glm::vec2{10.f, 30.f}, glm::vec4{ 1.f, 0.f, 1.f, 0.5f });
            AddDebugSquare(glm::vec2{-50.f, -50.f}, glm::vec2{50.f, 50.f}, glm::vec4{ 0.f, 0.f, 1.f, 0.5f });
            AddDebugLine(glm::vec2{-50.f, -50.f}, glm::vec2{50.f, 50.f}, glm::vec4{ 0.f, 0.f, 1.f, 0.5f });
            AddDebugCircle(250.f, glm::vec2{ -200.f, 200.f }, glm::vec4{ 0.f, 1.f, 0.f, 0.5f });
            AddDebugPoint(glm::vec2{ 10.f, 0.f }, glm::vec4{ 0.f, 0.f, 0.f, 1.f });

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
            ResizeFrameBuffer(windowWidth, windowHeight);
            glViewport(0, 0, windowWidth, windowHeight);

            // If the window size has changed
            if (m_cachedWindowWidth != windowWidth || m_cachedWindowHeight != windowHeight) 
            {
                m_cachedWindowWidth = windowWidth, m_cachedWindowHeight = windowHeight;

                // Update the frame buffer
                GLsizei const windowWidthInt{ static_cast<GLsizei>(windowWidth) };
                GLsizei const windowHeightInt{ static_cast<GLsizei>(windowHeight) };
                ResizeFrameBuffer(windowWidthInt, windowHeightInt);
                glViewport(0, 0, windowWidthInt, windowHeightInt);

                // Update size of background objects
                for (auto& backgroundObject : m_backgroundObjects) {
                    backgroundObject.transform.width = windowWidth;
                    backgroundObject.transform.height = windowHeight;
                }
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
            glm::mat4 viewToNdc{
                glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f)
            };



            DrawDebug(viewToNdc); // Draw debug gizmos in the scene

            m_font.RenderText("TEST: ", { m_mainCamera.GetPosition().x + 0.2f, m_mainCamera.GetPosition().y + 0.2f },
                0.0008f, m_mainCamera, { 0.87f, 0.92f, 0.8f });

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


        void RendererManager::DrawScene(glm::mat4 const& r_viewToNdc)
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

            // Make draw call for each background object
            for (auto& backgroundObject : m_backgroundObjects) {
                DrawRenderer(backgroundObject, *(shaderProgramIterator->second), GL_TRIANGLES,
                    r_viewToNdc * backgroundObject.transform.GetTransformMatrix());
            }

            // Call glDrawElements for each renderable object
            for (auto& renderable : m_triangleObjects) {

                DrawRenderer(renderable, *(shaderProgramIterator->second), GL_TRIANGLES,
                    r_viewToNdc * m_mainCamera.GetWorldToViewMatrix() * renderable.transform.GetTransformMatrix());
            }
        }


        void RendererManager::DrawDebug(glm::mat4 const& r_viewToNdc)
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

            glLineWidth(3.f);

            // Make draw call for each debug object
            for (auto& debugShape : m_lineObjects) {
                DrawRenderer(debugShape, *(shaderProgramIterator->second), GL_LINES, 
                    r_viewToNdc * m_mainCamera.GetWorldToViewMatrix() * debugShape.transform.GetTransformMatrix());
            }

            glPointSize(10.f);

            // Make draw call for each point
            for (auto& point : m_pointObjects) {
                DrawRenderer(point, *(shaderProgramIterator->second), GL_POINTS, 
                    r_viewToNdc * m_mainCamera.GetWorldToViewMatrix() * point.transform.GetTransformMatrix());
            }

            glPointSize(1.f);
            glLineWidth(1.f);
        }


        void RendererManager::DrawRenderer(Renderer const& r_renderer, ShaderProgram& r_shaderProgram, GLenum const primitiveType, glm::mat4 const& r_modelToNdc)
        {
            r_shaderProgram.Use();

            // Check if mesh index is valid
            unsigned char meshIndex{ static_cast<unsigned char>(r_renderer.meshType) };
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
            r_shaderProgram.SetUniform("uColor", r_renderer.color);

            // Bind the texture object
            if (r_renderer.p_texture != nullptr)
            {
                unsigned int textureUnit{ 0 };
                r_renderer.p_texture->Bind(textureUnit);
                r_shaderProgram.SetUniform("uTextureSampler2d", textureUnit);
                r_shaderProgram.SetUniform("uIsTextured", true);
            }
            else {
                r_shaderProgram.SetUniform("uIsTextured", false);
            }

            glDrawElements(primitiveType, static_cast<GLsizei>(m_meshes[meshIndex].indices.size()),
                GL_UNSIGNED_SHORT, NULL);

            // Unbind everything
            m_meshes[meshIndex].UnbindMesh();
            r_shaderProgram.UnUse();

            if (r_renderer.p_texture != nullptr)
            {
                r_renderer.p_texture->Unbind();
            }
        }


        void RendererManager::AddRendererObject(EnumMeshType meshType,
            float const width, float const height, float const orientation, 
            glm::vec2 const& r_position, glm::vec4 const& r_color)
        {
            Renderer newRenderer{};
            newRenderer.meshType = meshType;

            newRenderer.transform.width = width;
            newRenderer.transform.height = height;
            newRenderer.transform.orientation = orientation;
            newRenderer.transform.position = vec2{ r_position.x, r_position.y };

            newRenderer.color = r_color;

            m_triangleObjects.emplace_back(newRenderer);
        }

        void RendererManager::AddRendererObject(EnumMeshType meshType,
            float const width, float const height,
            float const orientation, glm::vec2 const& r_position,
            std::string const& r_texture, glm::vec4 const& r_color)
        {
            Renderer newRenderer{};
            newRenderer.meshType = meshType;

            newRenderer.transform.width = width;
            newRenderer.transform.height = height;
            newRenderer.transform.orientation = orientation;
            newRenderer.transform.position = vec2{ r_position.x, r_position.y };

            newRenderer.p_texture = ResourceManager::GetInstance()->GetTexture(r_texture);
            newRenderer.color = r_color;

            m_triangleObjects.emplace_back(newRenderer);
        }

        void RendererManager::AddBackgroundObject(
            float const width, float const height, glm::vec4 const& r_color)
        {
            Renderer newRenderer{};
            newRenderer.meshType = EnumMeshType::QUAD;

            newRenderer.transform.width = width;
            newRenderer.transform.height = height;
            newRenderer.transform.orientation = 0.f;
            newRenderer.transform.position = vec2{ 0.f, 0.f };

            newRenderer.color = r_color;

            m_backgroundObjects.emplace_back(newRenderer);
        }

        void RendererManager::AddBackgroundObject(
            float const width, float const height,
            std::string const& r_texture, glm::vec4 const& r_color)
        {
            Renderer newRenderer{};
            newRenderer.meshType = EnumMeshType::QUAD;

            newRenderer.transform.width = width;
            newRenderer.transform.height = height;
            newRenderer.transform.orientation = 0.f;
            newRenderer.transform.position = vec2{ 0.f, 0.f };

            newRenderer.p_texture = ResourceManager::GetInstance()->GetTexture(r_texture);
            newRenderer.color = r_color;

            m_backgroundObjects.emplace_back(newRenderer);
        }

        void RendererManager::AddDebugSquare(
            float const width, float const height,
            float const orientation, glm::vec2 const& r_centerPosition,
            glm::vec4 const& r_color)
        {
            Renderer newRenderer{};
            newRenderer.meshType = EnumMeshType::DEBUG_SQUARE;

            newRenderer.transform.width = width;
            newRenderer.transform.height = height;
            newRenderer.transform.orientation = orientation;
            newRenderer.transform.position = vec2{ r_centerPosition.x, r_centerPosition.y };

            newRenderer.color = r_color;

            m_lineObjects.emplace_back(newRenderer);
        }


        void RendererManager::AddDebugSquare(
            glm::vec2 const& r_bottomLeft, glm::vec2 const& r_topRight,
            glm::vec4 const& r_color)
        {
            // Assumes that this is a axis - aligned
            AddDebugSquare(
                r_topRight.x - r_bottomLeft.x,
                r_topRight.y - r_bottomLeft.y,
                0.f, (r_bottomLeft + r_topRight) * 0.5f,
                r_color
            );
        }


        void RendererManager::AddDebugCircle(
            float const radius, glm::vec2 const& r_centerPosition,
            glm::vec4 const& r_color)
        {
            Renderer newRenderer{};
            newRenderer.meshType = EnumMeshType::DEBUG_CIRCLE;
            
            // circle mesh has a radius of two, so setting the width and height
            // to the radius is what we want
            newRenderer.transform.width = radius; 
            newRenderer.transform.height = radius;
            newRenderer.transform.position = vec2{ r_centerPosition.x, r_centerPosition.y };

            newRenderer.color = r_color;

            m_lineObjects.emplace_back(newRenderer);
        }


        void RendererManager::AddDebugLine(
            glm::vec2 const& r_position1, glm::vec2 const& r_position2,
            glm::vec4 const& r_color)
        {
            Renderer newRenderer{};
            newRenderer.meshType = EnumMeshType::DEBUG_LINE;

            float angleRadians = glm::atan2(r_position2.y - r_position1.y, r_position2.x - r_position1.x);
            newRenderer.transform.orientation = glm::degrees(angleRadians);
            newRenderer.transform.width = glm::distance(r_position1, r_position2);
            glm::vec2 linePos = (r_position1 + r_position2) * 0.5f;
            newRenderer.transform.position = vec2{ linePos.x, linePos.y };

            newRenderer.color = r_color;

            m_lineObjects.emplace_back(newRenderer);
        }


        void RendererManager::AddDebugPoint(glm::vec2 const& r_position,
            glm::vec4 const& r_color)
        {
            Renderer newRenderer{};
            newRenderer.meshType = EnumMeshType::DEBUG_LINE;

            newRenderer.transform.width = 0.f;
            newRenderer.transform.position = vec2{ r_position.x, r_position.y };

            newRenderer.color = r_color;

            m_pointObjects.emplace_back(newRenderer);
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
                    VertexData{
                        glm::vec2{glm::cos(totalAngle), glm::sin(totalAngle)},
                        //glm::vec3{glm::cos(totalAngle), glm::sin(totalAngle), 1.f},
                        glm::vec2{0.f, 0.f}
                    });

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
            r_mesh.vertices.emplace_back(VertexData{ glm::vec2{-0.5f, -0.5f}, glm::vec2{0.f, 0.f} });
            // bottom-right
            r_mesh.vertices.emplace_back(VertexData{ glm::vec2{0.5f, -0.5f}, glm::vec2{1.f, 0.f} });
            // top-center
            r_mesh.vertices.emplace_back(VertexData{ glm::vec2{0.f, 0.5f}, glm::vec2{0.5f, 1.f} });


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
            r_mesh.vertices.emplace_back(VertexData{ glm::vec2{-0.5f, -0.5f}, glm::vec2{0.f, 0.f} });
            // bottom-right
            r_mesh.vertices.emplace_back(VertexData{ glm::vec2{0.5f, -0.5f}, glm::vec2{1.f, 0.f} });
            // top-right
            r_mesh.vertices.emplace_back(VertexData{ glm::vec2{0.5f, 0.5f}, glm::vec2{1.f, 1.f} });
            // top-left
            r_mesh.vertices.emplace_back(VertexData{ glm::vec2{-0.5f, 0.5f}, glm::vec2{0.f, 1.f} });


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
            r_mesh.vertices.emplace_back(VertexData{ glm::vec2{-0.5f, -0.5f}, glm::vec2{0.f, 0.f} });
            // bottom-right
            r_mesh.vertices.emplace_back(VertexData{ glm::vec2{0.5f, -0.5f}, glm::vec2{1.f, 0.f} });
            // top-right
            r_mesh.vertices.emplace_back(VertexData{ glm::vec2{0.5f, 0.5f}, glm::vec2{1.f, 1.f} });
            // top-left
            r_mesh.vertices.emplace_back(VertexData{ glm::vec2{-0.5f, 0.5f}, glm::vec2{0.f, 1.f} });

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

            r_mesh.vertices.emplace_back( VertexData{ glm::vec2{-0.5f, 0.f}, glm::vec2{0.f, 0.f} } );
            r_mesh.vertices.emplace_back( VertexData{ glm::vec2{0.5f, 0.f}, glm::vec2{1.f, 0.f} } );

            // Add indices
            r_mesh.indices.clear();
            r_mesh.indices.reserve(2);

            r_mesh.indices.emplace_back(0);
            r_mesh.indices.emplace_back(1);

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