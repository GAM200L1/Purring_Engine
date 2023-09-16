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
             \brief Clears the color buffer, sets the size of the viewport to the full window
                    and draw a triangle.

             \param[in] width Width of the viewport being rendered to.
             \param[in] height Height of the viewport being rendered to.
            *************************************************************************************/
            void DrawScene(float const width, float const height);


            // ----- Private variables ----- //
        private:
            GLFWwindow* p_windowRef{}; //! Pointer to the GLFW window to render to

            Graphics::Camera m_mainCamera{}; //! Camera object

            std::string m_systemName{ "Graphics" }; //! Name of system

            //! Container of shaders
            std::map<std::string, Graphics::ShaderProgram*> m_shaderPrograms;

            //! Container of meshes
            std::map<std::string, Graphics::MeshData> m_meshes{};

            //! Container of objects to draw
            std::vector<Graphics::Renderer> m_renderableObjects{};


            // ----- For rendering to ImGui window ----- //
            GLuint m_frameBufferObjectIndex{}; //! Frame buffer object to draw to render to ImGui window
            GLuint m_imguiTextureId{}; //! Texture ID of the texture generated to render to the ImGui window


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
                    a 1x1 square centered at the origin and creates a VAO.

             \param[in,out] r_mesh Object containing the mesh data generated.
            *************************************************************************************/
            void InitializeQuadMesh(MeshData& r_mesh);

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