#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     RendererManager.h
 \date     30-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the RendererManager class, which manages 
           the render passes and includes helper functions to draw debug shapes.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "GLHeaders.h"

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp> // atan2()


#include "Renderer.h"

#include "GUIRenderer.h"

#include "CameraManager.h"
#include "MeshData.h"
#include "FrameBuffer.h"
#include "ShaderProgram.h"
#include "System.h"
#include "Physics/Colliders.h"

#include "ECS/SceneView.h"
#include "Text.h"

#pragma once

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
         \brief System In charge of calling the draw functions in all the renderer components.
        *************************************************************************************/
        class RendererManager : public System
        {            
            // ----- Public Variables ----- //
        public:
            // the Entity IDs of the entities that have been rendered, in the order they were rendered in.
            static std::vector<EntityID> renderedEntities;
            static unsigned totalDrawCalls;   // Sum total of all draw calls made
            static unsigned textDrawCalls;    // Total draw calls made for text (1 draw call per chara)
            static unsigned objectDrawCalls;  // Total draw calls for gameobjects
            static unsigned debugDrawCalls;   // Total draw calls for debug shapes

            // ----- Constructors ----- //
        public:
            /*!***********************************************************************************
             \brief Initializes the renderer manager, and sets the reference to the window to 
                    draw to. 

             \param[in,out] p_window Pointer to the GLFWwindow to render to.
             \param[in,out] r_cameraManagerArg Reference to the camera manager.
             \param[in] windowWidth Width of the window.
             \param[in] windowHeight Height of the window.
            *************************************************************************************/
            RendererManager( CameraManager& r_cameraManagerArg, 
                int const windowWidth, int const windowHeight);

            // ----- Public getters / setters ----- //
        public:
            /*!***********************************************************************************
             \brief Sets the RGBA color that is rendered when there is nothing in the scene. 

             \param[in] r Red component of the color to set the background to (from [0, 1]).
             \param[in] g Green component of the color to set the background to (from [0, 1]).
             \param[in] b Blue component of the color to set the background to (from [0, 1]).
             \param[in] a Alpha component of the color to set the background to (from [0, 1]).
            *************************************************************************************/
            void SetBackgroundColor(float const r = 0.f, float const g = 0.f, float const b = 0.f, float const a = 1.f);

            /*!***********************************************************************************
             \brief Gets the RGBA color that is rendered when there is nothing in the scene. 

             \return The RGBA color that is rendered when there is nothing in the scene. 
            *************************************************************************************/
            inline vec4 GetBackgroundColor() const 
            {
                return vec4{ m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a };
            }

            // ----- Public methods ----- //
        public:
            /*!***********************************************************************************
             \brief Creates a VAO and VBO for a quad and stores it, and compiles and links a 
                    simple shader program to draw the triangle.
            *************************************************************************************/
            void InitializeSystem();

            /*!***********************************************************************************
             \brief Draws the editor windows, the gameobjects and the debug shapes. 

             \param[in] deltaTime Timestep (in seconds). Not used by the renderer manager.
            *************************************************************************************/
            void UpdateSystem(float deltaTime);

            /*!***********************************************************************************
             \brief Delete the buffers, VAO and shader program allocated through OpenGL.
            *************************************************************************************/
            void DestroySystem();

            /*!***********************************************************************************
             \brief Returns the name of the Graphics system.
            *************************************************************************************/
            inline std::string GetName() { return m_systemName; }

            /*!***********************************************************************************
             \brief Draws the texture attached to the render frame buffer to a quad stretched 
                    to the size of the window.
            *************************************************************************************/
            void DrawCameraQuad();

            /*!***********************************************************************************
             \brief Draws the custom cursor.

             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to NDC space.
            *************************************************************************************/
            void DrawCursor(glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief Loops through all objects with a Renderer component (or a class that
                    derives from it) and draws it. Batches the colors, transformation matrices 
                    and textured status together into a vertex buffer object and makes an 
                    instanced drawcall. The instanced batch is broken when a new texture is encountered.

             \tparam T - A component type derived from the Renderer.
             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to
                            NDC space.
             \param[in] r_rendererContainer Container containing the renderer and transform 
                            components of the objects to draw
            *************************************************************************************/
            template<typename T>
            void DrawQuadsInstanced(glm::mat4 const& r_worldToNdc, std::vector<EntityID> const& r_rendererIdContainer);

            /*!***********************************************************************************
             \brief Loops through all objects with colliders and rigidbody components and draws 
                    shapes to visualise their bounds, direction and magnitude for debug purposes. 

             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to NDC space.
             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from view to NDC space.
            *************************************************************************************/
            void DrawDebug(glm::mat4 const& r_worldToNdc, glm::mat4 const& r_viewToNdc);

            /*!***********************************************************************************
             \brief Binds the shader program, vertex array object and texture and makes a
                    draw call for the object passed in.

             \param[in] meshType Type of mesh to draw with.
             \param[in] r_color Color to draw the mesh.
             \param[in] r_shaderProgram Shader program to use.
             \param[in] primitiveType GL Primitive type to make the draw call with.
             \param[in] r_modelToNdc 4x4 matrix that transforms coordinates from model to NDC space.
            *************************************************************************************/
            void Draw(EnumMeshType const meshType, glm::vec4 const& r_color, ShaderProgram& r_shaderProgram,
                GLenum const primitiveType, glm::mat4 const& r_modelToNdc);

            /*!***********************************************************************************
             \brief Binds the shader program, vertex array object and texture and makes a
                    draw call for the object passed in.

             \param[in] meshType Type of mesh to draw with.
             \param[in] r_color Color to draw the mesh.
             \param[in] textureId Id of the texture to draw the mesh with.
             \param[in] r_shaderProgram Shader program to use.
             \param[in] primitiveType GL Primitive type to make the draw call with.
             \param[in] r_modelToNdc 4x4 matrix that transforms coordinates from model to NDC space.
            *************************************************************************************/
            void Draw(EnumMeshType const meshType, glm::vec4 const& r_color, 
                GLuint textureId, ShaderProgram& r_shaderProgram,
                GLenum const primitiveType, glm::mat4 const& r_modelToNdc);

            /*!***********************************************************************************
             \brief Creates a new VBO with the texture status, color and matrix of each instance
                    of the mesh, binds it to the VAO and makes an instanced draw call.

             \param[in] count Number of instances to draw.
             \param[in] meshIndex Index of mesh in [m_meshes]. Derived by casting EnumMeshType.
             \param[in] primitiveType GL Primitive type to make the draw call with.
            *************************************************************************************/
            void DrawInstanced(size_t const count, size_t const meshIndex, GLenum const primitiveType);

            /*!***********************************************************************************
             \brief Makes a draw call for a square to represent the AABB collider passed in.

             \param[in] r_aabbCollider Information about the AABB collider to render.
             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to NDC space.
             \param[in, out] r_shaderProgram Shader program to use.
             \param[in] r_color Color to draw the shape.
            *************************************************************************************/
            void DrawCollider(AABBCollider const& r_aabbCollider,
                glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
                glm::vec4 const& r_color = { 1.f, 0.f, 0.f, 1.f });

            /*!***********************************************************************************
             \brief Makes a draw call for a circle to represent the circle collider passed in.

             \param[in] r_circleCollider Information about the circle collider to render.
             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to NDC space.
             \param[in, out] r_shaderProgram Shader program to use.
             \param[in] r_color Color to draw the shape.
            *************************************************************************************/
            void DrawCollider(CircleCollider const& r_circleCollider,
                glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
                glm::vec4 const& r_color = { 0.f, 1.f, 0.f, 1.f });

            /*!***********************************************************************************
             \brief Makes a draw call for the outline of a rectangle.

             \param[in] width Width of the rectangle.
             \param[in] height Height of the rectangle.
             \param[in] orientation Orientation of the rectangle (in radians).
             \param[in] xPosition X position of the center of the rectangle.
             \param[in] yPosition Y position of the center of the rectangle.
             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to NDC space.
             \param[in, out] r_shaderProgram Shader program to use.
             \param[in] r_color Color to draw the shape.
            *************************************************************************************/
            void DrawDebugRectangle(float const width, float const height,
                float const orientation, float const xPosition, float const yPosition,
                glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
                glm::vec4 const& r_color);

            /*!***********************************************************************************
             \brief Makes a draw call for a line to represent the vector passed in.

             \param[in] r_vector Length and direction of the vector.
             \param[in] r_startPosition Position (in world space) the vector should start at.
             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to NDC space.
             \param[in, out] r_shaderProgram Shader program to use.
             \param[in] r_color Color to draw the shape.
            *************************************************************************************/
            void DrawDebugLine(
                glm::vec2 const& r_vector, glm::vec2 const& r_startPosition,
                glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
                glm::vec4 const& r_color = { 0.f, 0.f, 1.f, 1.f });

            /*!***********************************************************************************
             \brief Makes a draw call for a point to represent the position passed in.

             \param[in] r_position Position to draw the point at.
             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to NDC space.
             \param[in, out] r_shaderProgram Shader program to use.
             \param[in] r_color Color to draw the shape.
            *************************************************************************************/
            void DrawDebugPoint(glm::vec2 const& r_position,
                glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
                glm::vec4 const& r_color = { 0.f, 0.f, 1.f, 1.f });

            /*!***********************************************************************************
             \brief Makes a draw call for a "+" to represent the position, up and right vectors 
                    passed in.

             \param[in] r_position Position to draw the point at.
             \param[in] r_upVector Direction and magnitude of the vertical line to draw.
             \param[in] r_rightVector Direction and magnitude of the horizontal line to draw.
             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to NDC space.
             \param[in, out] r_shaderProgram Shader program to use.
             \param[in] r_color Color to draw the shape.
            *************************************************************************************/
            void DrawDebugCross(glm::vec2 const& r_position,
                glm::vec2 const& r_upVector, glm::vec2 const& r_rightVector,
                glm::mat4 const& r_worldToNdc, ShaderProgram& r_shaderProgram,
                glm::vec4 const& r_color = { 0.5f, 0.5f, 1.f, 1.f });


            /*!***********************************************************************************
            \brief  Computes the 4x4 matrix to transform coordinates in model space to world space.

            \param[in] width Width of the object.
            \param[in] height Height of the object.
            \param[in] orientation Counterclockwise angle (in radians) about the z-axis from the x-axis.
            \param[in] positionX X position of the object (in world space).
            \param[in] positionY Y position of the object (in world space).

            \return glm::mat4 - 4x4 matrix to transform coordinates in model space to world space.
            *************************************************************************************/
            static glm::mat4 GenerateTransformMatrix(float const width, float const height,
                float const orientation, float const positionX, float const positionY);

            /*!***********************************************************************************
            \brief  Computes the 4x4 matrix to transform coordinates in model space to world space.

            \param[in] rightVector Right vector of the object.
            \param[in] upVector Up vector of the object.
            \param[in] centerPosition Position of the center of the object (in world space).

            \return glm::mat4 - 4x4 matrix to transform coordinates in model space to world space.
            *************************************************************************************/
            static glm::mat4 GenerateTransformMatrix(glm::vec2 const& rightVector,
                glm::vec2 const& upVector, glm::vec2 const& centerPosition);

            /*!***********************************************************************************
            \brief  Computes the 4x4 matrix to transform coordinates in world space to model space.

            \param[in] width Width of the object.
            \param[in] height Height of the object.
            \param[in] orientation Counterclockwise angle (in radians) about the z-axis from the x-axis.
            \param[in] positionX X position of the object (in world space).
            \param[in] positionY Y position of the object (in world space).

            \return glm::mat4 - 4x4 matrix to transform coordinates in world space to model space.
            *************************************************************************************/
            static glm::mat4 GenerateInverseTransformMatrix(float const width, float const height,
                float const orientation, float const positionX, float const positionY);
            /*!***********************************************************************************
             \brief Renders text from r_text parameter. Retrieves glyph information from map
                   and renders a quad with the data.

             \param[in] r_worldToNdc Projection matrix for transforming vertex coordinates of quad
            *************************************************************************************/
            void RenderText(glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief Renders text from r_text parameter. Retrieves glyph information from map
                   and renders a quad with the data.

             \param[in] r_worldToNdc Projection matrix for transforming vertex coordinates of quad
            *************************************************************************************/
            void RenderText(const EntityID& r_id, glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief Renders a line of text from r_line parameter. Retrieves glyph information from
                    map and renders a quad with the data.

             \param[in] r_textComponent Component to get text data.
             \param[in] r_line line to render.
             \param[in] position Position of text to render on to the screen.
             \param[in] currentY y position of the line.
             \param[in] hAlignOffset horizontal alignment offset.
             \param[in] vAlignOffset vertical alignment offset.
            *************************************************************************************/
            void RenderLine(TextComponent const& r_textComponent, std::string const& r_line, vec2 position, float currentY, float hAlignOffset, float vAlignOffset);

            // ----- Private variables ----- //
        private:
            CameraManager& r_cameraManager; // Reference to the camera manager

            // Framebuffer object for rendering game scene rendered through the editor or in-game runtime camera
            Graphics::FrameBuffer m_renderFrameBuffer{}; 

            std::string m_systemName{ "RendererManager" }; // Name of system

            // Default shader program to use
            std::string m_defaultShaderProgramKey{"Textured"};
            std::string m_instancedShaderProgramKey{"Instanced"};
            std::string m_textShaderProgramKey{ "Text" };

            // Container of meshes
            std::vector<Graphics::MeshData> m_meshes{};

            //! Width and height of the ImGui window the last time the framebuffer was resized
            float m_cachedWindowWidth{ -1.f }, m_cachedWindowHeight{ -1.f };
            const int m_windowStartWidth, m_windowStartHeight;
                        
            std::vector<float> m_isTextured{}; // Container that stores whether the quad is textured
            std::vector<glm::mat4> m_modelToWorldMatrices{}; // Container that stores the model to world matrix for the quad
            std::vector<glm::vec4> m_colors{}; // Container that stores the color for each quad
            std::vector<glm::vec2> m_UV{};

            // Color that is rendered when there is nothing in the scene
            glm::vec4 m_backgroundColor{ 0.796f, 0.6157f, 0.4588f, 1.f }; // brown by default

            // ----- Private methods ----- //
        private:
            /*!***********************************************************************************
             \brief Sets the vertex positions and indices of the object passed in to that of
                    a circle (generated with [segments] number of points along its edges) 
                    centered at the origin with a diamter of 1 and creates a VAO.

             \param[in] segments Number of edges that should make up the circle.
             \param[in,out] r_mesh Object containing the mesh data generated.
            *************************************************************************************/
            void InitializeCircleMesh(std::size_t const segments, MeshData& r_mesh);

            /*!***********************************************************************************
             \brief Sets the vertex positions and indices of the object passed in to that of
                    an isosceles triangle centered at the origin and creates a VAO.

             \param[in,out] r_mesh Object containing the mesh data generated.
            *************************************************************************************/
            void InitializeTriangleMesh(MeshData& r_mesh);

            /*!***********************************************************************************
             \brief Sets the vertex positions and indices of the object passed in to that of 
                    a 1x1 quad made of 2 triangles centered at the origin and creates a VAO.

             \param[in,out] r_mesh Object containing the mesh data generated.
            *************************************************************************************/
            void InitializeQuadMesh(MeshData& r_mesh);

            /*!***********************************************************************************
             \brief Sets the vertex positions and indices of the object passed in to that of 
                    a 1x1 square outline centered at the origin and creates a VAO.

             \param[in,out] r_mesh Object containing the mesh data generated.
            *************************************************************************************/
            void InitializeSquareMesh(MeshData& r_mesh);

            /*!***********************************************************************************
             \brief Sets the vertex positions and indices of the object passed in to that of 
                    a 1 unit long horizontal line centered at the origin and creates a VAO.

             \param[in,out] r_mesh Object containing the mesh data generated.
            *************************************************************************************/
            void InitializeLineMesh(MeshData& r_mesh);

            /*!***********************************************************************************
             \brief Creates a VAO with a single vertex at the origin (0, 0).

             \param[in,out] r_mesh Object containing the mesh data generated.
            *************************************************************************************/
            void InitializePointMesh(MeshData& r_mesh);

            /*!***********************************************************************************
             \brief Prints the hardware specifications of the device related to graphics.
            *************************************************************************************/
            void PrintSpecifications() const;
        };

        /*!***********************************************************************************
         \brief Prints error messages from OpenGL.
        *************************************************************************************/
        void APIENTRY GlDebugOutput(GLenum source, GLenum type, unsigned int id,
            GLenum severity, GLsizei length, const char* message, const void* userParam);

    } // End of Graphics namespace
} // End of PE namespace