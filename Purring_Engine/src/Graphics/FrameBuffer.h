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
#include "ECS/Entity.h"

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
            inline GLuint GetTextureId() { return m_textureIndex; }

            /*!***********************************************************************************
             \brief Returns the index of the framebuffer.

             \return GLuint - Returns the index of the framebuffer.
            *************************************************************************************/
            inline GLuint GetFrameBufferId() { return m_frameBufferObjectIndex; }
            
            // ----- Public methods ----- // 
        public:
            /*!***********************************************************************************
             \brief Creates a frame buffer object with a texture bound to the color buffer so that
                    the texture can be read back and rendered to an ImGui window. Returns true 
                    if the frame buffer object was created successfully, false otherwise.

             \param[in] bufferWidth Width the buffer should be set to.
             \param[in] bufferHeight Height the buffer should be set to.

             \return true - FrameBuffer created successfully.
             \return false - FrameBuffer was not created.
            *************************************************************************************/
            bool CreateFrameBuffer(int const bufferWidth, int const bufferHeight);

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

             \param[in] width Width the texture object should be set to.
             \param[in] height Height the buffer should be set to.
            *************************************************************************************/
            void Resize(int const newWidth, int const newHeight);

            /*!***********************************************************************************
             \brief Reads the ID of the entity being drawn at pixel at the specified coordinate.

             \param[in] x x-coordinate of the pixel in the entity texture.
             \param[in] y y-coordinate of the pixel in the entity texture.
            *************************************************************************************/
            int ReadEntityId(int x, int y);

            /*!***********************************************************************************
             \brief Sets the value of the pixel at the specified coordinate.

             \param[in] x x-coordinate of the pixel in the entity texture.
             \param[in] y y-coordinate of the pixel in the entity texture.
             \param[in] pixelData Data to write to the pixel in the texture.
            *************************************************************************************/
            void WritePixel(int x, int y, void const* pixelData);

            /*!***********************************************************************************
             \brief Sets the values of the buffers and attachments to their default values.

             \param[in] r Red value of the color to default the framebuffer to.
             \param[in] g Green value of the color to default the framebuffer to.
             \param[in] b Blue value of the color to default the framebuffer to.
             \param[in] a Alpha value of the color to default the framebuffer to.
             \param[in] clearId Entity ID to default all the pixels to.
            *************************************************************************************/
            void Clear(float const r, float const g, float const b, float const a, EntityID const clearId);

            /*!***********************************************************************************
             \brief Deletes the framebuffer and the textures attached to it.
            *************************************************************************************/
            void Cleanup();

            // ----- Private variables ----- // 
        private:
            GLuint m_frameBufferObjectIndex{}; // Frame buffer object to draw to
            GLuint m_textureIndex{}; // ID of texture attachment that the scene window content is drawn to
            GLuint m_entityTextureId{}; // ID of texture attachment that the entity IDs are written to (for screen picking)

            int m_bufferWidth{}, m_bufferHeight{}; // Width and height of the buffer object            
        };
    } // End of Graphics namespace
} // End of PE namespace
