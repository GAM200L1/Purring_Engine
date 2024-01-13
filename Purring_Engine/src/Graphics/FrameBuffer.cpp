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

namespace PE
{
    namespace Graphics
    {
        bool FrameBuffer::CreateFrameBuffer(int const bufferWidth, int const bufferHeight, 
            bool const hasColorAtachment, bool const hasDepthAttachment, std::string const& r_name)
        {
            // Create a frame buffer
            glGenFramebuffers(1, &m_frameBufferObjectIndex);
            glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObjectIndex);

            m_hasColor = hasColorAtachment;
            m_hasDepth = hasDepthAttachment;

            m_colorAttachmentIndex = (m_hasColor ? CreateColorAttachment(bufferWidth, bufferHeight) : 0);
            m_depthAttachmentIndex = (m_hasDepth ? CreateDepthAttachment(bufferWidth, bufferHeight) : 0);

            m_bufferWidth = bufferWidth, m_bufferHeight = bufferHeight;

            // Disable color buffers and just do depth pass
            if (!m_colorAttachmentIndex)
            {
                glDrawBuffer(GL_NONE);
            }

            // Check if the framebuffer was created successfully
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) 
            {
                glDeleteTextures(1, &m_colorAttachmentIndex);
                glDeleteTextures(1, &m_depthAttachmentIndex);

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

            if (!r_name.empty()) {
                GLsizei vboNameLength{ static_cast<GLsizei>(r_name.length()) };
                glObjectLabel(GL_FRAMEBUFFER, m_frameBufferObjectIndex, vboNameLength, r_name.c_str());
            }

            // Unbind all the buffers created
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            return true;
        }


        GLuint FrameBuffer::CreateColorAttachment(int const bufferWidth, int const bufferHeight)
        {
            // Create a texture to be used as a color attachment
            GLuint returnIndex{};
            glGenTextures(1, &returnIndex);
            glBindTexture(GL_TEXTURE_2D, returnIndex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bufferWidth, bufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

            // Set the parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            const GLenum buffer{ GL_COLOR_ATTACHMENT0 };
            glFramebufferTexture2D(GL_FRAMEBUFFER, buffer, GL_TEXTURE_2D, returnIndex, 0);
            glDrawBuffers(1, &buffer);

            return returnIndex;
        }


        GLuint FrameBuffer::CreateDepthAttachment(int const bufferWidth, int const bufferHeight)
        {
            // Create a texture to be used as a color attachment
            GLuint returnIndex{};
            glGenTextures(1, &returnIndex);
            glBindTexture(GL_TEXTURE_2D, returnIndex);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);

            // Set the parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, returnIndex, 0);

            return returnIndex;
        
        }


        void FrameBuffer::Bind() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObjectIndex);

            const GLenum buffer{ GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, &buffer);
        }


        void FrameBuffer::Unbind() const
        {
           //GLint currentFrameBuffer{};
           //glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFrameBuffer);

           // // Unbind the framebuffer object (if it is currently bound)
           //if (m_frameBufferObjectIndex == static_cast<GLuint>(currentFrameBuffer))
           //{
           //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
           //}

           glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void FrameBuffer::Resize(int const newWidth, int const newHeight)
        {
            // Check if the size passed in is valid
            if (newWidth == 0 || newHeight == 0) { return; }

            Cleanup(); // Delete the old FBO

            // Make a new FBO
            CreateFrameBuffer(newWidth, newHeight, m_hasColor, m_hasDepth);
        }


        void FrameBuffer::Clear(float const r, float const g, float const b, float const a) 
        {
            //const GLenum buffer{ GL_COLOR_ATTACHMENT0 };
            //glDrawBuffers(1, &buffer);
            glClearColor(r, g, b, a);
            glClear(GL_COLOR_BUFFER_BIT);
        }


        void FrameBuffer::Cleanup()
        {
            // Delete the framebuffer object
            glDeleteTextures(1, &m_colorAttachmentIndex);
            glDeleteTextures(1, &m_depthAttachmentIndex);
            glDeleteFramebuffers(1, &m_frameBufferObjectIndex);

            m_colorAttachmentIndex = 0;
            m_depthAttachmentIndex = 0;
            m_frameBufferObjectIndex = 0;
        }
    } // End of Graphics namespace
} // End of PE namespace
