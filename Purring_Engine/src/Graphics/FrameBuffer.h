#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FrameBuffer.h
 \date     23-09-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the declaration of the FrameBuffer class, 
            which creates and manages a framebuffer with a texture bound to it.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "Graphics/GLHeaders.h"

#include "VertexData.h"

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
        \brief  Creates and manages a framebuffer with a texture bound to it.
        *************************************************************************************/
        class FrameBuffer
        {
            // ----- Public variables ----- // 
        public:
            static const unsigned int defaultId{ std::numeric_limits<GLuint>::max() }; // Default entity ID to clear the entity ID texture with

            // ----- Public getters ----- // 
        public:
            /*!***********************************************************************************
             \brief Returns the index of the texture bound to the framebuffer.

             \return GLuint - Returns the index of the texture bound to the framebuffer.
            *************************************************************************************/
            inline GLuint GetTextureId() { return m_colorAttachmentIndex; }

            /*!***********************************************************************************
             \brief Returns the index of the framebuffer.

             \return GLuint - Returns the index of the framebuffer.
            *************************************************************************************/
            inline GLuint GetFrameBufferId() { return m_frameBufferObjectIndex; }

            /*!***********************************************************************************
             \brief Returns the width of the framebuffer.

             \return int - Returns the width of the framebuffer.
            *************************************************************************************/
            inline int GetWidth() { return m_bufferWidth; }

            /*!***********************************************************************************
             \brief Returns the height of the framebuffer.

             \return int - Returns the height of the framebuffer.
            *************************************************************************************/
            inline int GetHeight() { return m_bufferHeight; }

            // ----- Public methods ----- // 
        public:
            /*!***********************************************************************************
             \brief Creates a frame buffer object with a texture bound to the color buffer so that
                    the texture can be read back and rendered to an ImGui window. Returns true
                    if the frame buffer object was created successfully, false otherwise.

             \param[in] bufferWidth Width the buffer should be set to.
             \param[in] bufferHeight Height the buffer should be set to.
             \param[in] hasColorAtachment True if the FBO should have a color attachment, false otherwise.
             \param[in] hasDepthAttachment True if the FBO should have a depth attachment, false otherwise.

             \return true - FrameBuffer created successfully.
             \return false - FrameBuffer was not created.
            *************************************************************************************/
            bool CreateFrameBuffer(int const bufferWidth, int const bufferHeight, 
                bool const hasColorAtachment, bool const hasDepthAttachment);

            /*!***********************************************************************************
             \brief Creates a 2D texture and binds it to the FBO to use as a color attachment. 

             \param[in] bufferWidth Width the attachment should be set to.
             \param[in] bufferHeight Height the attachment should be set to.

             \return GLuint - ID of the attachment object created.
            *************************************************************************************/
            GLuint CreateColorAttachment(int const bufferWidth, int const bufferHeight);

            /*!***********************************************************************************
             \brief Creates a 2D texture and binds it to the FBO to use as a depth attachment. 

             \param[in] bufferWidth Width the attachment should be set to.
             \param[in] bufferHeight Height the attachment should be set to.

             \return GLuint - ID of the attachment object created.
            *************************************************************************************/
            GLuint CreateDepthAttachment(int const bufferWidth, int const bufferHeight);

            /*!***********************************************************************************
             \brief Binds the framebuffer.
            *************************************************************************************/
            void Bind() const;

            /*!***********************************************************************************
             \brief Unbinds the framebuffer.
            *************************************************************************************/
            void Unbind() const;

            /*!***********************************************************************************
             \brief Resizes the texture object to match the size passed in.

             \param[in] newWidth Width the texture object should be set to.
             \param[in] newHeight Height the buffer should be set to.
            *************************************************************************************/
            void Resize(int const newWidth, int const newHeight);

            /*!***********************************************************************************
             \brief Sets the values of the buffers and attachments to their default values.

             \param[in] r Red value of the color to default the framebuffer to.
             \param[in] g Green value of the color to default the framebuffer to.
             \param[in] b Blue value of the color to default the framebuffer to.
             \param[in] a Alpha value of the color to default the framebuffer to.
            *************************************************************************************/
            void Clear(float const r, float const g, float const b, float const a);

            /*!***********************************************************************************
             \brief Deletes the framebuffer and the textures attached to it.
            *************************************************************************************/
            void Cleanup();

            // ----- Private variables ----- // 
        private:
            GLuint m_frameBufferObjectIndex{}; // Frame buffer object to draw to
            GLuint m_colorAttachmentIndex{}; // ID of texture attachment that the content is drawn to
            GLuint m_depthAttachmentIndex{}; // ID of depth buffer attachment that the depth pass is drawn to

            int m_bufferWidth{}, m_bufferHeight{}; // Width and height of the buffer object            
            bool m_hasColor{}; // True if this framebuffer has a color attachment
            bool m_hasDepth{}; // True if this framebuffer has a depth attachment
        };
    } // End of Graphics namespace
} // End of PE namespace
