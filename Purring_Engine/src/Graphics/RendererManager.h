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
#include <cstddef>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>
#include <vector>

#include "Camera.h"
#include "MeshData.h"
#include "Renderer.h"
#include "ShaderProgram.h"


namespace PE
{
    namespace Graphics
    {
        // In charge of calling the draw functions in all the renderer components.
        class RendererManager
        {
            // ----- Enum Definitions ----- //
        public:
            enum class RenderMode {
                wireframe_black = 0,
                wireframe_color,
                triangle_flat,
                triangle_smooth
            };

            // ----- Public methods ----- //
        public:
            /*!***********************************************************************************
             \brief Sets the reference to the window to draw to. Creates a VAO and VBO for a
                    quad and stores it, and compiles and links a simple shader program to draw
                    the triangle.

             \param[in,out] p_window Pointer to the GLFWwindow to render to.
            *************************************************************************************/
            void Initialize(GLFWwindow* p_window);

            /*!***********************************************************************************
             \brief Delete the buffers, VAO and shader program allocated through OpenGL.
            *************************************************************************************/
            void DestroyManager();

            /*!***********************************************************************************
             \brief Clears the color buffer, sets the size of the viewport to the full window
                    and draw a triangle.
            *************************************************************************************/
            void DrawScene();


            // ----- Private variables ----- //
        private:
            GLFWwindow* p_windowRef{};

            Graphics::Camera mainCamera{}; //! Camera object

            //! Container of shaders
            std::map<std::string, Graphics::ShaderProgram*> shaderPrograms;

            //! Container of meshes
            std::map<std::string, Graphics::MeshData> meshes{};

            //! Container of objects to draw
            std::vector<Graphics::Renderer> renderableObjects{};

            // ----- Private methods ----- //
        private:
            /*!***********************************************************************************
             \brief Generates [segments] number of points along the edge of a circle
                    and stores it in [r_points].

             \param[in] segments Number of edges that should make up the circle.
             \param[in,out] r_points Vector that the vertex positions should be output to.
            *************************************************************************************/
            void GenerateCirclePoints(std::size_t const segments, std::vector<glm::vec2>& r_points);
        };


    } // End of Graphics namespace
} // End of PE namespace