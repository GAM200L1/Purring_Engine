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

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "Graphics/GLHeaders.h"
#include <vector>

#include "VertexData.h"

namespace PE
{
    namespace Graphics
    {
        class FrameBuffer
        {
            // ----- Public getters ----- // 
        public:
            inline GLuint GetTextureId() { return m_textureIndex; }
            inline GLuint GetFrameBufferId() { return m_frameBufferObjectIndex; }
            
            // ----- Public methods ----- // 
        public:
            /*!***********************************************************************************
             \brief Creates a frame buffer object with a texture bound to the color buffer so that
                    the texture can be read back and rendered to an ImGui window.
                    Throws if the frame buffer object was not created successfully.

             \param[in] bufferWidth Width the buffer should be set to. Should match that of
                                    the ImGui window.
             \param[in] bufferHeight Height the buffer should be set to. Should match that of
                                     the ImGui window.

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
                              Should match that of the ImGui window.
             \param[in] height Height the buffer should be set to.
                               Should match that of the ImGui window.
            *************************************************************************************/
            void Resize(int const newWidth, int const newHeight);

            /*!***********************************************************************************
             \brief Deletes the framebuffer and the textures attached to it.
            *************************************************************************************/
            void Cleanup();

            // ----- Private variables ----- // 
        private:
            GLuint m_frameBufferObjectIndex{}; //! Frame buffer object to draw to
            GLuint m_textureIndex{}; //! Texture ID of the texture generated to attach to the framebuffer
        };
    } // End of Graphics namespace
} // End of PE namespace
