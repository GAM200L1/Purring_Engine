/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     MeshData.cpp
 \date     28-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the definitions of the functions in the MeshData class, 
            which create and manage the vertex array object created using data
            from the MeshData class.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "prpch.h"
#include "MeshData.h"
#include "../Logging/Logger.h"

extern Logger engine_logger;

namespace PE
{
    namespace Graphics
    {
        bool MeshData::CreateVertexArrayObject()
        {
            // Check if the mesh data is valid
            if (vertices.empty())
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "This mesh has no vertex data.", __FUNCTION__);

                return false;
            }
            else if (indices.empty())
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "This mesh has no index data.", __FUNCTION__);

                return false;
            }

            // Create buffer object for vertex data
            glCreateBuffers(1, &m_vertexBufferObject);
            if (0 == m_vertexBufferObject)
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Unable to create Vertex Buffer Object.", __FUNCTION__);

                return false;
            }
            glNamedBufferStorage(m_vertexBufferObject,
                static_cast<GLsizeiptr>(sizeof(VertexData) * vertices.size()),
                reinterpret_cast<GLvoid*>(vertices.data()),
                GL_DYNAMIC_STORAGE_BIT);

            // Create buffer object for element data
            glCreateBuffers(1, &m_elementBufferObject);
            if (0 == m_elementBufferObject)
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Unable to create Element Buffer Object.", __FUNCTION__);

                glDeleteBuffers(1, &m_vertexBufferObject);
                return false;
            }
            glNamedBufferStorage(m_elementBufferObject,
                static_cast<GLsizeiptr>(indices.size() * sizeof(GLushort)),
                reinterpret_cast<GLvoid*>(indices.data()), GL_DYNAMIC_STORAGE_BIT);

            // Create VAO
            glCreateVertexArrays(1, &m_vertexArrayObject);
            if (0 == m_vertexArrayObject)
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Unable to create Vertex Array Object.", __FUNCTION__);

                glDeleteBuffers(1, &m_vertexBufferObject);
                glDeleteBuffers(1, &m_elementBufferObject);
                return false;
            }

            // Bind the positions to attrib 0
            GLuint attributeIndex{ 0 }, bindingIndex{ 0 };
            glEnableVertexArrayAttrib(m_vertexArrayObject, attributeIndex);
            glVertexArrayVertexBuffer(m_vertexArrayObject, bindingIndex, m_vertexBufferObject, 0,
                static_cast<GLsizei>(sizeof(VertexData)));
            glVertexArrayAttribFormat(m_vertexArrayObject, attributeIndex, 2, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(m_vertexArrayObject, attributeIndex, bindingIndex);

            // Bind the colors to attrib 1
            attributeIndex = 1, bindingIndex = 1;
            glEnableVertexArrayAttrib(m_vertexArrayObject, attributeIndex);
            glVertexArrayVertexBuffer(m_vertexArrayObject, bindingIndex, m_vertexBufferObject, 0,
                static_cast<GLsizei>(sizeof(VertexData)));
            glVertexArrayAttribFormat(m_vertexArrayObject, attributeIndex, 3, GL_FLOAT, GL_FALSE,
                static_cast<GLuint>(sizeof(glm::vec2)));	// offset by vert pos
            glVertexArrayAttribBinding(m_vertexArrayObject, attributeIndex, bindingIndex);

            // Bind the texture coordinate to attrib 2
            attributeIndex = 2, bindingIndex = 2;
            glEnableVertexArrayAttrib(m_vertexArrayObject, attributeIndex);
            glVertexArrayVertexBuffer(m_vertexArrayObject, bindingIndex, m_vertexBufferObject, 0,
                static_cast<GLsizei>(sizeof(VertexData)));
            glVertexArrayAttribFormat(m_vertexArrayObject, attributeIndex, 2, GL_FLOAT, GL_FALSE,
                static_cast<GLintptr>(sizeof(glm::vec2) + sizeof(glm::vec3))); // offset by vert pos and color
            glVertexArrayAttribBinding(m_vertexArrayObject, attributeIndex, bindingIndex);

            // Bind the element array to the vert array
            glVertexArrayElementBuffer(m_vertexArrayObject, m_elementBufferObject);

            // Break the binding
            glBindVertexArray(0);

            return true;
        }


        void MeshData::BindMesh()
        {
            glBindVertexArray(m_vertexArrayObject);
        }


        void MeshData::UnbindMesh()
        {
            // Check if this mesh's VAO is currently bound
            GLint out{};
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &out);
            if (static_cast<GLuint>(out) == m_vertexArrayObject)
            {
                // Unbind this VAO
                glBindVertexArray(0);
            }
        }


        void MeshData::Cleanup()
        {
            // Delete the vao, vbo and ebo
            glDeleteBuffers(1, &m_vertexBufferObject);
            glDeleteBuffers(1, &m_elementBufferObject);
            glDeleteVertexArrays(1, &m_vertexArrayObject);
        }
    } // End of Graphics namespace
} // End of PE namespace
