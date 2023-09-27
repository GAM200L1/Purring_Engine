#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     RendererManager.h
 \date     20-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the RendererManager class, which manages 
           the render passes and includes helper functions to draw debug shapes.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "GLHeaders.h"

#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp> // atan2()
#include <map>
#include <vector>

#include "Camera.h"
#include "MeshData.h"
#include "Renderer.h"
#include "FrameBuffer.h"
#include "ShaderProgram.h"
#include "System.h"
#include "Physics/Colliders.h"


namespace PE
{
    namespace Graphics
    {
        // In charge of calling the draw functions in all the renderer components.
        class RendererManager : public System
        {
            // ----- Constructors ----- //
        public:
            /*!***********************************************************************************
             \brief Sets the reference to the window to draw to. 

             \param[in,out] p_window Pointer to the GLFWwindow to render to.
            *************************************************************************************/
            RendererManager(GLFWwindow* p_window);

            // ----- Public methods ----- //
        public:
            /*!***********************************************************************************
             \brief Creates a VAO and VBO for a quad and stores it, and compiles and links a 
                    simple shader program to draw the triangle.
            *************************************************************************************/
            void InitializeSystem();

            /*!***********************************************************************************
             \brief Calls the drawing functions

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
             \brief Calls DrawRenderer() on all renderable objects in [m_renderableObjects].

             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to NDC space.
            *************************************************************************************/
            void DrawScene(glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief  

             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to NDC space.
            *************************************************************************************/
            void DrawSceneInstanced(glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief Calls DrawRenderer() on all debug objects in [m_lineObjects] and 
                    [m_pointObjects].

             \param[in] r_worldToNdc 4x4 matrix that transforms coordinates from world to NDC space.
            *************************************************************************************/
            void DrawDebug(glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief Binds the shader program, vertex array object and texture and makes the
                    draw call for the [r_renderer] passed in.

             \param[in] r_renderer Renderer object with all the information to draw with.
             \param[in] r_shaderProgram Shader program to use.
             \param[in] primitiveType GL Primitive type to make the draw call with.
             \param[in] r_modelToNdc 4x4 matrix that transforms coordinates from model to NDC space.
            *************************************************************************************/
            void Draw(Renderer const& r_renderer, ShaderProgram& r_shaderProgram, 
                GLenum const primitiveType, glm::mat4 const& r_modelToNdc);

            /*!***********************************************************************************
             \brief Binds the shader program, vertex array object and texture and makes the
                    draw call for the [r_renderer] passed in.

             \param[in] meshType Type of mesh.
             \param[in] r_color Color to draw the mesh.
             \param[in] r_shaderProgram Shader program to use.
             \param[in] primitiveType GL Primitive type to make the draw call with.
             \param[in] r_modelToNdc 4x4 matrix that transforms coordinates from model to NDC space.
            *************************************************************************************/
            void Draw(EnumMeshType const meshType, glm::vec4 const& r_color, ShaderProgram& r_shaderProgram,
                GLenum const primitiveType, glm::mat4 const& r_modelToNdc);

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

            // ----- Private variables ----- //
        private:
            GLFWwindow* p_windowRef{}; //! Pointer to the GLFW window to render to

            Graphics::Camera m_mainCamera{}; //! Camera object

            Graphics::FrameBuffer m_imguiFrameBuffer{}; //! Framebuffer object for rendering to ImGui window

            std::string m_systemName{ "Graphics" }; //! Name of system

            //! Default shader program to use
            std::string m_defaultShaderProgramKey{"Textured"};
            std::string m_instancedShaderProgramKey{"Instanced"};

            //! Container of meshes
            std::vector<Graphics::MeshData> m_meshes{};

            //! Width and height of the ImGui window the last time the framebuffer was resized
            float m_cachedWindowWidth{ -1.f }, m_cachedWindowHeight{ -1.f }; 

            //! 4x4 world to NDC matrix. Updated when window is resized or camera has been repositioned
            glm::mat4 m_cachedWorldToNdcMatrix{}; 

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
            \brief  Computes the 4x4 matrix to transform coordinates in model space to world space.

            \param[in] width Width of the object.
            \param[in] height Height of the object.
            \param[in] orientation Counterclockwise angle (in radians) about the z-axis from the x-axis.
            \param[in] positionX X position of the object (in world space).
            \param[in] positionY Y position of the object (in world space).

            \return glm::mat4 - 4x4 matrix to transform coordinates in model space to world space.
            *************************************************************************************/
            glm::mat4 GenerateTransformMatrix(float const width, float const height, 
                float const orientation, float const positionX, float const positionY);


            /*!***********************************************************************************
            \brief  Computes the 4x4 matrix to transform coordinates in model space to world space.

            \param[in] rightVector Right vector of the object.
            \param[in] upVector Up vector of the object.
            \param[in] centerPosition Position of the center of the object (in world space).

            \return glm::mat4 - 4x4 matrix to transform coordinates in model space to world space.
            *************************************************************************************/
            glm::mat4 GenerateTransformMatrix(glm::vec2 const& rightVector,
                glm::vec2 const& upVector, glm::vec2 const& centerPosition);

            /*!***********************************************************************************
             \brief Prints the hardware specifications of the device related to graphics.
            *************************************************************************************/
            void PrintSpecifications() const;
        };


    } // End of Graphics namespace
} // End of PE namespace