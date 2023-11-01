/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FrameBuffer.cpp
 \date     23-09-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the declaration of the FrameBuffer class, 
            which creates and manages a framebuffer with a texture bound to it.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "prpch.h"
#include "FrameBuffer.h"

#include "Logging/Logger.h" 

extern Logger engine_logger;

#define LONGER

namespace PE
{
    namespace Graphics
    {
        bool FrameBuffer::CreateFrameBuffer(int const bufferWidth, int const bufferHeight, ColorAttachmentType const type)
        {
            // Create a frame buffer
            glGenFramebuffers(1, &m_frameBufferObjectIndex);
            glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObjectIndex);

            // Attach a texture to the framebuffer to render to
            //glGenTextures(1, &m_textureIndex);
            //glBindTexture(GL_TEXTURE_2D, m_textureIndex);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bufferWidth, bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureIndex, 0);

#ifdef LONGER
            glGenTextures(1, &m_textureIndex);
            glBindTexture(GL_TEXTURE_2D, m_textureIndex);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferWidth, bufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexImage2D(GL_TEXTURE_2D, 0, GetTextureInternalFormat(m_textureType), bufferWidth, bufferHeight, 0, GetTextureFormat(m_textureType), GetTextureDataType(m_textureType), NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureIndex, 0);
#else
             m_textureType = type;
             AdjustColorAttachment(bufferWidth, bufferHeight, m_textureType);
#endif

            GLenum const attachments[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, attachments);

            m_bufferWidth = bufferWidth, m_bufferHeight = bufferHeight;

            // Check if the framebuffer was created successfully
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) 
            {
                glDeleteTextures(1, &m_textureIndex);

                switch (status) {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
                    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                    engine_logger.SetTime();
                    engine_logger.AddLog(true, "Framebuffer is incomplete: One or more attachments are not complete.", __FUNCTION__);
                    break;
                }
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
                    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                    engine_logger.SetTime();
                    engine_logger.AddLog(true, "Framebuffer is incomplete: No images are attached.", __FUNCTION__);
                    break;
                }
                case GL_FRAMEBUFFER_UNSUPPORTED: {
                    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                    engine_logger.SetTime();
                    engine_logger.AddLog(true, "Framebuffer configuration is not supported.", __FUNCTION__);
                    break;
                }
                default: {
                    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                    engine_logger.SetTime();
                    engine_logger.AddLog(true, "Framebuffer is incomplete with unknown status code.", __FUNCTION__);
                    break;
                }
                }

                return false;
            }

            // Unbind all the buffers created
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            return true;
        }


        void FrameBuffer::AdjustColorAttachment(int const bufferWidth, int const bufferHeight,
            ColorAttachmentType const type)
        {

#if 0
            glBindTexture(GL_TEXTURE_2D, m_textureIndex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bufferWidth, bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureIndex, 0);
#else
            glBindTexture(GL_TEXTURE_2D, m_textureIndex);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexImage2D(GL_TEXTURE_2D, 0, GetTextureInternalFormat(m_textureType), bufferWidth, bufferHeight, 0, GetTextureFormat(m_textureType), GetTextureDataType(m_textureType), NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureIndex, 0);

            //// Get the texture details
            //GLint internalFormat{ GetTextureInternalFormat(type) };
            //GLenum format{ GetTextureFormat(type) };
            //GLenum dataType{ GetTextureDataType(type) };
            //GLenum colorAttachment{ GL_COLOR_ATTACHMENT0 };

            //// Set the values
            //glBindTexture(GL_TEXTURE_2D, m_textureIndex);
            //glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, bufferWidth, bufferHeight, 0, format, dataType, NULL);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            ////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            ////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            ////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment, GL_TEXTURE_2D, m_textureIndex, 0);
#endif
            //unsigned int bufs[1]{ GL_COLOR_ATTACHMENT0 };
            //glDrawBuffers(1, bufs);
        }


        void FrameBuffer::Bind() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObjectIndex);
        }


        void FrameBuffer::Unbind() const
        {
           GLint currentFrameBuffer{};
           glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFrameBuffer);

            // Unbind the framebuffer object (if it is currently bound)
           if (m_frameBufferObjectIndex == static_cast<GLuint>(currentFrameBuffer))
           {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
           }
        }

        void FrameBuffer::Resize(int const newWidth, int const newHeight)
        {
            // Resize texture to render to
            //glBindTexture(GL_TEXTURE_2D, m_textureIndex);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureIndex, 0);

#ifdef LONGER
            glBindTexture(GL_TEXTURE_2D, m_textureIndex);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexImage2D(GL_TEXTURE_2D, 0, GetTextureInternalFormat(m_textureType), newWidth, newHeight, 0, GetTextureFormat(m_textureType), GetTextureDataType(m_textureType), NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureIndex, 0);
#else
            // Resize the texture to be of the same width and height as that passed in
            AdjustColorAttachment(newWidth, newHeight, m_textureType);
#endif
            m_bufferWidth = newWidth, m_bufferHeight = newHeight;

            GLenum const attachments[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, attachments);
            
            
        }

        
        int FrameBuffer::ReadEntityId(int x, int y)
        {
            if (x < 0 || x >= m_bufferWidth ||
                y < 0 || y >= m_bufferHeight)
            {
                std::cout << "Attempting to read out of bounds\n";
                return defaultId;
            }

            //GLint currentFrameBuffer{};
            //glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFrameBuffer);

            //// Unbind the framebuffer object (if it is currently bound)
            //bool isBound{ m_frameBufferObjectIndex == static_cast<GLuint>(currentFrameBuffer) };
            //if (!isBound)
            //{
            //    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObjectIndex);
            //}

            //// Retrieve current texture
            //GLint currentTexture{};
            //glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

            //// Unbind if currently bound
            //bool isTextureBound{ static_cast<GLuint>(currentTexture) == m_entityTextureId };
            //if (!isTextureBound)
            //{
            //    glBindTexture(GL_TEXTURE_2D, m_entityTextureId);
            //}

            //glReadBuffer(GL_COLOR_ATTACHMENT1);
            //float pixelEntityId{200.f};
            //glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, reinterpret_cast<void*>(&pixelEntityId)); //glReadnPixels
            //glReadBuffer(GL_BACK);

            //if (!isBound)
            //{
            //    glBindFramebuffer(GL_FRAMEBUFFER, currentFrameBuffer);
            //}

            //if (!isTextureBound)
            //{
            //    glBindTexture(GL_TEXTURE_2D, currentTexture);
            //}

            return 0;//static_cast<int>(pixelEntityId);
        }

        
        void FrameBuffer::WritePixel(int x, int y, void const* pixelData)
        {
            if (x < 0 || x >= m_bufferWidth ||
                y < 0 || y >= m_bufferHeight)
            { 
                std::cout << "Attempting to write out of bounds\n";
                return;  
            }


            //GLint currentFrameBuffer{};
            //glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFrameBuffer);

            //// Unbind the framebuffer object (if it is currently bound)
            //bool isBound{ m_frameBufferObjectIndex == static_cast<GLuint>(currentFrameBuffer) };
            //if (!isBound)
            //{
            //    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObjectIndex);
            //}

            //// Retrieve current texture
            //GLint currentTexture{};
            //glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

            //// Unbind if currently bound
            //bool isTextureBound{ static_cast<GLuint>(currentTexture) == m_entityTextureId };
            //if(!isTextureBound)
            //{
            //    glBindTexture(GL_TEXTURE_2D, m_entityTextureId);
            //}

            ////glDrawBuffer(GL_COLOR_ATTACHMENT1);

            ////// Set the raster position to the coordinates of the pixel you want to write to
            ////glRasterPos2i(x, y);

            ////// Draw a single pixel using the glDrawPixels() function
            ////glDrawPixels(1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

            ////glDrawBuffer(GL_COLOR_ATTACHMENT0);


            //if (!isBound)
            //{
            //    glBindFramebuffer(GL_FRAMEBUFFER, currentFrameBuffer);
            //}

            //if (!isTextureBound)
            //{
            //    glBindTexture(GL_TEXTURE_2D, currentTexture);
            //}
        }


        void FrameBuffer::Clear(float const r, float const g, float const b, float const a, EntityID const clearId) 
        {
            //float colorArray[4]{r, g, b, a};
            //glClearTexImage(m_colorTextureId, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(colorArray));
            //glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glClearColor(r, g, b, a);
            glClear(GL_COLOR_BUFFER_BIT);

            //glDrawBuffer(GL_COLOR_ATTACHMENT1);
            //GLclampf idF{ static_cast<GLclampf>(clearId) };
            //glClearColor(idF, idF, idF, idF);
            //glClear(GL_COLOR_BUFFER_BIT);


            //float idInt{ -1.f };
            //glClearTexImage(m_entityTextureId, 0, GL_RED_INTEGER, GL_INT, reinterpret_cast<void*>(&idInt));

            //// Set the clear color to the desired value.
            //unsigned int clearColor = static_cast<unsigned int>(clearId);
            //glClearBufferuiv(GL_COLOR_BUFFER_BIT, 0, &clearColor);

            //glDrawBuffer(GL_COLOR_ATTACHMENT0);
        }


        void FrameBuffer::Cleanup()
        {
            // Delete the framebuffer object
            glDeleteTextures(1, &m_textureIndex);
            //glDeleteTextures(1, &m_entityTextureId);
            glDeleteFramebuffers(1, &m_frameBufferObjectIndex);
        }


        GLint FrameBuffer::GetTextureInternalFormat(ColorAttachmentType const type)
        {
            switch (type) {
            case ColorAttachmentType::RGBA_COLOR: return GL_RGBA;
            case ColorAttachmentType::RED_INTEGER: default: return GL_R32I;
            }
        }


        GLenum FrameBuffer::GetTextureFormat(ColorAttachmentType const type)
        {
            switch (type) {
            case ColorAttachmentType::RGBA_COLOR: return GL_RGBA;
            case ColorAttachmentType::RED_INTEGER: default: return GL_RED_INTEGER;
            }
        }


        GLenum FrameBuffer::GetTextureDataType(ColorAttachmentType const type)
        {
            switch (type) {
            case ColorAttachmentType::RGBA_COLOR: return GL_UNSIGNED_BYTE;
            case ColorAttachmentType::RED_INTEGER: default: return GL_INT;
            }
        }
    } // End of Graphics namespace
} // End of PE namespace
