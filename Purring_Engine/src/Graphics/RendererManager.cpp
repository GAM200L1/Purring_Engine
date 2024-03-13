/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     RendererManager.cpp
 \date:    30-08-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 \par      code %:     95%
 \par      changes:    Defined the render pipeline and all the functions in this file
                       other than the RenderText function.

 \co-author            Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu
 \par      code %:     5%
 \par      changes:    02-11-2023
                       Added function to render text.

 \brief    This file contains the RendererManager class, which manages
           the render passes and includes helper functions to draw debug shapes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"

#include "RendererManager.h"
#include <glm/gtc/constants.hpp>    // pi()
#include <glm/gtc/matrix_transform.hpp> // ortho()

#include "Logging/Logger.h" 
#include "ResourceManager/ResourceManager.h"

// ECS Components
#include "ECS/Entity.h"
#include "ECS/EntityFactory.h"
#include "Hierarchy/HierarchyManager.h"
// ImGui
#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE

// to handle missing hierarchy when run in game release
#ifdef GAMERELEASE
#include "Hierarchy/HierarchyManager.h"
#endif

// Physics and collision
#include "Physics/Colliders.h"

// text
#include "Text.h"
#include "Time/TimeManager.h"

// Animation
#include "Animation/Animation.h"

// GUI
#include "GUI/Canvas.h"

// layering
#include "Layers/LayerManager.h"

#include "Text.h"

// Particle effects
#include "VisualEffects/ParticleSystem.h"

extern Logger engine_logger;

namespace PE
{
    namespace Graphics
    {
        // Initialize static variables
        std::vector<EntityID> RendererManager::renderedEntities{};
        unsigned RendererManager::totalDrawCalls{};   // Sum total of all draw calls made
        unsigned RendererManager::textDrawCalls{};    // Total draw calls made for text (1 draw call per chara)
        unsigned RendererManager::objectDrawCalls{};  // Total draw calls for gameobjects
        unsigned RendererManager::debugDrawCalls{};   // Total draw calls for debug shapes

        RendererManager::RendererManager(CameraManager& r_cameraManagerArg, int const windowWidth, int const windowHeight)
            : r_cameraManager{ r_cameraManagerArg }, m_windowStartWidth{ windowWidth }, m_windowStartHeight{ windowHeight }
        {
            // Initialize GLEW
            if (glewInit() != GLEW_OK)
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(true, "Failed to initialize GLEW.", __FUNCTION__);
                throw;
            }

            // Enable alpha blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef GAMERELEASE
            // Enable debug output
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(GlDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

            Editor::GetInstance().Init();
#endif // !GAMERELEASE
        }


        void RendererManager::SetBackgroundColor(float const r, float const g, float const b, float const a)
        {
            m_backgroundColor.r = r;
            m_backgroundColor.g = g;
            m_backgroundColor.b = b;
            m_backgroundColor.a = a;
        }


        void RendererManager::InitializeSystem()
        {
            // Print the specs
            PrintSpecifications();

            // Create the framebuffer to render to a texture object
            int width, height;
            glfwGetWindowSize(WindowManager::GetInstance().GetWindow(), &width, &height);
            m_renderFrameBuffer.CreateFrameBuffer(width, height, true, false);
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
            ResourceManager::GetInstance().LoadShadersFromFile(m_textShaderProgramKey, "../Shaders/Text.vert", "../Shaders/Text.frag");

            // Reserve memory for the vectors to build the buffer with
            m_isTextured.reserve(3000);
            m_modelToWorldMatrices.reserve(3000);
            m_colors.reserve(3000);
            m_UV.reserve(3000);
            renderedEntities.reserve(3000);

            engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
            engine_logger.SetTime();
            engine_logger.AddLog(false, "RendererManager initialized!", __FUNCTION__);
        }


        void RendererManager::UpdateSystem(float deltaTime)
        {
            if (!WindowManager::GetInstance().GetWindow())
            {
                return;
            }
            deltaTime; // Prevent warnings

            // Reset counters
            totalDrawCalls = 0, textDrawCalls = 0, objectDrawCalls = 0, debugDrawCalls = 0;

            // Get the size of the window to render in
            float windowWidth{}, windowHeight{};

            // Reset the render order container
            renderedEntities.clear();

            // resizing window
#ifndef GAMERELEASE
            if (Editor::GetInstance().IsEditorActive())
            {
                Editor::GetInstance().GetWindowSize(windowWidth, windowHeight);
            }
            else
            {
                int width, height;
                glfwGetWindowSize(WindowManager::GetInstance().GetWindow(), &width, &height);
                windowWidth = static_cast<float>(width);
                windowHeight = static_cast<float>(height);
            }
#else
            int width, height;
            glfwGetWindowSize(WindowManager::GetInstance().GetWindow(), &width, &height);
            windowWidth = static_cast<float>(width);
            windowHeight = static_cast<float>(height);
#endif
            // Set background color of the window
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

            // Bind the RBO for rendering to a texture object
            m_renderFrameBuffer.Bind();

            // If the window size has changed
            if ((!CompareFloats(windowWidth, 0.f) && !CompareFloats(windowHeight, 0.f)) &&
                (m_cachedWindowWidth != windowWidth || m_cachedWindowHeight != windowHeight))
            {
                m_cachedWindowWidth = windowWidth, m_cachedWindowHeight = windowHeight;

                // Update the frame buffer
                GLsizei const windowWidthInt{ static_cast<GLsizei>(windowWidth) };
                GLsizei const windowHeightInt{ static_cast<GLsizei>(windowHeight) };
                glViewport(0, 0, windowWidthInt, windowHeightInt);
                m_renderFrameBuffer.Resize(windowWidthInt, windowHeightInt);

#ifndef GAMERELEASE
                // Update the editor camera viewport size
                r_cameraManager.GetEditorCamera().SetViewDimensions(windowWidth, windowHeight);
#endif // !GAMERELEASE
            }

#ifndef GAMERELEASE
            if (Editor::GetInstance().IsEditorActive())
            {
                // Update the ui camera viewport size to the size of the editor window
                r_cameraManager.GetUiCamera().SetViewDimensions(windowWidth, windowHeight);
            }
            else
            {
                // Update the ui camera viewport size to the size of the main camera if it exists,
                // or the cached start window size
                std::optional<std::reference_wrapper<Camera>> optional_mainCamera{ r_cameraManager.GetMainCamera() };

                if (optional_mainCamera.has_value())
                {
                    r_cameraManager.GetUiCamera().SetViewDimensions(optional_mainCamera.value().get().GetViewportWidth(), optional_mainCamera.value().get().GetViewportHeight());
                }
                else
                {
                    r_cameraManager.GetUiCamera().SetViewDimensions(static_cast<float>(m_windowStartWidth), static_cast<float>(m_windowStartHeight));
                }
            }
#else
            // Update the ui camera viewport size to the size of the main camera if it exists,
            // or the cached start window size
            std::optional<std::reference_wrapper<Camera>> optional_mainCamera{ r_cameraManager.GetMainCamera() };

            if (optional_mainCamera.has_value())
            {
                r_cameraManager.GetUiCamera().SetViewDimensions(optional_mainCamera.value().get().GetViewportWidth(), optional_mainCamera.value().get().GetViewportHeight());
            }
            else
            {
                r_cameraManager.GetUiCamera().SetViewDimensions(static_cast<float>(m_windowStartWidth), static_cast<float>(m_windowStartHeight));
            }
#endif // !GAMERELEASE

            // Clear the texture object
            m_renderFrameBuffer.Clear(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);

#ifndef GAMERELEASE            
            // Get the world to NDC matrix of the editor cam or the main runtime camera
            glm::mat4 worldToNdcMatrix{ r_cameraManager.GetWorldToNdcMatrix(Editor::GetInstance().IsEditorActive()) };
#else
            glm::mat4 worldToNdcMatrix{ r_cameraManager.GetWorldToNdcMatrix(false) };
#endif // !GAMERELEASE

            // Draw objects in the scene
            DrawQuadsInstanced<Renderer>(worldToNdcMatrix, Hierarchy::GetInstance().GetRenderOrder());

#ifndef GAMERELEASE            
            // Draw UI objects in the scene
            DrawQuadsInstanced<GUIRenderer>(worldToNdcMatrix, Hierarchy::GetInstance().GetRenderOrderUI());

            // Render Text
            //RenderText(Editor::GetInstance().IsEditorActive() ? worldToNdcMatrix : r_cameraManager.GetUiViewToNdcMatrix());
#else
            // Draw UI objects in the scene
            DrawQuadsInstanced<GUIRenderer>(r_cameraManager.GetUiViewToNdcMatrix(), Hierarchy::GetInstance().GetRenderOrderUI());

            // Render Text
            //RenderText(r_cameraManager.GetUiViewToNdcMatrix());
#endif // !GAMERELEASE

#ifndef GAMERELEASE
            if (Editor::GetInstance().IsRenderingDebug())
            {
                DrawDebug(worldToNdcMatrix); // Draw debug gizmos in the scene
            }
#endif // !GAMERELEASE

            // Unbind the RBO for rendering the game scene to
            m_renderFrameBuffer.Unbind();

            totalDrawCalls = textDrawCalls + objectDrawCalls + debugDrawCalls;

#ifndef GAMERELEASE
            // Check if the Imgui editor windows are active
            if (!Editor::GetInstance().IsEditorActive())
            {
                DrawCameraQuad();
            }

            Editor::GetInstance().Render(m_renderFrameBuffer);
#else
            DrawCameraQuad();
#endif // !GAMERELEASE


            // Poll for and process events
            glfwPollEvents(); // should be called before glfwSwapbuffers

            // Swap front and back buffers
            glfwSwapBuffers(WindowManager::GetInstance().GetWindow());
        }


        void RendererManager::DestroySystem()
        {
            // Release the buffer objects in each mesh
            for (auto& mesh : m_meshes) {
                mesh.Cleanup();
            }
            m_meshes.clear();

            // Delete the framebuffer object
            m_renderFrameBuffer.Cleanup();
        }

        void RendererManager::DrawCameraQuad()
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

            // Create the model to world matrix
            glm::mat4 glmObjectTransform
            {
                GenerateTransformMatrix(m_cachedWindowWidth, m_cachedWindowHeight, 0.f, 0.f, 0.f)
            };

            glm::mat4 windowToNdc
            {
                2.f / m_cachedWindowWidth, 0.f, 0.f, 0.f,
                0.f, 2.f / m_cachedWindowHeight, 0.f, 0.f,
                0.f, 0.f, -2.f / 20.f, 0.f,
                0.f, 0.f, 0.f, 1.f // assumption: view frustrum is centered
            };

            Draw(EnumMeshType::QUAD, glm::vec4{ 1.f, 1.f, 1.f, 1.f },
                m_renderFrameBuffer.GetTextureId(), *(shaderProgramIterator->second),
                GL_TRIANGLES, windowToNdc * glmObjectTransform);
        }


        //--------------------------------------------------------------------------------------------------------------------------------------------

                //template<typename T>
                //void RendererManager::DrawQuads(glm::mat4 const& r_worldToNdc, SceneView<T, Transform> const& r_sceneView)
                //{
                //    auto shaderProgramIterator{ ResourceManager::GetInstance().ShaderPrograms.find(m_defaultShaderProgramKey) };

                //    // Check if shader program is valid
                //    if (shaderProgramIterator == ResourceManager::GetInstance().ShaderPrograms.end())
                //    {
                //        engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                //        engine_logger.SetTime();
                //        engine_logger.AddLog(false, "Shader program " + m_defaultShaderProgramKey + " does not exist.", __FUNCTION__);
                //        return;
                //    }

                //    // Make draw call for each game object with a renderer component
                //    for (const EntityID& id : r_sceneView)
                //    {
                //        T& renderer{ EntityManager::GetInstance().Get<T>(id) };

                //        // Skip drawing this object is the entity or renderer is not enabled
                //        if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive
                //            || !renderer.GetEnabled()/* || !Hierarchy::GetInstance().AreParentsActive(id)*/) { continue; }

                //        Transform& transform{ EntityManager::GetInstance().Get<Transform>(id) };

                //        // Store the index of the rendered entity
                //        renderedEntities.emplace_back(id);

                //        glm::mat4 glmObjectTransform
                //        {
                //            GenerateTransformMatrix(transform.width, // width
                //                transform.height, transform.orientation, // height, orientation
                //                transform.position.x, transform.position.y) // x, y position
                //        };

                //        Draw(dynamic_cast<Renderer&>(renderer), *(shaderProgramIterator->second), GL_TRIANGLES,
                //            r_worldToNdc * glmObjectTransform);
                //    }
                //}


                //template<typename T>
                //void RendererManager::DrawQuadsInstanced(glm::mat4 const& r_worldToNdc, SceneView<T, Transform> const& r_sceneView)
                //{
                //    auto shaderProgramIterator{ ResourceManager::GetInstance().ShaderPrograms.find(m_instancedShaderProgramKey) };

                //    // Check if shader program is valid
                //    if (shaderProgramIterator == ResourceManager::GetInstance().ShaderPrograms.end())
                //    {
                //        engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                //        engine_logger.SetTime();
                //        engine_logger.AddLog(false, "Shader program " + m_instancedShaderProgramKey + " does not exist.", __FUNCTION__);
                //        return;
                //    }

                //    ShaderProgram& r_shaderProgram{ *(shaderProgramIterator->second) };
                //    r_shaderProgram.Use();

                //    // Pass the world to NDC transform matrix as a uniform variable
                //    r_shaderProgram.SetUniform("uWorldToNdc", r_worldToNdc);

                //    // Bind the quad mesh
                //    size_t meshIndex{ static_cast<unsigned char>(EnumMeshType::QUAD) };
                //    m_meshes[meshIndex].Bind();

                //    // Store the texture being used
                //    std::string currentTexture{};
                //    std::shared_ptr<Graphics::Texture> p_texture{};

                //    // Clear the buffers for the 
                //    m_isTextured.clear();
                //    m_UV.clear();
                //    m_modelToWorldMatrices.clear();
                //    m_colors.clear();

                //    int count{};

                //    // Make draw call for each game object with a renderer component

                //    for (const EntityID& id : r_sceneView)
                //    {
                //        T& renderer{ EntityManager::GetInstance().Get<T>(id) };
                //        
                //        // Skip drawing this object is the entity or renderer is not enabled
                //        if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive 
                //            || !renderer.GetEnabled()/* || !Hierarchy::GetInstance().AreParentsActive(id)*/) { continue; }

                //        // Store the index of the rendered entity
                //        renderedEntities.emplace_back(id);
                //        
                //        const Transform& transform{ EntityManager::GetInstance().Get<Transform>(id) };

                //        // Attempt to retrieve and bind the texture
                //        if (renderer.GetTextureKey().empty())
                //        {
                //            m_isTextured.emplace_back(0.f);
                //        }
                //        else if(currentTexture != renderer.GetTextureKey())
                //        {
                //            // Check if we were already 
                //            if (!currentTexture.empty()) 
                //            {
                //                DrawInstanced(count, meshIndex, GL_TRIANGLES);
                //                currentTexture.clear();
                //                count = 0;
                //            }

                //            std::shared_ptr<Texture> texture { ResourceManager::GetInstance().GetTexture(renderer.GetTextureKey()) };

                //            // Check if texture is null
                //            if (!texture)
                //            {
                //                // Remove the texture and set the object to neon pink
                //                renderer.SetTextureKey("");
                //                renderer.SetColor(1.f, 0.f, 1.f, 1.f);

                //                m_isTextured.emplace_back(0.f);
                //            }
                //            else
                //            {
                //                // Unbind the existing texture
                //                if (p_texture) 
                //                {
                //                    p_texture->Unbind();
                //                }

                //                // Store the texture key of the current texture
                //                currentTexture = renderer.GetTextureKey();

                //                // Bind the new texture
                //                GLint textureUnit{ 0 };
                //                p_texture = texture;
                //                p_texture->Bind(textureUnit);
                //                r_shaderProgram.SetUniform("uTextureSampler2d", textureUnit);

                //                m_isTextured.emplace_back(1.f);
                //            }
                //        }
                //        else 
                //        {
                //            m_isTextured.emplace_back(1.f);
                //        }

                //        // Add the matrix and colors to the buffer
                //        m_modelToWorldMatrices.emplace_back(GenerateTransformMatrix(transform.width, // width
                //            transform.height, transform.orientation, // height, orientation
                //            transform.position.x, transform.position.y)); // x, y position
                //        m_colors.emplace_back(renderer.GetColor());

                //        // Add the UV coordinate adjustments
                //        m_UV.emplace_back(renderer.GetUVCoordinatesMin()); // bottom left
                //        m_UV.emplace_back(renderer.GetUVCoordinatesMax().x, renderer.GetUVCoordinatesMin().y); // bottom right
                //        m_UV.emplace_back(renderer.GetUVCoordinatesMax()); // top right
                //        m_UV.emplace_back(renderer.GetUVCoordinatesMin().x, renderer.GetUVCoordinatesMax().y); // top left

                //        ++count; 
                //    }

                //    // Draw the remaining objects
                //    DrawInstanced(count, meshIndex, GL_TRIANGLES);

                //    // Unbind everything
                //    m_meshes[meshIndex].Unbind();
                //    r_shaderProgram.UnUse();

                //    if (p_texture != nullptr)
                //    {
                //        p_texture->Unbind();
                //    }
                //}

        //--------------------------------------------------------------------------------------------------------------------------------------------

        template<typename T>
        void RendererManager::DrawQuadsInstanced(glm::mat4 const& r_worldToNdc, std::vector<EntityID> const& r_rendererIdContainer)
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

            // Bind the quad mesh
            size_t meshIndex{ static_cast<unsigned char>(EnumMeshType::QUAD) };
            m_meshes[meshIndex].Bind();

            // Store the texture being used
            std::string currentTexture{};
            std::shared_ptr<Graphics::Texture> p_texture{};

            // Clear the buffers for the 
            m_isTextured.clear();
            m_UV.clear();
            m_modelToWorldMatrices.clear();
            m_colors.clear();

            int count{};

            // Make draw call for each game object with a renderer component
            for (EntityID const& id : r_rendererIdContainer)
            {

                // break the instanced render
                if (EntityManager::GetInstance().Has<PE::TextComponent>(id))
                {
                    DrawInstanced(count, meshIndex, GL_TRIANGLES);
                    currentTexture.clear();
                    count = 0;
                    RenderText(id, r_worldToNdc);
                    // continue from the next one after it
                    auto ite = ++(std::find(r_rendererIdContainer.begin(), r_rendererIdContainer.end(), id));
                    std::vector<size_t> cont;
                    while (ite != r_rendererIdContainer.end())
                    {
                        cont.emplace_back(*ite);
                        ++ite;
                    }
                    DrawQuadsInstanced<T>(r_worldToNdc, cont);
                    return;
                }

                // handle particle effects
                if (EntityManager::GetInstance().Has<PE::ParticleEmitter>(id))
                {
                    
                    // dump current 
                    DrawInstanced(count, meshIndex, GL_TRIANGLES);
                    currentTexture.clear();
                    count = 0;
                    // Render particle effects
                    auto& em = EntityManager::GetInstance().Get<ParticleEmitter>(id);
                    if (!em.isActive)
                        continue;
                    T& renderer{ EntityManager::GetInstance().Get<T>(id) };

                    switch (em.particleType)
                    {
                    case SQUARE:
                    {
                        for (auto& p : em.GetParticles())
                        {
                            if (!p.enabled)
                                continue;
                            auto& xform = p.transform;
                            m_isTextured.emplace_back(0.f);
                            m_modelToWorldMatrices.emplace_back(GenerateTransformMatrix(xform.width, // width
                                xform.height, xform.orientation, // height, orientation
                                xform.position.x, xform.position.y)); // x, y position
                            m_colors.emplace_back(glm::vec4(em.startColor.x, em.startColor.y, em.startColor.z, em.startColor.w));

                            // Add the UV coordinate adjustments
                            m_UV.emplace_back(renderer.GetUVCoordinatesMin()); // bottom left
                            m_UV.emplace_back(renderer.GetUVCoordinatesMax().x, renderer.GetUVCoordinatesMin().y); // bottom right
                            m_UV.emplace_back(renderer.GetUVCoordinatesMax()); // top right
                            m_UV.emplace_back(renderer.GetUVCoordinatesMin().x, renderer.GetUVCoordinatesMax().y); // top left
                            ++count;
                        }
                        DrawInstanced(count, meshIndex, GL_TRIANGLES);
                        currentTexture.clear();
                        count = 0;
                    }
                    break;
                    /*case CIRCLE:
                    {

                    }
                        break;*/
                    case TEXTURED:
                    case ANIMATED:
                    {
                        const glm::vec2& minUV = renderer.GetUVCoordinatesMin();
                        const glm::vec2& maxUV = renderer.GetUVCoordinatesMax();
                        if (EntityManager::GetInstance().Has<AnimationComponent>(id) && EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimation()->GetFrameCount())
                        {
                            std::map<int, std::vector<const PE::Particle*>> particles;
                            for (auto& p : em.GetParticles())
                            {
                                particles[p.spriteID % EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimation()->GetFrameCount()].emplace_back(&p);
                            }
                            for (auto& [k, v] : particles)
                            {
                                int frame = ((em.particleType == ANIMATED)? EntityManager::GetInstance().Get<AnimationComponent>(id).GetCurrentFrameIndex() : 0) + k;
                                frame %= EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimation()->GetFrameCount();
                                glm::vec2 minOffset{ EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimation()->GetCurrentAnimationFrame(frame).m_minUV.x, EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimation()->GetCurrentAnimationFrame(frame).m_minUV.y };
                                glm::vec2 maxOffset{ EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimation()->GetCurrentAnimationFrame(frame).m_maxUV.x, EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimation()->GetCurrentAnimationFrame(frame).m_maxUV.y };
                                for (auto& p : v)
                                {
                                    if (!p->enabled)
                                        continue;
                                    // Attempt to retrieve and bind the texture
                                    if (renderer.GetTextureKey().empty())
                                    {
                                        m_isTextured.emplace_back(0.f);
                                    }
                                    else if (currentTexture != renderer.GetTextureKey())
                                    {
                                        // Check if we were already 
                                        if (!currentTexture.empty())
                                        {
                                            DrawInstanced(count, meshIndex, GL_TRIANGLES);
                                            currentTexture.clear();
                                            count = 0;
                                        }

                                        std::shared_ptr<Texture> texture{ ResourceManager::GetInstance().GetTexture(renderer.GetTextureKey()) };

                                        // Check if texture is null
                                        if (!texture)
                                        {
                                            // Remove the texture and set the object to neon pink
                                            renderer.SetTextureKey("");
                                            renderer.SetColor(1.f, 0.f, 1.f, 1.f);

                                            m_isTextured.emplace_back(0.f);
                                        }
                                        else
                                        {
                                            // Unbind the existing texture
                                            if (p_texture)
                                            {
                                                p_texture->Unbind();
                                            }

                                            // Store the texture key of the current texture
                                            currentTexture = renderer.GetTextureKey();

                                            // Bind the new texture
                                            GLint textureUnit{ 0 };
                                            p_texture = texture;
                                            p_texture->Bind(textureUnit);
                                            r_shaderProgram.SetUniform("uTextureSampler2d", textureUnit);

                                            m_isTextured.emplace_back(1.f);
                                        }
                                    }
                                    else
                                    {
                                        m_isTextured.emplace_back(1.f);
                                    }
                                    // change this line + look for texture
                                    //m_isTextured.emplace_back(0.f);
                                    auto& xform = p->transform;
                                    m_modelToWorldMatrices.emplace_back(GenerateTransformMatrix(xform.width, // width
                                        xform.height, xform.orientation, // height, orientation
                                        xform.position.x, xform.position.y)); // x, y position
                                    m_colors.emplace_back(glm::vec4(em.startColor.x, em.startColor.y, em.startColor.z, em.startColor.w));

                                    // Add the UV coordinate adjustments
                                    m_UV.emplace_back(minOffset); // bottom left
                                    m_UV.emplace_back(maxOffset.x, minOffset.y); // bottom right
                                    m_UV.emplace_back(maxOffset); // top right
                                    m_UV.emplace_back(minOffset.x, maxOffset.y); // top left

                                    ++count;

                                }
                                DrawInstanced(count, meshIndex, GL_TRIANGLES);
                                count = 0;
                            }
                        }
                        else
                        {
                            for (auto& p : em.GetParticles())
                            {
                                if (!p.enabled)
                                    continue;
                                // Attempt to retrieve and bind the texture
                                if (renderer.GetTextureKey().empty())
                                {
                                    m_isTextured.emplace_back(0.f);
                                }
                                else if (currentTexture != renderer.GetTextureKey())
                                {
                                    // Check if we were already 
                                    if (!currentTexture.empty())
                                    {
                                        DrawInstanced(count, meshIndex, GL_TRIANGLES);
                                        currentTexture.clear();
                                        count = 0;
                                    }

                                    std::shared_ptr<Texture> texture{ ResourceManager::GetInstance().GetTexture(renderer.GetTextureKey()) };

                                    // Check if texture is null
                                    if (!texture)
                                    {
                                        // Remove the texture and set the object to neon pink
                                        renderer.SetTextureKey("");
                                        renderer.SetColor(1.f, 0.f, 1.f, 1.f);

                                        m_isTextured.emplace_back(0.f);
                                    }
                                    else
                                    {
                                        // Unbind the existing texture
                                        if (p_texture)
                                        {
                                            p_texture->Unbind();
                                        }

                                        // Store the texture key of the current texture
                                        currentTexture = renderer.GetTextureKey();

                                        // Bind the new texture
                                        GLint textureUnit{ 0 };
                                        p_texture = texture;
                                        p_texture->Bind(textureUnit);
                                        r_shaderProgram.SetUniform("uTextureSampler2d", textureUnit);

                                        m_isTextured.emplace_back(1.f);
                                    }
                                }
                                else
                                {
                                    m_isTextured.emplace_back(1.f);
                                }
                                // change this line + look for texture
                                //m_isTextured.emplace_back(0.f);
                                auto& xform = p.transform;
                                m_modelToWorldMatrices.emplace_back(GenerateTransformMatrix(xform.width, // width
                                    xform.height, xform.orientation, // height, orientation
                                    xform.position.x, xform.position.y)); // x, y position
                                m_colors.emplace_back(glm::vec4(em.startColor.x, em.startColor.y, em.startColor.z, em.startColor.w));

                                // Add the UV coordinate adjustments
                                m_UV.emplace_back(minUV); // bottom left
                                m_UV.emplace_back(maxUV.x, minUV.y); // bottom right
                                m_UV.emplace_back(maxUV); // top right
                                m_UV.emplace_back(minUV.x, maxUV.y); // top left

                                ++count;
                            }
                        }

                        DrawInstanced(count, meshIndex, GL_TRIANGLES);

                        count = 0;
                    }
                    break;
                    default:
                        break;
                    }

                    renderedEntities.emplace_back(id);

                    // continue from the next one after it
                    auto ite = ++(std::find(r_rendererIdContainer.begin(), r_rendererIdContainer.end(), id));
                    std::vector<size_t> cont;
                    while (ite != r_rendererIdContainer.end())
                    {
                        cont.emplace_back(*ite);
                        ++ite;
                    }
                    DrawQuadsInstanced<T>(r_worldToNdc, cont);
                    return;
                }

                // Skip this object if it has no renderer
                if (!EntityManager::GetInstance().Has<T>(id))
                {
                    return;
                }

                T& renderer{ EntityManager::GetInstance().Get<T>(id) };

                // Skip drawing this object is the entity or renderer is not enabled
                if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive
                    || !renderer.GetEnabled()/* || !Hierarchy::GetInstance().AreParentsActive(id)*/) {
                    continue;
                }

                // Store the index of the rendered entity
                renderedEntities.emplace_back(id);

                const Transform& transform{ EntityManager::GetInstance().Get<Transform>(id) };

                // Attempt to retrieve and bind the texture
                if (renderer.GetTextureKey().empty())
                {
                    m_isTextured.emplace_back(0.f);
                }
                else if (currentTexture != renderer.GetTextureKey())
                {
                    // Check if we were already 
                    if (!currentTexture.empty())
                    {
                        DrawInstanced(count, meshIndex, GL_TRIANGLES);
                        currentTexture.clear();
                        count = 0;
                    }

                    std::shared_ptr<Texture> texture{ ResourceManager::GetInstance().GetTexture(renderer.GetTextureKey()) };

                    // Check if texture is null
                    if (!texture)
                    {
                        // Remove the texture and set the object to neon pink
                        renderer.SetTextureKey("");
                        renderer.SetColor(1.f, 0.f, 1.f, 1.f);

                        m_isTextured.emplace_back(0.f);
                    }
                    else
                    {
                        // Unbind the existing texture
                        if (p_texture)
                        {
                            p_texture->Unbind();
                        }

                        // Store the texture key of the current texture
                        currentTexture = renderer.GetTextureKey();

                        // Bind the new texture
                        GLint textureUnit{ 0 };
                        p_texture = texture;
                        p_texture->Bind(textureUnit);
                        r_shaderProgram.SetUniform("uTextureSampler2d", textureUnit);

                        m_isTextured.emplace_back(1.f);
                    }
                }
                else
                {
                    m_isTextured.emplace_back(1.f);
                }

                // Add the matrix and colors to the buffer
                m_modelToWorldMatrices.emplace_back(GenerateTransformMatrix(transform.width, // width
                    transform.height, transform.orientation, // height, orientation
                    transform.position.x, transform.position.y)); // x, y position
                m_colors.emplace_back(renderer.GetColor());

                // Add the UV coordinate adjustments
                m_UV.emplace_back(renderer.GetUVCoordinatesMin()); // bottom left
                m_UV.emplace_back(renderer.GetUVCoordinatesMax().x, renderer.GetUVCoordinatesMin().y); // bottom right
                m_UV.emplace_back(renderer.GetUVCoordinatesMax()); // top right
                m_UV.emplace_back(renderer.GetUVCoordinatesMin().x, renderer.GetUVCoordinatesMax().y); // top left

                ++count;
            }

            // Draw the remaining objects
            DrawInstanced(count, meshIndex, GL_TRIANGLES);

            // Unbind everything
            m_meshes[meshIndex].Unbind();
            r_shaderProgram.UnUse();

            if (p_texture != nullptr)
            {
                p_texture->Unbind();
            }


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
            for (const auto& layer : LayerView<Collider>())
            {
                for (const EntityID& id : InternalView(layer))
                {
                    // Don't draw anything if the entity is inactive
                    if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive/* || !Hierarchy::GetInstance().AreParentsActive(id)*/) { continue; }

                    Collider& collider{ EntityManager::GetInstance().Get<Collider>(id) };

                    std::visit([&](auto& col)
                        {
                            DrawCollider(col, r_worldToNdc,
                            *(shaderProgramIterator->second));
                        }, collider.colliderVariant);
                }
            }

            // Draw a point and line for each rigidbody representing the position and velocity
            for (const auto& layer : LayerView<RigidBody>())
            {
                for (const EntityID& id : InternalView(layer))
                {
                    // Don't draw anything if the entity is inactive
                    if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive/* || !Hierarchy::GetInstance().AreParentsActive(id)*/) { continue; }

                    RigidBody& rigidbody{ EntityManager::GetInstance().Get<RigidBody>(id) };
                    Transform& transform{ EntityManager::GetInstance().Get<Transform>(id) };

                    glm::vec2 glmPosition{ transform.position.x, transform.position.y };

                    // Draw a line that represents the velocity
                    DrawDebugLine(glm::vec2{ rigidbody.velocity.x, rigidbody.velocity.y },
                        glmPosition, r_worldToNdc, *(shaderProgramIterator->second));

                    // Draw a point at the center of the object
                    DrawDebugPoint(glmPosition, r_worldToNdc, *(shaderProgramIterator->second));
                }
            }

            // Draw a "+" for every camera component
            for (const auto& layer : LayerView<Camera, Transform>())
            {
                for (const EntityID& id : InternalView(layer))
                {
                    // Don't draw anything if the entity is inactive
                    if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive/* || !Hierarchy::GetInstance().AreParentsActive(id)*/) { continue; }

                    // Don't draw a cross for the UI camera
                    if (id == r_cameraManager.GetUiCameraId()) { continue; }

                    Camera& camera{ EntityManager::GetInstance().Get<Camera>(id) };
                    Transform& transform{ EntityManager::GetInstance().Get<Transform>(id) };

                    glm::vec2 glmPosition{ transform.position.x, transform.position.y };

                    // Draw a cross to represent the orientation and position of the camera
                    DrawDebugCross(glmPosition, camera.GetUpVector(transform.orientation) * 100.f,
                        camera.GetRightVector(transform.orientation) * 100.f, r_worldToNdc, *(shaderProgramIterator->second));
                }
            }

            glLineWidth(2.f);

            // Draw a rectangle for every canvas component
            for (auto const& id : GUISystem::GetActiveCanvases())
            {
                if (!EntityManager::GetInstance().Has<Canvas>(id))
                    continue;

                Canvas& canvasComponent{ EntityManager::GetInstance().Get<Canvas>(id) };
                glm::vec2 position{ 0.f, 0.f };
                if (EntityManager::GetInstance().Has<Transform>(id))
                {
                    Transform& transformComponent{ EntityManager::GetInstance().Get<Transform>(id) };
                    position.x = transformComponent.position.x;
                    position.y = transformComponent.position.y;
                }

                // Draw a cross to represent the orientation and position of the canvas
                DrawDebugRectangle(canvasComponent.GetWidth(), canvasComponent.GetHeight(),
                    0.f, position.x, position.y, r_worldToNdc, *(shaderProgramIterator->second),
                    glm::vec4{ 1.f, 1.f, 1.f, 1.f });
            }

            // Draw a rectangle for all the text component bounds
            for (const auto& layer : LayerView<TextComponent, Transform>())
            {
                for (auto const& id : InternalView(layer))
                {
                    // Don't draw anything if the entity is inactive
                    if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive || !GETGUISYSTEM()->IsChildedToCanvas(id)) { continue; }

                    Transform& transformComponent{ EntityManager::GetInstance().Get<Transform>(id) };

                    // Draw a cross to represent the orientation and position of the text bounds
                    DrawDebugRectangle(transformComponent.width, transformComponent.height,
                        transformComponent.orientation,
                        transformComponent.position.x, transformComponent.position.y,
                        r_worldToNdc, *(shaderProgramIterator->second),
                        glm::vec4{ 0.3f, 0.3f, 1.f, 1.f });
                }
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

            if (meshType == EnumMeshType::QUAD || meshType == EnumMeshType::TRIANGLE)
                ++objectDrawCalls;
            else
                ++debugDrawCalls;
        }


        void RendererManager::Draw(EnumMeshType meshType, glm::vec4 const& r_color,
            GLuint textureId, ShaderProgram& r_shaderProgram,
            GLenum const primitiveType, glm::mat4 const& r_modelToNdc)
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

            // Bind the texture
            GLint textureUnit{ 0 };
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, textureId);
            r_shaderProgram.SetUniform("uTextureSampler2d", textureUnit);
            r_shaderProgram.SetUniform("uIsTextured", true);


            glDrawElements(primitiveType, static_cast<GLsizei>(m_meshes[meshIndex].indices.size()),
                GL_UNSIGNED_SHORT, NULL);

            // Unbind everything
            m_meshes[meshIndex].Unbind();
            r_shaderProgram.UnUse();
            glBindTexture(GL_TEXTURE_2D, 0);

            if (meshType == EnumMeshType::QUAD || meshType == EnumMeshType::TRIANGLE)
                ++objectDrawCalls;
            else
                ++debugDrawCalls;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------------

                //void RendererManager::Draw(Renderer& r_renderer, ShaderProgram& r_shaderProgram,
                //    GLenum const primitiveType, glm::mat4 const& r_modelToNdc)
                //{
                //    r_shaderProgram.Use();

                //    // Check if mesh index is valid
                //    unsigned char meshIndex{ static_cast<unsigned char>(r_renderer.GetMeshType()) };
                //    if (meshIndex >= m_meshes.size())
                //    {
                //        engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                //        engine_logger.SetTime();
                //        engine_logger.AddLog(false, "Mesh type is invalid.", __FUNCTION__);
                //        return;
                //    }

                //    m_meshes[meshIndex].Bind();

                //    // Attempt to retrieve and bind the texture
                //    std::shared_ptr<Graphics::Texture> p_texture{};

                //    if (r_renderer.GetTextureKey().empty()) 
                //    {
                //        r_shaderProgram.SetUniform("uIsTextured", false);
                //    }
                //    else 
                //    {
                //        std::shared_ptr<Texture> texture{ ResourceManager::GetInstance().GetTexture(r_renderer.GetTextureKey()) };

                //        // Check if texture is null
                //        if (!texture)
                //        {
                //            // Remove the texture and set the object to neon pink
                //            r_renderer.SetTextureKey("");
                //            r_renderer.SetColor(1.f, 0.f, 1.f, 1.f);

                //            r_shaderProgram.SetUniform("uIsTextured", false);
                //        }
                //        else 
                //        {
                //            p_texture = texture;
                //            GLint textureUnit{ 0 };
                //            p_texture->Bind(textureUnit);
                //            r_shaderProgram.SetUniform("uTextureSampler2d", textureUnit);
                //            r_shaderProgram.SetUniform("uIsTextured", true);
                //        }
                //    }

                //    // Pass the model to NDC transform matrix as a uniform variable
                //    r_shaderProgram.SetUniform("uModelToNdc", r_modelToNdc);

                //    // Pass the color of the quad as a uniform variable
                //    r_shaderProgram.SetUniform("uColor", r_renderer.GetColor());


                //    glDrawElements(primitiveType, static_cast<GLsizei>(m_meshes[meshIndex].indices.size()),
                //        GL_UNSIGNED_SHORT, NULL);

                //    // Unbind everything
                //    m_meshes[meshIndex].Unbind();
                //    r_shaderProgram.UnUse();

                //    if (p_texture != nullptr)
                //    {
                //        p_texture->Unbind();
                //    }

                //    ++objectDrawCalls;
                //}   

        //--------------------------------------------------------------------------------------------------------------------------------------------


        void RendererManager::DrawInstanced(size_t const count, size_t const meshIndex, GLenum const primitiveType)
        {
            if (!count) { return; }

            // Store the sizes of the sub buffers
            size_t sizeOfTexturedVector{ m_isTextured.size() * sizeof(float) };
            size_t sizeOfColorVector{ m_colors.size() * sizeof(glm::vec4) };
            size_t sizeOfMatrixVector{ m_modelToWorldMatrices.size() * sizeof(glm::mat4) };
            size_t sizeOfUVVector{ m_UV.size() * sizeof(glm::vec2) };


            // Create buffer object for additional vertex data
            GLuint vertexBufferObject{}, vertexArrayObjectIndex{ m_meshes[meshIndex].GetVertexArrayObjectIndex() };
            glCreateBuffers(1, &vertexBufferObject);
            glNamedBufferStorage(vertexBufferObject,
                static_cast<GLsizeiptr>(sizeOfTexturedVector + sizeOfColorVector + sizeOfMatrixVector + sizeOfUVVector),
                nullptr, GL_DYNAMIC_STORAGE_BIT);


            // Store UV coordinates in VBO
            glNamedBufferSubData(vertexBufferObject, 0,
                static_cast<GLsizeiptr>(sizeOfUVVector),
                reinterpret_cast<GLvoid*>(m_UV.data()));

            // Bind the UV coordinates
            GLuint attributeIndex{ 2 }, bindingIndex{ 2 };
            glEnableVertexArrayAttrib(vertexArrayObjectIndex, attributeIndex);
            glVertexArrayVertexBuffer(vertexArrayObjectIndex, bindingIndex, vertexBufferObject, 0,
                static_cast<GLsizei>(sizeof(glm::vec2)));
            glVertexArrayAttribFormat(vertexArrayObjectIndex, attributeIndex, 2, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vertexArrayObjectIndex, attributeIndex, bindingIndex);

            // Store textured bools in VBO
            glNamedBufferSubData(vertexBufferObject, static_cast<GLintptr>(sizeOfUVVector),
                static_cast<GLsizeiptr>(sizeOfTexturedVector),
                reinterpret_cast<GLvoid*>(m_isTextured.data()));

            // Bind the textured bools            
            ++attributeIndex, ++bindingIndex;
            glEnableVertexArrayAttrib(vertexArrayObjectIndex, attributeIndex);
            glVertexArrayVertexBuffer(vertexArrayObjectIndex, bindingIndex, vertexBufferObject, static_cast<GLintptr>(sizeOfUVVector),
                static_cast<GLsizei>(sizeof(float)));
            glVertexArrayAttribFormat(vertexArrayObjectIndex, attributeIndex, 1, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vertexArrayObjectIndex, attributeIndex, bindingIndex);
            glVertexAttribDivisor(attributeIndex, 1); // Advance once per instance 

            // Store colors in VBO
            glNamedBufferSubData(vertexBufferObject,
                static_cast<GLintptr>(sizeOfUVVector + sizeOfTexturedVector),
                static_cast<GLsizeiptr>(sizeOfColorVector),
                reinterpret_cast<GLvoid*>(m_colors.data()));

            // Bind the colors
            ++attributeIndex, ++bindingIndex;
            glEnableVertexArrayAttrib(vertexArrayObjectIndex, attributeIndex);
            glVertexArrayVertexBuffer(vertexArrayObjectIndex, bindingIndex, vertexBufferObject,
                static_cast<GLintptr>(sizeOfUVVector + sizeOfTexturedVector), static_cast<GLsizei>(sizeof(glm::vec4)));
            glVertexArrayAttribFormat(vertexArrayObjectIndex, attributeIndex, 4, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vertexArrayObjectIndex, attributeIndex, bindingIndex);
            glVertexAttribDivisor(attributeIndex, 1); // Advance once per instance

            // Store model to NDC matrices in VBO
            glNamedBufferSubData(vertexBufferObject,
                static_cast<GLintptr>(sizeOfUVVector + sizeOfTexturedVector + sizeOfColorVector),
                static_cast<GLsizeiptr>(sizeOfMatrixVector),
                reinterpret_cast<GLvoid*>(m_modelToWorldMatrices.data()));

            // Bind the model to NDC matrices
            ++attributeIndex, ++bindingIndex;

            glVertexArrayVertexBuffer(vertexArrayObjectIndex, bindingIndex, vertexBufferObject,
                static_cast<GLintptr>(sizeOfUVVector + sizeOfTexturedVector + sizeOfColorVector),
                static_cast<GLsizei>(sizeof(glm::mat4)));

            for (int i{}; i < 4; ++i)
            {
                glEnableVertexArrayAttrib(vertexArrayObjectIndex, attributeIndex);

                glVertexArrayAttribFormat(vertexArrayObjectIndex, attributeIndex, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4 * i); // offset by vert pos and color
                glVertexArrayAttribBinding(vertexArrayObjectIndex, attributeIndex, bindingIndex);
                ++attributeIndex;
            }
            // Advance once per instance
            glVertexArrayBindingDivisor(vertexArrayObjectIndex, bindingIndex, 1);

            // ------------------------------------------- FOR M2 RUBRIC 1124, ADD BREAKPOINT TO THIS LINE -------------//
            // Make instanced draw
            glDrawElementsInstanced(primitiveType, static_cast<GLsizei>(m_meshes[meshIndex].indices.size()),
                GL_UNSIGNED_SHORT, NULL, static_cast<GLsizei>(count));

            // Delete the buffer
            glDeleteBuffers(1, &vertexBufferObject);

            --attributeIndex;
            while (attributeIndex >= 2)
            {
                glDisableVertexArrayAttrib(vertexArrayObjectIndex, attributeIndex--);
            }

            // Clear the vectors
            m_isTextured.clear();
            m_colors.clear();
            m_UV.clear();
            m_modelToWorldMatrices.clear();

            ++objectDrawCalls;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------------

                //void RendererManager::DrawUi(glm::mat4 const& r_viewToNdc)
                //{
                //    if (!GETGUISYSTEM()->AreThereActiveCanvases()) { return; }

                //    std::vector<EntityID> guiToDraw{};
                //    guiToDraw.reserve(100);

                //    for (EntityID objectID : SceneView<GUIRenderer>())
                //    {
                //        // Check if the object is childed to a canvas object
                //        bool isChild{ GETGUISYSTEM()->IsChildedToCanvas(objectID) };
                //        if (!isChild)
                //        {
                //            continue; // If not childed to a canvas or canvas is inactive, don't update
                //        }

                //        // Check if the object has a renderer component
                //        if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(objectID))
                //        {
                //            // Store it to be drawn
                //            if (EntityManager::GetInstance().Get<GUIRenderer>(objectID).GetEnabled())
                //            {
                //                guiToDraw.emplace_back(objectID);
                //            }
                //        }
                //    } // end of sceneview loop

                //    DrawQuadsInstanced(r_viewToNdc, guiToDraw, true);
                //}

        //--------------------------------------------------------------------------------------------------------------------------------------------

        void RendererManager::DrawCollider(AABBCollider const& r_aabbCollider,
            glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
            glm::vec4 const& r_color)
        {
            // Derive the scale and position of the debug shape to draw
            // from the bounds of the AABB collider
            DrawDebugRectangle(r_aabbCollider.max.x - r_aabbCollider.min.x, // width
                r_aabbCollider.max.y - r_aabbCollider.min.y, // height
                0.f, // orientation
                (r_aabbCollider.min.x + r_aabbCollider.max.x) * 0.5f, // x position
                (r_aabbCollider.min.y + r_aabbCollider.max.y) * 0.5f, // y position
                r_worldToNdc, r_shaderProgram, r_color);
        }


        void RendererManager::DrawCollider(CircleCollider const& r_circleCollider,
            glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
            glm::vec4 const& r_color)
        {
            // Derive the scale and position of the debug shape to draw
            // from the radius and position of the collider
            float const diameter{ r_circleCollider.radius * 2.f };

            glm::mat4 modelToWorld
            {
                GenerateTransformMatrix(diameter, diameter, 0.f, // width, height, orientation
                    r_circleCollider.center.x, r_circleCollider.center.y) // x, y position
            };

            Draw(EnumMeshType::DEBUG_CIRCLE, r_color,
                r_shaderProgram, GL_LINES, r_worldToNdc * modelToWorld);
        }


        void RendererManager::DrawDebugRectangle(float const width, float const height,
            float const orientation, float const xPosition, float const yPosition,
            glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
            glm::vec4 const& r_color)
        {
            // Derive the scale and position of the debug shape to draw
            // from the bounds of the AABB collider
            glm::mat4 modelToWorld
            {
                GenerateTransformMatrix(width, height, orientation, // width, height, orientation
                    xPosition, yPosition) // x position, y position
            };

            Draw(EnumMeshType::DEBUG_SQUARE, r_color,
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


        void RendererManager::DrawDebugCross(glm::vec2 const& r_position,
            glm::vec2 const& r_upVector, glm::vec2 const& r_rightVector,
            glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
            glm::vec4 const& r_color)
        {
            // Draw vertical line then horizontal line
            DrawDebugLine(r_upVector, r_position - r_upVector * 0.5f, r_worldToNdc, r_shaderProgram, r_color);
            DrawDebugLine(r_rightVector, r_position - r_rightVector * 0.5f, r_worldToNdc, r_shaderProgram, r_color);
        }


        void RendererManager::RenderText(glm::mat4 const& r_worldToNdc)
        {
            // Don't bother if there are no active canvases
            if (!GETGUISYSTEM()->AreThereActiveCanvases()) { return; }

            std::shared_ptr<ShaderProgram> p_textShader{ ResourceManager::GetInstance().ShaderPrograms[m_textShaderProgramKey] };
            for (const auto& layer : LayerView<TextComponent, Transform>())
            {
                for (const EntityID& id : InternalView(layer))
                {
                    // Don't draw anything if the entity is inactive or is not childed to a canvas
                    if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive || !GETGUISYSTEM()->IsChildedToCanvas(id) /*|| !Hierarchy::GetInstance().AreParentsActive(id)*/) { continue; }

                    TextComponent const& textComponent{ EntityManager::GetInstance().Get<TextComponent>(id) };
                    TextBox textBox{ EntityManager::GetInstance().Get<Transform>(id).position,
                                      EntityManager::GetInstance().Get<Transform>(id).width,
                                      EntityManager::GetInstance().Get<Transform>(id).height };

                    // if component has no font key
                    if (textComponent.GetFontKey() == "")
                    {
                        break;
                    }

                    // Store the index of the rendered entity
                    renderedEntities.emplace_back(id);

                    std::shared_ptr<Font> p_font{ ResourceManager::GetInstance().GetFont(textComponent.GetFontKey()) };

                    std::vector<std::string> lines{ SplitTextIntoLines(textComponent, textBox) };
                    float currentY{ 0.f };
                    float hAlignOffset, vAlignOffset;

                    // activate corresponding render state	
                    p_textShader->Use();
                    p_textShader->SetUniform("u_ViewProjection", r_worldToNdc);
                    p_textShader->SetUniform("textColor", textComponent.GetColor());

                    glActiveTexture(GL_TEXTURE0);
                    glBindVertexArray(p_font->vertexArrayObject);

                    // get vertical alignment offset
                    VerticalTextAlignment(textComponent, lines, textBox, vAlignOffset);

                    // render line
                    for (std::string const& line : lines)
                    {
                        // get horizontal alignment offset
                        HorizontalTextAlignment(textComponent, line, textBox, hAlignOffset);

                        RenderLine(textComponent, line, textBox.position, currentY, hAlignOffset, vAlignOffset);

                        // add line height to current y, need to multiply by line spacing
                        currentY += p_font->lineHeight * textComponent.GetLineSpacing();
                    }

                    glBindTexture(GL_TEXTURE_2D, 0);
                    glBindVertexArray(0);

                    p_textShader->UnUse();
                }
            }
        }

        void RendererManager::RenderText(const EntityID& r_id, glm::mat4 const& r_worldToNdc)
        {
            // Don't bother if there are no active canvases
            if (!GETGUISYSTEM()->AreThereActiveCanvases()) { return; }

            std::shared_ptr<ShaderProgram> p_textShader{ ResourceManager::GetInstance().ShaderPrograms[m_textShaderProgramKey] };

            // Don't draw anything if the entity is inactive or is not childed to a canvas
            if (!EntityManager::GetInstance().Get<EntityDescriptor>(r_id).isActive || !GETGUISYSTEM()->IsChildedToCanvas(r_id) /*|| !Hierarchy::GetInstance().AreParentsActive(id)*/) { return; }

            TextComponent const& textComponent{ EntityManager::GetInstance().Get<TextComponent>(r_id) };
            TextBox textBox{ EntityManager::GetInstance().Get<Transform>(r_id).position,
                              EntityManager::GetInstance().Get<Transform>(r_id).width,
                              EntityManager::GetInstance().Get<Transform>(r_id).height };

            // if component has no font key
            if (textComponent.GetFontKey() == "")
            {
                return;
            }

            // Store the index of the rendered entity
            renderedEntities.emplace_back(r_id);

            std::shared_ptr<Font> p_font{ ResourceManager::GetInstance().GetFont(textComponent.GetFontKey()) };

            std::vector<std::string> lines{ SplitTextIntoLines(textComponent, textBox) };
            float currentY{ 0.f };
            float hAlignOffset, vAlignOffset;

            // activate corresponding render state	
            p_textShader->Use();
            p_textShader->SetUniform("u_ViewProjection", r_worldToNdc);
            p_textShader->SetUniform("textColor", textComponent.GetColor());

            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(p_font->vertexArrayObject);

            // get vertical alignment offset
            VerticalTextAlignment(textComponent, lines, textBox, vAlignOffset);

            // render line
            for (std::string const& line : lines)
            {
                // get horizontal alignment offset
                HorizontalTextAlignment(textComponent, line, textBox, hAlignOffset);

                RenderLine(textComponent, line, textBox.position, currentY, hAlignOffset, vAlignOffset);

                // add line height to current y, need to multiply by line spacing
                currentY += p_font->lineHeight * textComponent.GetLineSpacing();
            }

            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);

            p_textShader->UnUse();
        }

        void RendererManager::RenderLine(TextComponent const& r_textComponent, std::string const& r_line, vec2 position, float currentY, float hAlignOffset, float vAlignOffset)
        {
            // iterate through all characters
            std::string::const_iterator c;

            std::shared_ptr<Font> p_font{ ResourceManager::GetInstance().GetFont(r_textComponent.GetFontKey()) };

            float currentX{ 0.f };

            for (c = r_line.begin(); c != r_line.end(); c++)
            {
                Character ch = p_font->characters.at(*c);

                currentX += ch.size.x;

                // calculate position to render character based on bearings, and alignment offset
                float xPosition = position.x + ch.bearing.x * r_textComponent.GetSize() + hAlignOffset;
                float yPosition = position.y - (ch.size.y - ch.bearing.y + currentY) * r_textComponent.GetSize() + vAlignOffset;

                float w = ch.size.x * r_textComponent.GetSize();
                float h = ch.size.y * r_textComponent.GetSize();
                // update VBO for each character
                float vertices[6][4] = {
                    { xPosition,     yPosition + h,   0.0f, 0.0f },
                    { xPosition ,    yPosition,       0.0f, 1.0f },
                    { xPosition + w, yPosition,       1.0f, 1.0f },

                    { xPosition,     yPosition + h,   0.0f, 0.0f },
                    { xPosition + w, yPosition ,      1.0f, 1.0f },
                    { xPosition + w, yPosition + h,   1.0f, 0.0f }
                };

                // render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, ch.textureID);

                // update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, p_font->vertexBufferObject);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);

                ++textDrawCalls;
                // now advance cursors for next glyph
                position.x += (ch.advance >> 6) * r_textComponent.GetSize(); // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
            }
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
                    glm::vec2{ glm::cos(totalAngle) * 0.5f, glm::sin(totalAngle) * 0.5f },
                    glm::vec2{ 0.f, 0.f });

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
            r_mesh.vertices.emplace_back(glm::vec2{ -0.5f, -0.5f }, glm::vec2{ 0.f, 0.f });
            // bottom-right
            r_mesh.vertices.emplace_back(glm::vec2{ 0.5f, -0.5f }, glm::vec2{ 1.f, 0.f });
            // top-center
            r_mesh.vertices.emplace_back(glm::vec2{ 0.f, 0.5f }, glm::vec2{ 0.5f, 1.f });


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
            r_mesh.vertices.emplace_back(glm::vec2{ -0.5f, -0.5f }, glm::vec2{ 0.f, 0.f });
            // bottom-right
            r_mesh.vertices.emplace_back(glm::vec2{ 0.5f, -0.5f }, glm::vec2{ 1.f, 0.f });
            // top-right
            r_mesh.vertices.emplace_back(glm::vec2{ 0.5f, 0.5f }, glm::vec2{ 1.f, 1.f });
            // top-left
            r_mesh.vertices.emplace_back(glm::vec2{ -0.5f, 0.5f }, glm::vec2{ 0.f, 1.f });


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
            r_mesh.vertices.emplace_back(glm::vec2{ -0.5f, -0.5f }, glm::vec2{ 0.f, 0.f });
            // bottom-right
            r_mesh.vertices.emplace_back(glm::vec2{ 0.5f, -0.5f }, glm::vec2{ 1.f, 0.f });
            // top-right
            r_mesh.vertices.emplace_back(glm::vec2{ 0.5f, 0.5f }, glm::vec2{ 1.f, 1.f });
            // top-left
            r_mesh.vertices.emplace_back(glm::vec2{ -0.5f, 0.5f }, glm::vec2{ 0.f, 1.f });

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

            r_mesh.vertices.emplace_back(glm::vec2{ -0.5f, 0.f }, glm::vec2{ 0.f, 0.f });
            r_mesh.vertices.emplace_back(glm::vec2{ 0.5f, 0.f }, glm::vec2{ 1.f, 0.f });

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

            r_mesh.vertices.emplace_back(glm::vec2{ 0.f, 0.f }, glm::vec2{ 0.f, 0.f });

            // Add indices
            r_mesh.indices.clear();
            r_mesh.indices.resize(1);

            // Generate VAO
            r_mesh.CreateVertexArrayObject();
        }


        glm::mat4 RendererManager::GenerateTransformMatrix(float const width, float const height,
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


        glm::mat4 RendererManager::GenerateInverseTransformMatrix(float const width, float const height,
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


        void APIENTRY GlDebugOutput(GLenum source, GLenum type, unsigned int id,
            GLenum severity, GLsizei length, const char* message, const void* userParam)
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
    } // End of Graphics namespace
} // End of PE namespace