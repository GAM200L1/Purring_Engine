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
#include "ShaderProgram.h"
#include "System.h"

#include "Text.h"

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

             \param[in] r_viewToNdc 4x4 matrix that transforms coordinates from view to NDC space.
            *************************************************************************************/
            void DrawScene(glm::mat4 const& r_viewToNdc);

            /*!***********************************************************************************
             \brief Calls DrawRenderer() on all debug objects in [m_lineObjects] and 
                    [m_pointObjects].

             \param[in] r_viewToNdc 4x4 matrix that transforms coordinates from view to NDC space.
            *************************************************************************************/
            void DrawDebug(glm::mat4 const& r_viewToNdc);

            /*!***********************************************************************************
             \brief Binds the shader program, vertex array object and texture and makes the
                    draw call for the [r_renderer] passed in.

             \param[in] r_renderer Renderer object with all the information to draw with.
             \param[in] r_shaderProgram Shader program to use.
             \param[in] primitiveType GL Primitive type to make the draw call with.
             \param[in] r_modelToNdc 4x4 matrix that transforms coordinates from model to NDC space.
            *************************************************************************************/
            void DrawRenderer(Renderer const& r_renderer, ShaderProgram& r_shaderProgram, GLenum const primitiveType,
                glm::mat4 const& r_modelToNdc);

            /*!***********************************************************************************
             \brief Adds a filled renderer object to be drawn.
             
             \param[in] meshType Type of mesh (Quad or triangle).
             \param[in] width Width of the mesh.
             \param[in] height Height of the mesh.
             \param[in] orientation Angle (in degrees) of the square about the z-axis from the
                                    x-axis. Positive for counter-clockwise rotation, negative
                                    for clockwise.
             \param[in] r_position Position of the center of the mesh.
             \param[in] r_color Color of the object fill. White by default.
            *************************************************************************************/
            void AddRendererObject(EnumMeshType meshType,
                float const width, float const height, float const orientation, 
                glm::vec2 const& r_position, glm::vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });

            /*!***********************************************************************************
             \brief Adds a textured renderer object to be drawn.
             
             \param[in] meshType Type of mesh (Quad or triangle).
             \param[in] width Width of the mesh.
             \param[in] height Height of the mesh.
             \param[in] orientation Angle (in degrees) of the square about the z-axis from the 
                                    x-axis. Positive for counter-clockwise rotation, negative 
                                    for clockwise.
             \param[in] r_position Position of the center of the mesh.
             \param[in] r_texture Texture to apply to the mesh.
             \param[in] r_color Color of the object fill. White by default.
            *************************************************************************************/
            void AddRendererObject(EnumMeshType meshType,
                float const width, float const height,
                float const orientation, glm::vec2 const& r_position,
                std::string const& r_textureName, glm::vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });
            
            /*!***********************************************************************************
             \brief Adds a filled renderer object to be drawn the same size as the window.
             
             \param[in] meshType Type of mesh (Quad or triangle).
             \param[in] width Width of the mesh.
             \param[in] height Height of the mesh.
             \param[in] orientation Angle (in degrees) of the square about the z-axis from the
                                    x-axis. Positive for counter-clockwise rotation, negative
                                    for clockwise.
             \param[in] r_position Position of the center of the mesh.
             \param[in] r_color Color of the object fill. White by default.
            *************************************************************************************/
            void AddBackgroundObject(float const width, float const height, glm::vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });

            /*!***********************************************************************************
             \brief Adds a textured renderer object to be drawn the same size as the window.
             
             \param[in] meshType Type of mesh (Quad or triangle).
             \param[in] width Width of the mesh.
             \param[in] height Height of the mesh.
             \param[in] orientation Angle (in degrees) of the square about the z-axis from the 
                                    x-axis. Positive for counter-clockwise rotation, negative 
                                    for clockwise.
             \param[in] r_position Position of the center of the mesh.
             \param[in] r_texture Texture to apply to the mesh.
             \param[in] r_color Color of the object fill. White by default.
            *************************************************************************************/
            void AddBackgroundObject(float const width, float const height,
                std::string const& r_textureName, glm::vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });
            
            /*!***********************************************************************************
             \brief Adds the outline of a square to be drawn when debug mode is enabled. 
             
             \param[in] width Width of the square.
             \param[in] height Height of the square.
             \param[in] orientation Angle (in degrees) of the square about the z-axis from the 
                                    x-axis. Positive for counter-clockwise rotation, negative 
                                    for clockwise.
             \param[in] r_centerPosition Coordinates of the center of the square.
             \param[in] r_color Color of the square. White by default.
            *************************************************************************************/
            void AddDebugSquare(float const width, float const height,
                float const orientation, glm::vec2 const& r_centerPosition,
                glm::vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });

            /*!***********************************************************************************
             \brief Adds the outline of a square to be drawn when debug mode is enabled. 
                    Assumes that the square is axis-aligned.
             
             \param[in] r_bottomLeft Coordinates of the bottom left corner of the square.
             \param[in] r_topRight Coordinates of the top right corner of the square.
             \param[in] r_color Color of the square. White by default.
            *************************************************************************************/
            void AddDebugSquare(glm::vec2 const& r_bottomLeft, glm::vec2 const& r_topRight,
                glm::vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });

            /*!***********************************************************************************
             \brief Adds the outline of a circle to be drawn when debug mode is enabled.
             
             \param[in] radius Radius of the circle.
             \param[in] r_centerPosition Position of the center of the circle.
             \param[in] r_color Color of the circle. White by default.
            *************************************************************************************/
            void AddDebugCircle(float const radius, glm::vec2 const& r_centerPosition,
                glm::vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });

            /*!***********************************************************************************
             \brief Adds a line to be drawn when debug mode is enabled.
             
             \param[in] r_position1 Position of one endpoint of the line.
             \param[in] r_position2 Position of one endpoint of the line.
             \param[in] r_color Color of the line. White by default.
            *************************************************************************************/
            void AddDebugLine(glm::vec2 const& r_position1, glm::vec2 const& r_position2,
                glm::vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });

            /*!***********************************************************************************
            \brief Adds a point to be drawn when debug mode is enabled.
            
            \param[in] r_position Position of the point.
            \param[in] r_color Color of the point. White by default.
            *************************************************************************************/
            void AddDebugPoint(glm::vec2 const& r_position, glm::vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });

            Graphics::Camera m_mainCamera{}; //! Camera object

            // ----- Private variables ----- //
        private:
            GLFWwindow* p_windowRef{}; //! Pointer to the GLFW window to render to


            std::string m_systemName{ "Graphics" }; //! Name of system

            //! Default texture to use
            std::string m_defaultTextureName{ "Cat" };

            //! Default shader program to use
            std::string m_defaultShaderProgramKey{"Textured"};

            //! Container of meshes
            std::vector<Graphics::MeshData> m_meshes{};

            //! Container of textured and filled objects to draw
            std::vector<Graphics::Renderer> m_triangleObjects{};

            //! Container of textured and filled objects to draw that should have the same size as the window
            std::vector<Graphics::Renderer> m_backgroundObjects{};

            //! Container of debug objects to draw
            std::vector<Graphics::Renderer> m_lineObjects{};

            //! Container of debug points to draw
            std::vector<Graphics::Renderer> m_pointObjects{};

            // font
            Font m_font;


            // ----- For rendering to ImGui window ----- //
            GLuint m_frameBufferObjectIndex{}; //! Frame buffer object to draw to render to ImGui window
            GLuint m_imguiTextureId{}; //! Texture ID of the texture generated to render to the ImGui window
            float m_cachedWindowWidth{ -1.f }, m_cachedWindowHeight{ -1.f };

            // ----- Private methods ----- //
        private:
            /*!***********************************************************************************
             \brief Sets the vertex positions and indices of the object passed in to that of
                    a circle (generated with [segments] number of points along its edges) 
                    centered at the origin and creates a VAO.

                    This function has not been implemented fully.

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
             \brief Prints the graphics specifications of the device.
            *************************************************************************************/
            void PrintSpecifications();


            // ----- For rendering to ImGui window ----- //

            /*!***********************************************************************************
             \brief Creates a frame buffer object with a texture bound to the color buffer so that
                    the texture can be read back and rendered to an ImGui window.                    
                    Throws if the frame buffer object was not created successfully.

             \param[in] bufferWidth Width the buffer should be set to. Should match that of 
                                    the ImGui window.
             \param[in] bufferHeight Height the buffer should be set to. Should match that of 
                                     the ImGui window.
            *************************************************************************************/
            void CreateFrameBuffer(int const bufferWidth, int const bufferHeight);

            /*!***********************************************************************************
             \brief Binds the framebuffer.
            *************************************************************************************/
            void BindFrameBuffer();

            /*!***********************************************************************************
             \brief Unbinds the framebuffer.
            *************************************************************************************/
            void UnbindFrameBuffer();

            /*!***********************************************************************************
             \brief Resizes the texture object to match the size passed in.

             \param[in] width Width the texture object should be set to. 
                              Should match that of the ImGui window.
             \param[in] height Height the buffer should be set to.
                               Should match that of the ImGui window.
            *************************************************************************************/
            void ResizeFrameBuffer(GLsizei const width, GLsizei const height);
        };


    } // End of Graphics namespace
} // End of PE namespace