#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     NewRendererManager.h
 \date     11-12-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the NewRendererManager class, which manages the 
           objects to be drawn and renderers being used.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "GLHeaders.h"

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp> // atan2()

#include "System.h"
#include "Events/EventHandler.h"

// ECS Components
#include "ECS/Entity.h"
#include "ECS/EntityFactory.h"
#include "Hierarchy/HierarchyManager.h"

// Renderer components
#include "Renderer.h"
#include "GUIRenderer.h"
#include "BatchRenderer.h"

#include "FrameBuffer.h"

using ShaderProgramKey = std::string;

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
         \brief System In charge of calling the draw functions in all the renderer components.
        *************************************************************************************/
        class NewRendererManager : public System
        {
            // ----- Public Enums ----- //
        public:
            /*!***********************************************************************************
             \brief Indices of the framebuffers
            *************************************************************************************/
            enum class EnumFrameBuffer : unsigned short 
            {
                EDIT_MODE,
                PLAY_MODE,
                FRAMEBUFFER_COUNT
            };


            // ----- Public Members ----- //
        public: 
            // the Entity IDs of the entities that have been rendered, in the order they were rendered in.
            static std::vector<EntityID> renderedEntities;
            static unsigned totalDrawCalls;   // Sum total of all draw calls made
            static unsigned textDrawCalls;    // Total draw calls made for text (1 draw call per chara)
            static unsigned objectDrawCalls;  // Total draw calls for gameobjects
            static unsigned debugDrawCalls;   // Total draw calls for debug shapes


            // ----- Constructor ----- //
        public:
            /*!***********************************************************************************
             \brief Constructs the renderer manager and sets the target resolution + aspect ratio.

             \param[in] _width - Target game window width to render to.
             \param[in] _height - Target game window width to render to.
            *************************************************************************************/
            NewRendererManager(int const _width, int const _height);


            // ----- Public Setters ----- //
        public:
            /*!***********************************************************************************
             \brief Sets the color to clear the screen to.

             \param[in] r - Red component of the color to set the object to (from [0, 1]).
             \param[in] g - Green component of the color to set the object to (from [0, 1]).
             \param[in] b - Blue component of the color to set the object to (from [0, 1]).
             \param[in] a - Alpha component of the color to set the object to (from [0, 1]).
            *************************************************************************************/
            void SetClearColor(float const r, float const g, float const b, float const a = 1.f) 
            {
                m_clearColor.r = r;
                m_clearColor.g = g;
                m_clearColor.b = b;
                m_clearColor.a = a;
            }


            // ----- Public Getters ----- //
        public:
            /*!***********************************************************************************
             \brief Returns the name of the Graphics system.
            *************************************************************************************/
            inline std::string GetName() { return m_systemName; }


            // ----- Public methods ----- //
        public:
            /*!***********************************************************************************
             \brief 
            *************************************************************************************/
            void InitializeSystem();

            /*!***********************************************************************************
             \brief 

             \param[in] deltaTime Timestep (in seconds). Not used by the renderer manager.
            *************************************************************************************/
            void UpdateSystem(float deltaTime);

            /*!***********************************************************************************
             \brief 
            *************************************************************************************/
            void DestroySystem();

            /*!***********************************************************************************
             \brief

             \param[in] r_renderData - 
            *************************************************************************************/
            void AddDebugShape(RenderData const& r_renderData);


            // ----- Private variables ----- //
        private:
            std::string m_systemName{ "RendererManager" }; // Name of system

            // Dimensions of target resolution of the game window
            int m_targetWidth, m_targetHeight;
            float m_targetAspectRatio; // Aspect ratio = width / height

            // Width and height of the ImGui windows the last time the framebuffer was resized
            float m_cachedSceneWindowWidth{ -1.f }, m_cachedSceneWindowHeight{ -1.f };
            float m_cachedPlayWindowWidth{ -1.f }, m_cachedPlayWindowHeight{ -1.f };
            int keyEventHandler{};  // ID of the event handler for key triggers

            // Color to clear the screen to
            glm::vec4 m_clearColor{ 0.796f, 0.6157f, 0.4588f, 1.f };

            // Batch renderer to use
            BatchRenderer m_batchRenderer{};
            // Container of framebuffers
            std::vector<FrameBuffer> m_frameBuffers{};

            // Default shader program to use
            ShaderProgramKey m_coloredShaderProgramKey{"Colored"};      // Shader used for batched rendering of colored, untextured meshes
            ShaderProgramKey m_texturedShaderProgramKey{"Textured"};    // Shader used for batched rendering of textured meshes
            ShaderProgramKey m_instancedShaderProgramKey{"Instanced"};  // Shader used for instanced rendering of textured meshes
            ShaderProgramKey m_textShaderProgramKey{"Text"};            // Shader used for rendering text

            // Containers for debug shapes
            std::vector<RenderData> m_debugRectangles{};
            std::vector<RenderData> m_debugCircles{};
            std::vector<RenderData> m_debugLines{};
            std::vector<RenderData> m_debugPoints{};

            // Text objects 
            std::vector<EntityID> m_worldTextObjects{};   // Text objects in world space
            std::vector<EntityID> m_uiTextObjects{};      // Text objects in UI space


            // ----- Private Methods ----- //
        private:

            /*!***********************************************************************************
             \brief

             \param[in] r_worldToNdc - Matrix that converts objects from world to NDC space.
            *************************************************************************************/
            void DrawGameObjects(glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief

             \param[in] r_toNdc - Matrix that converts the UI elements from world or view 
                            to NDC space.
            *************************************************************************************/
            void DrawUi(glm::mat4 const& r_toNdc);

            /*!***********************************************************************************
             \brief

             \param[in] r_worldToNdc - Matrix that converts objects from world to NDC space.
            *************************************************************************************/
            void DrawDebug(glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief

             \param[in] textureId - ID of the texture
            *************************************************************************************/
            void DrawScreenQuad(GLuint textureId);

            /*!***********************************************************************************
             \brief Renders text from r_text parameter. Retrieves glyph information from map
                   and renders a quad with the data.

             \param[in] r_textObjects - Container of IDs of text objects to render.
             \param[in] r_toNdc - Matrix for transforming coordinates from world or view to NDC space.
            *************************************************************************************/
            void DrawTextObjects(std::vector<EntityID> const& r_textObjects, glm::mat4 const& r_toNdc);

            /*!***********************************************************************************
             \brief Helper method to sort text components in the scene into world space
                    and screen space text objects
            *************************************************************************************/
            void SortTextObjects();

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void OnKeyTriggeredEvent(const Event<KeyEvents>& r_event);

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void ConfigureOpenGlState();

            /*!***********************************************************************************
             \brief Clears the content of the window to the RGBA color passed in.

             \param[in] r - Red value of the RGBA color to set the background to. Values range [0, 1].
             \param[in] g - Green value of the RGBA color to set the background to. Values range [0, 1].
             \param[in] b - Blue value of the RGBA color to set the background to. Values range [0, 1].
             \param[in] a - Alpha value of the RGBA color to set the background to. Values range [0, 1].
            *************************************************************************************/
            void ClearBackground(float r, float g, float b, float a); 

            /*!***********************************************************************************
             \brief Reserves [reserveAmount] of space in all the member containers

             \param[in] reserveAmount - Number of elements to reserve in each container.
            *************************************************************************************/
            void ReserveContainersSpace(std::size_t const reserveAmount); 

            /*!***********************************************************************************
             \brief Checks if the play window size has changed and resizes the frame buffer 
                    if necessary. 
            *************************************************************************************/
            void UpdatePlayWindowSize();

            /*!***********************************************************************************
             \brief Checks if the scene window size has changed and resizes the frame buffer
                    if necessary.
            *************************************************************************************/
            void UpdateSceneWindowSize();

            /*!***********************************************************************************
             \brief Set the OpenGL viewport to the size passed in.

             \param[in] width - Width to set the viewport to.
             \param[in] height - Height to set the viewport to.
            *************************************************************************************/
            void SetViewport(float const width, float const height);

        }; // End of class

    } // End of Graphics namespace
} // End of PE namespace