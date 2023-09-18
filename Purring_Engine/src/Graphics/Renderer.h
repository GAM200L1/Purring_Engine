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
#include "Texture.h"

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
            Graphics::EnumMeshType meshType{ EnumMeshType::QUAD }; // Type of mesh
            temp::Transform transform{};
            std::shared_ptr<Graphics::Texture> p_texture{ nullptr }; // Will move the texture object out later
            // obj factory will set a hidden layer value
        };
    } // End of Graphics namespace
} // End of PE namespace