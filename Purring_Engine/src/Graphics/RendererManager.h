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
            void Draw(Renderer const& r_renderer, ShaderProgram& r_shaderProgram, 
                GLenum const primitiveType, glm::mat4 const& r_modelToNdc);

            /*!***********************************************************************************
             \brief Binds the shader program, vertex array object and texture and makes the
                    draw call for the [r_renderer] passed in.

             \param[in] r_renderer Renderer object with all the information to draw with.
             \param[in] r_shaderProgram Shader program to use.
             \param[in] primitiveType GL Primitive type to make the draw call with.
             \param[in] r_modelToNdc 4x4 matrix that transforms coordinates from model to NDC space.
            *************************************************************************************/
            void Draw(EnumMeshType meshType, glm::vec4 const& r_color, ShaderProgram& r_shaderProgram,
                GLenum const primitiveType, glm::mat4 const& r_modelToNdc);

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
             \brief 

             \param[in] orientation (in radians)
            *************************************************************************************/
            glm::mat4 GenerateTransformMatrix(float const width, float const height, 
                float const orientation, float const positionX, float const positionY);


            /*!***********************************************************************************
             \brief 

             \param[in] 
            *************************************************************************************/
            glm::mat4 GenerateTransformMatrix(glm::vec2 const& horizontalVector, 
                glm::vec2 const& verticalVector, glm::vec2 const& centerPosition);

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