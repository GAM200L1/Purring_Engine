#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Renderer.h
 \date     20-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the Renderer class, which acts as a base 
            component to add to gameobjects to be rendered.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "Graphics/GLHeaders.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "VertexData.h"

// This namespace contains classes made to store the data
// that should be found in the gameobject / transform components
// that have yet to be implemented
namespace temp
{
    class Transform
    {
    public:
        float width{400.f}, height{400.f};  //! Width and height of the object
        float orientation{0.f};         //! Counterclockwise angle (in degrees) about z
        glm::vec2 position{};           //! Position of the center in world space

    public:
        /*!***********************************************************************************
        \brief  Gets the matrix to transform coordinates in model space to world space as
                a 4x4 matrix.

        \return glm::mat4 - 4x4 matrix to transform coordinates in model space to world space.
        *************************************************************************************/
        glm::mat4 GetTransformMatrix() const
        {
            // Get scale matrix
            glm::mat4 scale_matrix{
                width,  0.f,    0.f, 0.f,
                0.f,    height, 0.f, 0.f,
                0.f,    0.f,    1.f, 0.f,
                0.f,    0.f,    0.f, 1.f
            };

            // Get rotation matrix
            GLfloat angle_rad{ glm::radians(orientation) };
            GLfloat sin_angle{ glm::sin(angle_rad) };
            GLfloat cos_angle{ glm::cos(angle_rad) };
            glm::mat4 rotation_matrix{
                cos_angle,  sin_angle, 0.f, 0.f,
                -sin_angle, cos_angle, 0.f, 0.f,
                0.f,        0.f,       1.f, 0.f,
                0.f,        0.f,       0.f, 1.f
            };

            // Get translation matrix
            glm::mat4 translation_matrix{
                1.f,    0.f,    0.f,    0.f,
                0.f,    1.f,    0.f,    0.f,
                0.f,    0.f,    1.f,    0.f,
                position.x, position.y, 0.f, 1.f
            };

            return translation_matrix * rotation_matrix * scale_matrix;
        }
    };

    // Temp class to store texture object members and helper functions
    class Texture
    {
    public:
        GLuint textureObjectHandle{};		//! Handle to texture object
        GLuint textureUnit{};				    //! Texture unit that the object is bound to
        GLenum textureWrapMode{ GL_REPEAT }; //! Texture wrap mode

        /*!***********************************************************************************
         \brief Binds this texture object.
        *************************************************************************************/
        void BindTextureObject() const
        {
            // Bind the texture
            glBindTextureUnit(textureUnit, textureObjectHandle);

            // Set the wrap mode
            glTextureParameteri(textureObjectHandle, GL_TEXTURE_WRAP_S, static_cast<GLint>(textureWrapMode));
            glTextureParameteri(textureObjectHandle, GL_TEXTURE_WRAP_T, static_cast<GLint>(textureWrapMode));
        }

        /*!***********************************************************************************
         \brief Unbinds this texture object if it is bound.

         \param[in] textureUnit Texture unit that this object was bound to
        *************************************************************************************/
        void UnbindTextureObject() const
        {
            // Retrieve current texture
            GLint currentTexture{};
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

            // Unbind if currently bound
            if (static_cast<GLuint>(currentTexture) == textureObjectHandle) 
            {
                glBindTextureUnit(textureUnit, 0);
            }
        }

        /*!***********************************************************************************
         \brief Creates a 2D texture object and stores the handle.

         \param[in] pathname Filepath of the texture to load and use as the texture object.
        *************************************************************************************/
        void CreateTextureObject(std::string pathname)
        {
            GLuint width{ 256 }, height{ 256 }, bytes_per_texel{ 4 };

            // Open the texture file
            std::ifstream ifs{ pathname, std::ios::binary };

            if (!ifs) {
                std::cout << "ERROR: Unable to open texture file: "
                    << pathname << "\n";
                return;
            }
            ifs.seekg(std::ios::beg);

            // Copy contents of the file to heap memory
            GLuint num_bytes{ width * height * bytes_per_texel };
            char* ptr_texels{ new char[static_cast<std::size_t>(num_bytes)] };

            while (ifs) {
                ifs.read(ptr_texels,
                    static_cast<std::streamsize>(num_bytes));
            }

            // Define and initialize a handle to the texture object
            GLuint texobj_hdl;
            glCreateTextures(GL_TEXTURE_2D, 1, &texobj_hdl);

            // Allocate GPU storage for texture image data loaded from file
            glTextureStorage2D(texobj_hdl, 1, GL_RGBA8,
                static_cast<GLsizei>(width), static_cast<GLsizei>(height));

            // Copy image data from client memory to GPU texture buffer memory
            glTextureSubImage2D(texobj_hdl, 0, 0, 0,
                static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(ptr_texels));


            // Delete client-side memory; Data is buffered on GPU
            delete[] ptr_texels;

            textureObjectHandle = texobj_hdl;

        }
    };
}


namespace PE
{
    namespace Graphics
    {
        //! Types of primitives 
        enum class EnumMeshType : unsigned char
        {
            QUAD,
            TRIANGLE,
            DEBUG_SQUARE,
            DEBUG_CIRCLE,
            DEBUG_LINE
        };

        // Renderer component. Attach one to each gameobject to be drawn.
        class Renderer
        {
            // ----- Public variables ----- //
        public:
            bool enabled{ true }; // Set to true to render the object, false not to.
            glm::vec4 color{ 1.f, 0.f, 0.f, 0.5f }; // RGBA values of a color in a range of 0 to 1
            std::string shaderProgramName{ "basic" }; // Name of the shader program
            Graphics::EnumMeshType meshType{ EnumMeshType::QUAD }; // Type of mesh
            temp::Transform transform{};
            temp::Texture texture{}; // Will move the texture object out later
            // obj factory will set a hidden layer value
        };
    } // End of Graphics namespace
} // End of PE namespace