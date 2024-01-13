/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     NewRendererManager.cpp
 \date:    11-12-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief    This file contains the NewRendererManager class, which manages the 
           objects to be drawn and renderers being used.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"

#include "NewRendererManager.h"
#include "GraphicsUtilities.h"
#include "Graphics/Text.h"
#include "CameraManager.h"

#include "WindowManager.h"
#include "ResourceManager/ResourceManager.h"

// ECS Components
#include "ECS/Entity.h"
#include "ECS/EntityFactory.h"
#include "Hierarchy/HierarchyManager.h"

// ImGui
#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE


//#define MY_DEBUG

namespace PE
{
    namespace Graphics
    {
        // Initialize static variables
        std::vector<EntityID> NewRendererManager::renderedEntities{};
        unsigned NewRendererManager::totalDrawCalls{};   // Sum total of all draw calls made
        unsigned NewRendererManager::textDrawCalls{};    // Total draw calls made for text (1 draw call per chara)
        unsigned NewRendererManager::objectDrawCalls{};  // Total draw calls for gameobjects
        unsigned NewRendererManager::debugDrawCalls{};   // Total draw calls for debug shapes

        NewRendererManager::NewRendererManager(int const _width, int const _height)
            : m_targetWidth{ _width }, m_targetHeight{ _height }, 
            m_targetAspectRatio{static_cast<float>(_width) / static_cast<float>(_height)}
        {
            // Initialize GLEW
            if (glewInit() != GLEW_OK)
            {
                std::cout << "Failed to initialize GLEW\n";
                throw;
            }
        }


        void NewRendererManager::InitializeSystem()
        {
            // Retrieve and store data about GPU
            RetrieveSpecifications();
            PrintSpecifications();

            // Configure OpenGL state
            ConfigureOpenGlState();

            // Reserve sufficient space in the containers
            ReserveContainersSpace(1000U);


            // Load shader programs
            // We load our shader programs here because we don't foresee ourselves needing different shaderprograms for basic rendering
            ResourceManager::GetInstance().LoadShadersFromFile(m_texturedShaderProgramKey, "../Shaders/TexturedDefault.vert", "../Shaders/TexturedDefault.frag");
            ResourceManager::GetInstance().LoadShadersFromFile(m_coloredShaderProgramKey, "../Shaders/ColoredDefault.vert", "../Shaders/ColoredDefault.frag");
            ResourceManager::GetInstance().LoadShadersFromFile(m_textShaderProgramKey, "../Shaders/Text.vert", "../Shaders/Text.frag");


            // Create framebuffers
            int width, height;
            glfwGetWindowSize(WindowManager::GetInstance().GetWindow(), &width, &height);

            m_frameBuffers.resize(static_cast<std::size_t>(EnumFrameBuffer::FRAMEBUFFER_COUNT));
            m_frameBuffers[static_cast<std::size_t>(EnumFrameBuffer::EDIT_MODE)].CreateFrameBuffer(width, height, true, false, "Edit FBO");
            m_frameBuffers[static_cast<std::size_t>(EnumFrameBuffer::PLAY_MODE)].CreateFrameBuffer(width, height, true, false, "Play FBO");

            // Update the size fo the framebuffers
            UpdatePlayWindowSize();
#ifndef GAMERELEASE
            UpdateSceneWindowSize();
#endif !GAMERELEASE


            // Create renderers and configure
            // We only have one as we do separate passes for separate framebuffers
            m_batchRenderer.Initialize();
            m_batchRenderer.SetTexturedShaderProgram(m_texturedShaderProgramKey);
            m_batchRenderer.SetColoredShaderProgram(m_coloredShaderProgramKey);

            // Add key event 
            keyEventHandler = ADD_KEY_EVENT_LISTENER(KeyEvents::KeyTriggered, NewRendererManager::OnKeyTriggeredEvent, this);
        }


        void NewRendererManager::UpdateSystem(float deltaTime)
        {
            //@CHECK krystal

            deltaTime; // prevent warnings

            GLFWwindow* const p_win{ WindowManager::GetInstance().GetWindow() };
            if (!p_win)
            {
                return;
            }

            UpdatePlayWindowSize();
#ifndef GAMERELEASE
            UpdateSceneWindowSize();
#endif !GAMERELEASE

            // Set background color of the window
            ClearBackground(0.f, 0.f, 0.f, 1.f);

            // Retrieve text objects
            // Sort into world space text and screen space text
            SortTextObjects();

            // Get the world to NDC matrix
            glm::mat4 worldToNdcMatrix{ GETCAMERAMANAGER()->GetWorldToNdcMatrix(false)};


#ifndef GAMERELEASE
            FrameBuffer& playBuffer{ m_frameBuffers[static_cast<std::size_t>(EnumFrameBuffer::PLAY_MODE)] };
            playBuffer.Bind();
#endif //!GAMERELEASE

            SetViewport(m_cachedPlayWindowWidth, m_cachedPlayWindowHeight);

            ClearBackground(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

            // Render game objects if there is a main camera
            if (GETCAMERAMANAGER()->GetMainCamera().has_value()) 
            {
                DrawGameObjects(worldToNdcMatrix);
            }

            // Render UI
            DrawUi(GETCAMERAMANAGER()->GetUiViewToNdcMatrix());

            // Render debug shapes
            DrawDebug(worldToNdcMatrix);

#ifndef GAMERELEASE
            playBuffer.Unbind();
#endif //!GAMERELEASE

#ifndef GAMERELEASE
            // ----- In the editor, if the editor is active, draw the scene window ----- //

            if (Editor::GetInstance().IsEditorActive())
            {
                FrameBuffer& editorBuffer{ m_frameBuffers[static_cast<std::size_t>(EnumFrameBuffer::EDIT_MODE)] };
                editorBuffer.Bind(); 
                SetViewport(m_cachedSceneWindowWidth, m_cachedSceneWindowHeight);

                ClearBackground(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

                worldToNdcMatrix = GETCAMERAMANAGER()->GetWorldToNdcMatrix(true);

                // Render game objects
                DrawGameObjects(worldToNdcMatrix);

                // Render UI
                DrawUi(worldToNdcMatrix);

                // Render debug shapes
                DrawDebug(worldToNdcMatrix);

                editorBuffer.Unbind();

                // Update Editor windows
                GLuint sceneTexId{ editorBuffer.GetTextureId() }, playTexId{ playBuffer.GetTextureId() };
                Editor::GetInstance().Render(editorBuffer.GetTextureId(), playBuffer.GetTextureId());
            }
            else
            {
                // Update Editor windows
                GLuint playTexId{ playBuffer.GetTextureId() };
                Editor::GetInstance().Render(0, playBuffer.GetTextureId());
            }
#else 

            // Draw the camera quad
            //DrawScreenQuad(playBuffer.GetTextureId());

#endif

            // Poll for and process events
            glfwPollEvents(); // should be called before glfwSwapbuffers

            // Swap front and back buffers
            glfwSwapBuffers(WindowManager::GetInstance().GetWindow());
        }


        void NewRendererManager::DestroySystem()
        {
            // Unsubscribe event
            REMOVE_KEY_EVENT_LISTENER(keyEventHandler);

            // Delete the framebuffers
            for (FrameBuffer& fbo : m_frameBuffers)
            {
                fbo.Cleanup();
            }

            // Clean up the renderer
            m_batchRenderer.Cleanup();
        }


        void NewRendererManager::AddDebugShape(RenderData const& r_renderData)
        {
            switch (r_renderData.meshType)
            {
            case EnumMeshType::DEBUG_CIRCLE:
            {
                m_debugCircles.emplace_back(r_renderData);
                break;
            }
            case EnumMeshType::DEBUG_SQUARE:
            {
                m_debugRectangles.emplace_back(r_renderData);
                break;
            }
            case EnumMeshType::DEBUG_LINE:
            {
                m_debugLines.emplace_back(r_renderData);
                break;
            }
            case EnumMeshType::DEBUG_POINT:
            {
                m_debugPoints.emplace_back(r_renderData);
                break;
            }
            default: break;
            }
        }


        void NewRendererManager::DrawGameObjects(glm::mat4 const& r_worldToNdc)
        {
            //@CHECK krystal

            // Don't bother rendering anything if there is no main camera
            if (!(GETCAMERAMANAGER()->GetMainCamera().has_value())) { return; }
            
            // Loop through the renderable objects
            std::vector<EntityID> const& renderableContainer{ Hierarchy::GetInstance().GetRenderOrder() };
            for (EntityID const& id : renderableContainer)
            {
#ifdef MY_DEBUG
                std::cout << "NewRendererManager::DrawGameObjects(): Rendering entity " << id << "\n";
#endif // !MY_DEBUG

                // @FUTURE krystal -> add a check for whether this has a text component or a renderer component

                if (!EntityManager::GetInstance().Has<Transform>(id))
                {
#ifdef MY_DEBUG
                    // if there's no transform
                    std::cout << "NewRendererManager::DrawGameObjects(): Attempted to render entity " << id << " but it does not have a transform component\n";

#endif // !MY_DEBUG
                    continue;
                }

                // Get transform
                Transform& transformComponent{ EntityManager::GetInstance().Get<Transform>(id) };
                
                // Get renderer
                Renderer& rendererComponent{ EntityManager::GetInstance().Get<Renderer>(id) };
                
                // Check if renderer is textured
                if (rendererComponent.GetIsTextured())
                {
                    /*
                       TexturedRenderData(EnumMeshType _meshType, float const _width, float const _height,
                            float const _orientation, glm::vec2 const& r_position, glm::vec4 const& r_color,
                            UVContainer const& r_uvCoordinates, TextureID const _textureId)
                    */
                    std::shared_ptr<Texture> const& textureObj{ ResourceManager::GetInstance().GetTexture(rendererComponent.GetTextureKey()) };

#ifdef MY_DEBUG
                    std::cout << "NewRendererManager::DrawGameObjects(): entity " << id << " is textured with texture " << textureObj->GetTextureID() << "\n";
#endif // !MY_DEBUG

                    // Construct textured render data + call draw function in batch renderer
                    m_batchRenderer.DrawQuad(
                        TexturedRenderData{
                            EnumMeshType::TEXTURED_QUAD, transformComponent.width, transformComponent.height,// meshtype, width, height
                            transformComponent.orientation, glm::vec2{transformComponent.position.x, transformComponent.position.y},// orientation, position
                            rendererComponent.GetColor(), // color
                            QuadMeshData::GetUvCoordinates(rendererComponent.GetUVCoordinatesMin(), rendererComponent.GetUVCoordinatesMax()), // uv coords
                            textureObj->GetTextureID() // texture
                        }, r_worldToNdc
                    );
                } 
                else
                {
                    /*
                       RenderData(EnumMeshType _meshType, float const _width, float const _height,
                            float const _orientation, glm::vec2 const& r_position, glm::vec4 const& r_color)
                    */
#ifdef MY_DEBUG
                    std::cout << "NewRendererManager::DrawGameObjects(): entity " << id << " is NOT textured\n";
#endif // !MY_DEBUG

                    // Construct render data + call draw function in batch renderer
                    m_batchRenderer.DrawQuad(
                        RenderData{
                            EnumMeshType::TEXTURED_QUAD, transformComponent.width, transformComponent.height,// meshtype, width, height
                            transformComponent.orientation, glm::vec2{transformComponent.position.x, transformComponent.position.y},// orientation, position
                            rendererComponent.GetColor(), // color
                        }, r_worldToNdc
                    );
                }                
            } // End of for (EntityID const& id : renderableContainer)

            m_batchRenderer.Flush(); // Ensure all objects have been rendered

#ifdef MY_DEBUG
            std::cout << "NewRendererManager::DrawGameObjects(): end renderer loop\n";
#endif // !MY_DEBUG
            // Loop through text objects
            //m_worldTextObjects
            DrawTextObjects(m_worldTextObjects, r_worldToNdc);
#ifdef MY_DEBUG
            std::cout << "NewRendererManager::DrawGameObjects(): exit\n";
#endif // !MY_DEBUG

            m_batchRenderer.Flush(); // Ensure all objects have been rendered
        }


        void NewRendererManager::DrawUi(glm::mat4 const& r_toNdc)
        {
            //@CHECK krystal

            // Don't bother if there are no active canvases
            if (!GETGUISYSTEM()->AreThereActiveCanvases()) { return; }

            // Loop through the renderable UI objects
            std::vector<EntityID> const& renderableContainer{ Hierarchy::GetInstance().GetRenderOrderUI() };
            for (EntityID const& id : renderableContainer)
            {
                // @FUTURE krystal -> add a check for whether this has a text component or a renderer component

                if (!EntityManager::GetInstance().Has<Transform>(id))
                {
#ifdef MY_DEBUG
                    // if there's no transform
                    std::cout << "NewRendererManager::DrawGameObjects(): Attempted to render entity " << id << " but it does not have a transform component\n";
#endif // !MY_DEBUG
                    
                    continue;
                }

                // Get transform
                Transform& transformComponent{ EntityManager::GetInstance().Get<Transform>(id) };

                // Get renderer
                GUIRenderer& rendererComponent{ EntityManager::GetInstance().Get<GUIRenderer>(id) };

                // Check if renderer is textured
                if (rendererComponent.GetIsTextured())
                {
                    /*
                       TexturedRenderData(EnumMeshType _meshType, float const _width, float const _height,
                            float const _orientation, glm::vec2 const& r_position, glm::vec4 const& r_color,
                            UVContainer const& r_uvCoordinates, TextureID const _textureId)
                    */

                    std::shared_ptr<Texture> const& textureObj{ ResourceManager::GetInstance().GetTexture(rendererComponent.GetTextureKey()) };

                    // Construct textured render data + call draw function in batch renderer
                    m_batchRenderer.DrawQuad(
                        TexturedRenderData{
                            EnumMeshType::TEXTURED_QUAD, transformComponent.width, transformComponent.height,// meshtype, width, height
                            transformComponent.orientation, glm::vec2{transformComponent.position.x, transformComponent.position.y},// orientation, position
                            rendererComponent.GetColor(), // color
                            QuadMeshData::GetUvCoordinates(rendererComponent.GetUVCoordinatesMin(), rendererComponent.GetUVCoordinatesMax()), // uv coords
                            textureObj->GetTextureID() // texture
                        }, r_toNdc
                    );
                }
                else
                {
                    /*
                       RenderData(EnumMeshType _meshType, float const _width, float const _height,
                            float const _orientation, glm::vec2 const& r_position, glm::vec4 const& r_color)
                    */

                    // Construct render data + call draw function in batch renderer
                    m_batchRenderer.DrawQuad(
                        RenderData{
                            EnumMeshType::TEXTURED_QUAD, transformComponent.width, transformComponent.height,// meshtype, width, height
                            transformComponent.orientation, glm::vec2{transformComponent.position.x, transformComponent.position.y},// orientation, position
                            rendererComponent.GetColor(), // color
                        }, r_toNdc
                    );
                }
            } // End of for (EntityID const& id : renderableContainer)

            m_batchRenderer.Flush(); // Ensure all objects have been rendered

            // Loop through text objects
            //m_uiTextObjects
            DrawTextObjects(m_uiTextObjects, r_toNdc);

            m_batchRenderer.Flush(); // Ensure all objects have been rendered
        }


        void NewRendererManager::DrawDebug(glm::mat4 const& r_worldToNdc)
        {
            // Loop through debug objects
            for (RenderData const& rectangle : m_debugRectangles)
            {
                m_batchRenderer.DrawRectangleOutline(rectangle, r_worldToNdc);
            }
            for (RenderData const& circle : m_debugCircles)
            {
                m_batchRenderer.DrawCircleOutline(circle, r_worldToNdc);
            }
            for (RenderData const& line : m_debugLines)
            {
                m_batchRenderer.DrawLine(line, r_worldToNdc);
            }
            for (RenderData const& point : m_debugPoints)
            {
                m_batchRenderer.DrawPoint(point, r_worldToNdc);
            }

            m_batchRenderer.Flush(); // Ensure all objects have been rendered
        }


        void NewRendererManager::DrawScreenQuad(GLuint textureId)
        {
            //@CHECK krystal

            /*
                (EnumMeshType _meshType, float const _width, float const _height,
                float const _orientation, glm::vec2 const& r_position, glm::vec4 const& r_color,
                std::array<glm::vec2, 4> const& r_uvCoordinates, TextureKey const& _textureKey)
            */

            // Construct the render data for the camera quad
            TexturedRenderData renderData
            {
                EnumMeshType::TEXTURED_QUAD, m_cachedPlayWindowWidth, m_cachedPlayWindowHeight, // meshtype, width, height
                0.f, glm::vec2{0.f, 0.f}, // orientation, position
                glm::vec4{1.f, 1.f, 1.f, 1.f}, // color (white)
                QuadMeshData::GetDefaultUvCoordinates(), textureId 
            };

            // Construct the world to NDC matrix
            glm::mat4 windowToNdc
            {
                2.f / m_cachedPlayWindowWidth, 0.f, 0.f, 0.f,
                0.f, 2.f / m_cachedPlayWindowHeight, 0.f, 0.f,
                0.f, 0.f, -2.f / 20.f, 0.f,
                0.f, 0.f, 0.f, 1.f // assumption: view frustrum is centered
            };

            // Add this to the batch
            m_batchRenderer.DrawQuad(renderData, windowToNdc);

            m_batchRenderer.Flush(); // Ensure all objects have been rendered
        }


        void NewRendererManager::DrawTextObjects(std::vector<EntityID> const& r_textObjects, glm::mat4 const& r_toNdc)
        {
            std::shared_ptr<ShaderProgram> p_textShader{ ResourceManager::GetInstance().ShaderPrograms[m_textShaderProgramKey] };

            for (const EntityID& id : r_textObjects)
            {
                // Don't draw anything if the entity is inactive or is not childed to a canvas
                if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive /*|| !Hierarchy::GetInstance().AreParentsActive(id)*/) { continue; }

                TextComponent const& textComponent{ EntityManager::GetInstance().Get<TextComponent>(id) };
                vec2 position{ EntityManager::GetInstance().Get<Transform>(id).position };

                // if component has no font
                if (textComponent.GetFont() == nullptr)
                {
                    break;
                }

                // Store the index of the rendered entity
                renderedEntities.emplace_back(id);

                // get width and height of text
                glm::vec2 textSize{ textComponent.GetFont()->Characters.at('a').Size };
                textSize.x = textComponent.GetFont()->Characters.at('a').Size.x * textComponent.GetText().size() * textComponent.GetSize();
                textSize.y *= (textComponent.GetSize());

                // Resize the transform if the entity does not have other renderer components
                if (!EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<GUIRenderer>())
                    && !EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<Renderer>()))
                {
                    EntityManager::GetInstance().Get<Transform>(id).width = textSize.x;
                    EntityManager::GetInstance().Get<Transform>(id).height = textSize.y;
                }

                textSize.x *= 0.5f;
                textSize.y *= 0.5f;

                // activate corresponding render state	
                p_textShader->Use();
                p_textShader->SetUniform("u_ViewProjection", r_toNdc);
                p_textShader->SetUniform("textColor", textComponent.GetColor());

                glActiveTexture(GL_TEXTURE0);
                glBindVertexArray(textComponent.GetFont()->m_vertexArrayObject);

                // iterate through all characters
                std::string::const_iterator c;
                for (c = textComponent.GetText().begin(); c != textComponent.GetText().end(); c++)
                {
                    Character ch = textComponent.GetFont()->Characters.at(*c);

                    float xPosition = position.x + ch.Bearing.x * textComponent.GetSize() - textSize.x;
                    float yPosition = position.y - (ch.Size.y - ch.Bearing.y) * textComponent.GetSize() - textSize.y;

                    float w = ch.Size.x * textComponent.GetSize();
                    float h = ch.Size.y * textComponent.GetSize();
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
                    glBindTexture(GL_TEXTURE_2D, ch.TextureID);

                    // update content of VBO memory
                    glBindBuffer(GL_ARRAY_BUFFER, textComponent.GetFont()->m_vertexBufferObject);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);

                    // render quad
                    glDrawArrays(GL_TRIANGLES, 0, 6);

                    ++textDrawCalls;

                    // now advance cursors for next glyph
                    position.x += (ch.Advance >> 6) * textComponent.GetSize(); // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
                }

                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);

                p_textShader->UnUse();
            }

        }


        void NewRendererManager::SortTextObjects()
        {
            m_uiTextObjects.clear();
            m_worldTextObjects.clear();

            // Sort the text objects such that text objects that are childed to canvases 
            // are rendered in ui space and text objects that are not are rendered in world space
            for (EntityID id : SceneView<TextComponent>())
            {
                if (GETGUISYSTEM()->IsChildedToCanvas(id))
                    m_uiTextObjects.emplace_back(id);
                else
                    m_worldTextObjects.emplace_back(id);
            }
        }


        void NewRendererManager::OnKeyTriggeredEvent(const Event<KeyEvents>& r_event)
        {
            //dynamic cast
            if (r_event.GetType() != KeyEvents::KeyTriggered)
            {
                return;
            }
            PE::KeyTriggeredEvent triggeredEvent = dynamic_cast<const PE::KeyTriggeredEvent&>(r_event);
#ifdef MY_DEBUG
            std::cout << "NewRendererManager::OnKeyTriggeredEvent(): " << triggeredEvent.keycode << "\n";
#endif
        }


        void NewRendererManager::ConfigureOpenGlState()
        {
            // Enable alpha blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GL_PRIMITIVE_RESTART);
            glPrimitiveRestartIndex(restartIndex);

            // note that the opengl version is also being configured by imgui 
            // in editor.cpp `ImGui_ImplOpenGL3_Init("#version 450");`

#ifndef GAMERELEASE
            // Enable debug output
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(GLDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif // !GAMERELEASE
            
        }


        void NewRendererManager::ClearBackground(float r, float g, float b, float a)
        {
            glClearColor(r, g, b, a);       // Set the background color
            glClear(GL_COLOR_BUFFER_BIT);   // Clear the color buffer
        }


        void NewRendererManager::ReserveContainersSpace(std::size_t const reserveAmount)
        {
            m_debugRectangles.reserve(reserveAmount);
            m_debugCircles.reserve(reserveAmount);
            m_debugLines.reserve(reserveAmount);
            m_debugPoints.reserve(reserveAmount);

            m_worldTextObjects.reserve(reserveAmount);
            m_uiTextObjects.reserve(reserveAmount);
        }


        void NewRendererManager::UpdatePlayWindowSize()
        {
            // @CHECK krystal
            // idk if i should be caching something else in case the screenpicking explodes
            
            //m_cachedPlayWindowWidth, m_cachedPlayWindowHeight;
            float windowWidth, windowHeight;

#ifndef GAMERELEASE
            if (Editor::GetInstance().IsEditorActive())
            {
                Editor::GetInstance().GetGameWindowSize(windowWidth, windowHeight);
            }
            else
#endif // !GAMERELEASE
            {
                int width, height;
                glfwGetWindowSize(WindowManager::GetInstance().GetWindow(), &width, &height);
                windowWidth = static_cast<float>(width);
                windowHeight = static_cast<float>(height);
            }

            // Check if cached window size matches the actual window size
            if ((!CompareFloats(windowWidth, 0.f) && !CompareFloats(windowHeight, 0.f)) && // Ensure that the window dimensions are not zero
                (!CompareFloats(m_cachedPlayWindowWidth, windowWidth) || !CompareFloats(m_cachedPlayWindowHeight, windowHeight)))
            {
                m_cachedPlayWindowWidth = windowWidth, m_cachedPlayWindowHeight = windowHeight;
//#ifdef MY_DEBUG
                std::cout << "NewRendererManager::UpdatePlayWindowSize(): changing play window to " << m_cachedPlayWindowWidth << " x " << m_cachedPlayWindowHeight << "\n";
//#endif 
                // Update the frame buffer
                GLsizei const windowWidthInt{ static_cast<GLsizei>(windowWidth) };
                GLsizei const windowHeightInt{ static_cast<GLsizei>(windowHeight) };
                m_frameBuffers[static_cast<std::size_t>(EnumFrameBuffer::PLAY_MODE)].Resize(windowWidthInt, windowHeightInt);
            }
        }


        void NewRendererManager::UpdateSceneWindowSize()
        {
            // @CHECK krystal
            // idk if i should be caching something else in case the screenpicking explodes

            //m_cachedSceneWindowWidth, m_cachedSceneWindowHeight;

#ifndef GAMERELEASE

            if (!Editor::GetInstance().IsEditorActive()) { return; }

            float windowWidth, windowHeight; 
            Editor::GetInstance().GetWindowSize(windowWidth, windowHeight);

            // Check if cached window size matches the actual window size
            if ((!CompareFloats(windowWidth, 0.f) && !CompareFloats(windowHeight, 0.f)) && // Ensure that the window dimensions are not zero
                (!CompareFloats(m_cachedSceneWindowWidth, windowWidth) || !CompareFloats(m_cachedSceneWindowHeight, windowHeight)))
            {
                m_cachedSceneWindowWidth = windowWidth, m_cachedSceneWindowHeight = windowHeight;
//#ifdef MY_DEBUG
                std::cout << "NewRendererManager::UpdateSceneWindowSize(): changing scene window to " << m_cachedSceneWindowWidth << " x " << m_cachedSceneWindowHeight << "\n";
//#endif
                // Update the frame buffer
                GLsizei const windowWidthInt{ static_cast<GLsizei>(windowWidth) };
                GLsizei const windowHeightInt{ static_cast<GLsizei>(windowHeight) };
                glViewport(0, 0, windowWidthInt, windowHeightInt);
                m_frameBuffers[static_cast<std::size_t>(EnumFrameBuffer::EDIT_MODE)].Resize(windowWidthInt, windowHeightInt);
            }

#endif // !GAMERELEASE
        }

        void NewRendererManager::SetViewport(float const width, float const height)
        {
            // Update the frame buffer
            GLsizei const windowWidthInt{ static_cast<GLsizei>(width) };
            GLsizei const windowHeightInt{ static_cast<GLsizei>(height) };
            glViewport(0, 0, windowWidthInt, windowHeightInt);
        }

    } // End of Graphics namespace
} // End of PE namespace