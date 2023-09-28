#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     MeshData.cpp
 \date     28-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the declaration of the MeshData class, 
            which create and manage the vertex array object created using data
            from the MeshData class.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "Graphics/GLHeaders.h"
#include <glm/glm.hpp>
#include <vector>

#include "VertexData.h"

namespace PE
{
    namespace Graphics
    {
        class MeshData
        {
            // ----- Public variables ----- // 
        public:
            std::vector<VertexData> vertices{}; //! Vertex coordinates in model space
            std::vector<GLushort> indices{};    //! Order to draw the vertices coordinates in

            // ----- Public methods ----- // 
        public:
            /*!***********************************************************************************
             \brief Returns the index of the vertex array object.

             \return GLuint - The index of the vertex array object.
            *************************************************************************************/
            inline GLuint GetVertexArrayObjectIndex() const { return m_vertexArrayObject; }

            // ----- Public methods ----- // 
        public:
            /*!***********************************************************************************
             \brief Creates a vertex array object from the mesh data.

             \return true - If the Vertex Array Object was created successfully.
             \return false - If the Vertex Array Object was not created.
            *************************************************************************************/
            bool CreateVertexArrayObject();

            /*!***********************************************************************************
             \brief Binds the vertex array object associated with this mesh from
                    the OpenGL context.
            *************************************************************************************/
            void Bind() const;

            /*!***********************************************************************************
             \brief Unbinds the vertex array object associated with this mesh from
                    the OpenGL context.
            *************************************************************************************/
            void Unbind() const;

            /*!***********************************************************************************
             \brief Returns the buffers used by this mesh back to the GPU.
            *************************************************************************************/
            void Cleanup();


            // ----- Private variables ----- // 
        private:
            GLuint		m_vertexArrayObject{};    //! Handle to Vertex Array Object (VAO)
            GLuint		m_vertexBufferObject{};   //! Handle to Vertex Buffer Object (VBO)
            GLuint		m_elementBufferObject{};  //! Handle to Element Buffer Object (EBO)
        };
    } // End of Graphics namespace
} // End of PE namespace
