#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     VertexBufferObject.h
 \date     11-12-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the VertexBufferObject class, which manages 
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "GLHeaders.h"

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
         \brief 
        *************************************************************************************/
        class BufferElement
        {

        public:
            BufferElement()
                { /* Empty by design */ } 

        private:
            std::size_t SizeOfGLType(GLenum glType){
                switch(glType){
                    case GL_BOOL: { return sizeof(GLboolean); }
                    case GL_BYTE: { return sizeof(GLbyte); }
                    case GL_UNSIGNED_BYTE: { return sizeof(GLubyte); }
                    case GL_SHORT: { return sizeof(GLshort); }
                    case GL_UNSIGNED_SHORT: { return sizeof(GLushort); }
                    case GL_INT: { return sizeof(GLint); }
                    case GL_UNSIGNED_INT: { return sizeof(GLuint); }
                    case GL_FLOAT: { return sizeof(GLfloat); }
                    case GL_DOUBLE: { return sizeof(GLdouble); }
                    default: { return 0; }
                }
            }

        private:

            bool normalized;
        }; // End of BufferElement class

        /*!***********************************************************************************
         \brief 
        *************************************************************************************/
        class VertexBufferObject
        {    
        public:
            using BufferLayout = std::vector<BufferElement>;
        public: 
            VertexBufferObject(BufferLayout const& layout) : m_bufferElements{layout} 
                { /* Empty by design */ }

        public:
            std::size_t GetNumberOfElements() { return m_bufferElements.size(); }
        
        private:
            void SetLayout()
            {
                // Recalculate the strides
            }

        private:
            BufferLayout m_bufferElements;
        }; // End of VertexBufferObject class

        class VertexArrayObject
        {

        }; // End of VertexArrayObject class

    } // End of Graphics namespace
} // End of PE namespace