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

// Animation
#include "Animation/Animation.h"

extern Logger engine_logger;

std::vector<float> isTextured{};
std::vector<glm::mat4> modelToWorldMatrices{};
std::vector<glm::vec4> colors{};

// temp animation manager
PE::AnimationManager animationManager;
int idleAnimation, walkingAnimation;

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
            m_imguiFrameBuffer.CreateFrameBuffer(width, height);

            Editor::GetInstance().Init(p_window);
        }
        
        void RendererManager::InitializeSystem()
        {
            // Print the specs
            PrintSpecifications();

            // Create the framebuffer to render to ImGui window
            int width, height;
            glfwGetWindowSize(p_windowRef, &width, &height);
            m_imguiFrameBuffer.CreateFrameBuffer(width, height);
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
            ResourceManager::GetInstance().LoadShadersFromFile(m_defaultShaderProgramKey, "../Shaders/Textured.vert", "../Shaders/Textured.frag");
            ResourceManager::GetInstance().LoadShadersFromFile(m_instancedShaderProgramKey, "../Shaders/Instanced.vert", "../Shaders/Instanced.frag");

            isTextured.reserve(3000);
            modelToWorldMatrices.reserve(3000);
            colors.reserve(3000);

            // Create Animation
            idleAnimation = animationManager.CreateAnimation();
            walkingAnimation = animationManager.CreateAnimation();

            // animation 1
            animationManager.AddFrameToAnimation(idleAnimation, "catAnim1", 0.1f);
            animationManager.AddFrameToAnimation(idleAnimation, "catAnim2", 0.1f);
            animationManager.AddFrameToAnimation(idleAnimation, "catAnim3", 0.1f);
            animationManager.AddFrameToAnimation(idleAnimation, "catAnim4", 0.1f);
            animationManager.AddFrameToAnimation(idleAnimation, "catAnim5", 0.1f);

            // animation 2
            animationManager.AddFrameToAnimation(walkingAnimation, "cat", 0.2f);
            animationManager.AddFrameToAnimation(walkingAnimation, "cat2Anim1", 0.2f);
            animationManager.AddFrameToAnimation(walkingAnimation, "cat2", 0.2f);
            animationManager.AddFrameToAnimation(walkingAnimation, "cat2Anim2", 0.2f);

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

            // Retrieve whether to render in the editor or the full window
            bool renderInEditor{ Editor::GetInstance().IsEditorActive() };

            // Get the size of the window to render in
            float windowWidth{}, windowHeight{};


            if (renderInEditor)
            {
                Editor::GetInstance().GetWindowSize(windowWidth, windowHeight);
            }
            else 
            {
                int width, height;
                glfwGetWindowSize(p_windowRef, &width, &height);
                windowWidth = static_cast<float>(width);
                windowHeight = static_cast<float>(height);
            }

            // If the window size has changed
            if (m_cachedWindowWidth != windowWidth || m_cachedWindowHeight != windowHeight) 
            {
                m_cachedWindowWidth = windowWidth, m_cachedWindowHeight = windowHeight;

                // Update the frame buffer
                GLsizei const windowWidthInt{ static_cast<GLsizei>(windowWidth) };
                GLsizei const windowHeightInt{ static_cast<GLsizei>(windowHeight) };
                m_imguiFrameBuffer.Resize(windowWidthInt, windowHeightInt);
                glViewport(0, 0, windowWidthInt, windowHeightInt);

                // Compute the view to NDC matrix
                float halfWidth{ windowWidth * 0.5f };
                float halfHeight{ windowHeight * 0.5f };
                m_cachedWorldToNdcMatrix = 
                    glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f)
                    * m_mainCamera.GetWorldToViewMatrix();
            } 
            else if (m_mainCamera.GetHasChanged()) 
            {
                // Compute the view to NDC matrix
                float halfWidth{ windowWidth * 0.5f };
                float halfHeight{ windowHeight * 0.5f };
                m_cachedWorldToNdcMatrix =
                    glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f)
                    * m_mainCamera.GetWorldToViewMatrix();
            }

            // Set background color to black
            glClearColor(1.f, 1.f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

            // Enable alpha blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            if(renderInEditor)
            {
                // Bind the RBO for rendering to the ImGui window
                m_imguiFrameBuffer.Bind();

                // Set the background color of the ImGui window to white
                glClearColor(1.f, 1.f, 1.f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
            }

            // Update Animation here
            std::string currentTextureKey;
            if (g_entityManager->Get<RigidBody>(1).GetVelocity().x == 0.f &&
                g_entityManager->Get<RigidBody>(1).GetVelocity().y == 0.f)
            {
                currentTextureKey = animationManager.UpdateAnimation(idleAnimation, deltaTime);
            }
            else
            {
                currentTextureKey = animationManager.UpdateAnimation(walkingAnimation, deltaTime);
            }
            
            g_entityManager->Get<Graphics::Renderer>(1).SetTextureKey(currentTextureKey);

            DrawSceneInstanced(m_cachedWorldToNdcMatrix); // Draw objects in the scene

            if (Editor::GetInstance().IsRenderingDebug()) 
            {
                DrawDebug(m_cachedWorldToNdcMatrix); // Draw debug gizmos in the scene
            }


            if (renderInEditor)
            {
                // Unbind the RBO for rendering to the ImGui window
                m_imguiFrameBuffer.Unbind();

                Editor::GetInstance().Render(m_imguiFrameBuffer.GetTextureId());
            }

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
            m_imguiFrameBuffer.Cleanup();
        }


        void RendererManager::DrawScene(glm::mat4 const& r_worldToNdc)
        {
            auto shaderProgramIterator{ ResourceManager::GetInstance().ShaderPrograms.find(m_defaultShaderProgramKey) };

            // Check if shader program is valid
            if (shaderProgramIterator == ResourceManager::GetInstance().ShaderPrograms.end())
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Shader program " + m_defaultShaderProgramKey + " does not exist.", __FUNCTION__);
                return;
            }

            // Make draw call for each game object with a renderer component
            for (const EntityID& id : SceneView<Renderer>())
            {
                Renderer& renderer{ g_entityManager->Get<Renderer>(id) };
                Transform& transform{ g_entityManager->Get<Transform>(id) };

                glm::mat4 glmObjectTransform
                {
                    GenerateTransformMatrix(transform.width, // width
                        transform.height, transform.orientation, // height, orientation
                        transform.position.x, transform.position.y) // x, y position
                };

                Draw(renderer, *(shaderProgramIterator->second), GL_TRIANGLES,
                    r_worldToNdc * glmObjectTransform);
            }
        }

        void RendererManager::DrawSceneInstanced(glm::mat4 const& r_worldToNdc)
        {
            auto shaderProgramIterator{ ResourceManager::GetInstance().ShaderPrograms.find(m_instancedShaderProgramKey) };

            // Check if shader program is valid
            if (shaderProgramIterator == ResourceManager::GetInstance().ShaderPrograms.end())
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Shader program " + m_instancedShaderProgramKey + " does not exist.", __FUNCTION__);
                return;
            }

            ShaderProgram& r_shaderProgram{ *(shaderProgramIterator->second) };
            r_shaderProgram.Use();

            // Pass the world to NDC transform matrix as a uniform variable
            r_shaderProgram.SetUniform("uWorldToNdc", r_worldToNdc);

            int count{};
            size_t meshIndex{ static_cast<unsigned char>(EnumMeshType::QUAD) };
            m_meshes[meshIndex].Bind();

            std::string currentTexture{};
            std::shared_ptr<Graphics::Texture> p_texture{};

            
            isTextured.clear();
            modelToWorldMatrices.clear();
            colors.clear();

            // Make draw call for each game object with a renderer component

            for (const EntityID& id : SceneView<Renderer>())
            {
                const Renderer& renderer{ g_entityManager->Get<Renderer>(id) };
                const Transform& transform{ g_entityManager->Get<Transform>(id) };

                // Attempt to retrieve and bind the texture
                if (renderer.GetTextureKey().empty())
                {
                    isTextured.emplace_back(0);
                }
                else if(currentTexture != renderer.GetTextureKey())
                {
                    // Check if we were already 
                    if (!currentTexture.empty()) 
                    {
                        DrawInstanced(count, meshIndex, GL_TRIANGLES);
                        currentTexture.clear();
                        count = 0;
                    }

                    auto textureIterator{ ResourceManager::GetInstance().Textures.find(renderer.GetTextureKey()) };

                    // Check if shader program is valid
                    if (textureIterator == ResourceManager::GetInstance().Textures.end())
                    {
                        engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                        engine_logger.SetTime();
                        engine_logger.AddLog(false, "Texture " + renderer.GetTextureKey() + " does not exist.", __FUNCTION__);

                        isTextured.emplace_back(0);
                    }
                    else
                    {
                        currentTexture = renderer.GetTextureKey();
                        p_texture = textureIterator->second;
                        unsigned int textureUnit{ 0 };
                        p_texture->Bind(textureUnit);
                        r_shaderProgram.SetUniform("uTextureSampler2d", textureUnit);
                        isTextured.emplace_back(1);
                    }
                }
                else 
                {
                    isTextured.emplace_back(1);
                }

                modelToWorldMatrices.emplace_back(GenerateTransformMatrix(transform.width, // width
                    transform.height, transform.orientation, // height, orientation
                    transform.position.x, transform.position.y)); // x, y position
                colors.emplace_back(renderer.GetColor());

                ++count;
            }

            DrawInstanced(count, meshIndex, GL_TRIANGLES);

            // Unbind everything
            m_meshes[meshIndex].Unbind();
            r_shaderProgram.UnUse();

            if (p_texture != nullptr)
            {
                p_texture->Unbind();
            }
        }


        void RendererManager::DrawDebugInstanced(glm::mat4 const& r_worldToNdc)
        {
            auto shaderProgramIterator{ ResourceManager::GetInstance().ShaderPrograms.find(m_instancedShaderProgramKey) };

            // Check if shader program is valid
            if (shaderProgramIterator == ResourceManager::GetInstance().ShaderPrograms.end())
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Shader program " + m_instancedShaderProgramKey + " does not exist.", __FUNCTION__);
                return;
            }

            // Set the width and size of lines and points
            glLineWidth(3.f);
            glPointSize(10.f);

            ShaderProgram& r_shaderProgram{ *(shaderProgramIterator->second) };
            r_shaderProgram.Use();

            // Pass the world to NDC transform matrix as a uniform variable
            r_shaderProgram.SetUniform("uWorldToNdc", r_worldToNdc);

            // Draw each of the colliders
            for (const EntityID& id : SceneView<Collider>())
            {
                Collider& collider{ g_entityManager->Get<Collider>(id) };

                std::visit([&](auto& col)
                    {
                        DrawCollider(col, r_worldToNdc, 
                            *(shaderProgramIterator->second));
                    }, collider.colliderVariant);
            }

            // Draw a point and line for each rigidbody representing the position and velocity
            for (const EntityID& id : SceneView<RigidBody>())
            {
                RigidBody& rigidbody{ g_entityManager->Get<RigidBody>(id) };
                Transform& transform{ g_entityManager->Get<Transform>(id) };

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


        void RendererManager::DrawDebug(glm::mat4 const& r_worldToNdc)
        {
            auto shaderProgramIterator{ ResourceManager::GetInstance().ShaderPrograms.find(m_defaultShaderProgramKey) };

            // Check if shader program is valid
            if (shaderProgramIterator == ResourceManager::GetInstance().ShaderPrograms.end())
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
            for (const EntityID& id : SceneView<Collider>())
            {
                Collider& collider{ g_entityManager->Get<Collider>(id) };

                std::visit([&](auto& col)
                    {
                        DrawCollider(col, r_worldToNdc, 
                            *(shaderProgramIterator->second));
                    }, collider.colliderVariant);
            }

            // Draw a point and line for each rigidbody representing the position and velocity
            for (const EntityID& id : SceneView<RigidBody>())
            {
                RigidBody& rigidbody{ g_entityManager->Get<RigidBody>(id) };
                Transform& transform{ g_entityManager->Get<Transform>(id) };

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

            m_meshes[meshIndex].Bind();

            // Pass the model to NDC transform matrix as a uniform variable
            r_shaderProgram.SetUniform("uModelToNdc", r_modelToNdc);

            // Pass the color of the quad as a uniform variable
            r_shaderProgram.SetUniform("uColor", r_color);

            // Tell the program that we're not using textures
            r_shaderProgram.SetUniform("uIsTextured", false);

            glDrawElements(primitiveType, static_cast<GLsizei>(m_meshes[meshIndex].indices.size()),
                GL_UNSIGNED_SHORT, NULL);

            // Unbind everything
            m_meshes[meshIndex].Unbind();
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

            m_meshes[meshIndex].Bind();

            // Pass the model to NDC transform matrix as a uniform variable
            r_shaderProgram.SetUniform("uModelToNdc", r_modelToNdc);

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
                auto textureIterator{ ResourceManager::GetInstance().Textures.find(r_renderer.GetTextureKey()) };

                // Check if shader program is valid
                if (textureIterator == ResourceManager::GetInstance().Textures.end())
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
            m_meshes[meshIndex].Unbind();
            r_shaderProgram.UnUse();

            if (p_texture != nullptr)
            {
                p_texture->Unbind();
            }
        }   

        void RendererManager::DrawInstanced(size_t const count, size_t const meshIndex, GLenum const primitiveType)
        {
            if (!count) { return; }

            size_t sizeOfTexturedVector{ isTextured.size() * sizeof(float) };
            size_t sizeOfColorVector{ colors.size() * sizeof(glm::vec4) };
            size_t sizeOfMatrixVector{ modelToWorldMatrices.size() * sizeof(glm::mat4) };


            // Create buffer object for additional vertex data
            GLuint vbo_hdl;
            glCreateBuffers(1, &vbo_hdl);
            glNamedBufferStorage(vbo_hdl,
                static_cast<GLsizeiptr>(sizeOfTexturedVector + sizeOfColorVector + sizeOfMatrixVector),
                nullptr, GL_DYNAMIC_STORAGE_BIT);


            // Store textured bools in VBO
            glNamedBufferSubData(vbo_hdl, 0,
                static_cast<GLsizeiptr>(sizeOfTexturedVector),
                reinterpret_cast<GLvoid*>(isTextured.data())); 

            // Bind the textured bools
            GLuint attributeIndex{ 2 }, bindingIndex{ 2 };
            glEnableVertexArrayAttrib(m_meshes[meshIndex].GetVertexArrayObjectIndex(), attributeIndex);
            glVertexArrayVertexBuffer(m_meshes[meshIndex].GetVertexArrayObjectIndex(), bindingIndex, vbo_hdl, 0,
                static_cast<GLsizei>(sizeof(float)));
            glVertexArrayAttribFormat(m_meshes[meshIndex].GetVertexArrayObjectIndex(), attributeIndex, 1, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(m_meshes[meshIndex].GetVertexArrayObjectIndex(), attributeIndex, bindingIndex);
            glVertexAttribDivisor(attributeIndex, 1);


            // Store colors in VBO
            glNamedBufferSubData(vbo_hdl,
                static_cast<GLintptr>(sizeOfTexturedVector),
                static_cast<GLsizeiptr>(sizeOfColorVector),
                reinterpret_cast<GLvoid*>(colors.data()));

            // Bind the colors
            ++attributeIndex, ++bindingIndex;
            glEnableVertexArrayAttrib(m_meshes[meshIndex].GetVertexArrayObjectIndex(), attributeIndex);
            glVertexArrayVertexBuffer(m_meshes[meshIndex].GetVertexArrayObjectIndex(), bindingIndex, vbo_hdl,
                static_cast<GLintptr>(sizeOfTexturedVector), static_cast<GLsizei>(sizeof(glm::vec4)));
            glVertexArrayAttribFormat(m_meshes[meshIndex].GetVertexArrayObjectIndex(), attributeIndex, 4, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(m_meshes[meshIndex].GetVertexArrayObjectIndex(), attributeIndex, bindingIndex);
            glVertexAttribDivisor(attributeIndex, 1);

            // Store model to NDC matrix in VBO
            glNamedBufferSubData(vbo_hdl,
                static_cast<GLintptr>(sizeOfTexturedVector + sizeOfColorVector),
                static_cast<GLsizeiptr>(sizeOfMatrixVector),
                reinterpret_cast<GLvoid*>(modelToWorldMatrices.data()));

            // Bind the model to NDC matrices
            ++attributeIndex, ++bindingIndex;

            glVertexArrayVertexBuffer(m_meshes[meshIndex].GetVertexArrayObjectIndex(), bindingIndex, vbo_hdl,
                static_cast<GLintptr>(sizeOfTexturedVector + sizeOfColorVector),
                static_cast<GLsizei>(sizeof(glm::mat4)));

            for (int i{}; i < 4; ++i)
            {
                glEnableVertexArrayAttrib(m_meshes[meshIndex].GetVertexArrayObjectIndex(), attributeIndex);

                glVertexArrayAttribFormat(m_meshes[meshIndex].GetVertexArrayObjectIndex(), attributeIndex, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4 * i); // offset by vert pos and color
                glVertexArrayAttribBinding(m_meshes[meshIndex].GetVertexArrayObjectIndex(), attributeIndex, bindingIndex);
                ++attributeIndex;
            }

            glVertexArrayBindingDivisor(m_meshes[meshIndex].GetVertexArrayObjectIndex(), bindingIndex, 1);


            glDrawElementsInstanced(primitiveType, static_cast<GLsizei>(m_meshes[meshIndex].indices.size()),
                GL_UNSIGNED_SHORT, NULL, count);

            // Delete the buffer
            glDeleteBuffers(1, &vbo_hdl);

            --attributeIndex;
            while (attributeIndex >= 2) 
            {
                glDisableVertexArrayAttrib(m_meshes[meshIndex].GetVertexArrayObjectIndex(), attributeIndex--);
            }

            // Clear the vectors
            isTextured.clear();
            colors.clear();
            modelToWorldMatrices.clear();
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
            float const diameter{ r_circleCollider.radius * 2.f };

            glm::mat4 modelToWorld
            {
                GenerateTransformMatrix(diameter, diameter, 0.f, // width, height, orientation
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
                    glm::vec2{glm::cos(totalAngle) * 0.5f, glm::sin(totalAngle) * 0.5f},
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
            // Get rotation matrix
            GLfloat sin_angle{ glm::sin(orientation) };
            GLfloat cos_angle{ glm::cos(orientation) };

            return glm::mat4{
                width * cos_angle, width * sin_angle, 0.f, 0.f,
                height * -sin_angle, height * cos_angle, 0.f, 0.f,
                0.f,        0.f,       1.f, 0.f,
                positionX, positionY, 0.f, 1.f
            };
        }


        glm::mat4 RendererManager::GenerateTransformMatrix(glm::vec2 const& rightVector,
            glm::vec2 const& upVector, glm::vec2 const& centerPosition)
        {
            return glm::mat4{
                rightVector.x, rightVector.y, 0.f,    0.f,
                upVector.x,   upVector.y,   0.f,    0.f,
                0.f,    0.f,    1.f,    0.f,
                centerPosition.x, centerPosition.y, 0.f, 1.f
            };
        }

        void RendererManager::PrintSpecifications() const
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
    } // End of Graphics namespace
} // End of PE namespace