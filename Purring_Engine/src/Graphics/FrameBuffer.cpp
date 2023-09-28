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
        bool FrameBuffer::CreateFrameBuffer(int const bufferWidth, int const bufferHeight)
        {
            // Create a frame buffer
            glGenFramebuffers(1, &m_frameBufferObjectIndex);
            glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObjectIndex);

            // Attach a texture to the framebuffer
            glGenTextures(1, &m_textureIndex);
            glBindTexture(GL_TEXTURE_2D, m_textureIndex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bufferWidth, bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureIndex, 0);

            // Check if the framebuffer was created successfully
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
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
            glBindTexture(GL_TEXTURE_2D, m_textureIndex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureIndex, 0);
        }

        void FrameBuffer::Cleanup()
        {
            // Delete the framebuffer object
            glDeleteTextures(1, &m_textureIndex);
            glDeleteFramebuffers(1, &m_frameBufferObjectIndex);
        }
    } // End of Graphics namespace
} // End of PE namespace
