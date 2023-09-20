/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Transform.h
 \date     20-09-2023

 \author               Liew Yeni
 \par      email:      yeni.l\@digipen.edu

 \brief     Contains the implementation of ComponentPool.


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#pragma once

#include "glm/glm.hpp" //! Only used to convert mat3x3 to glm::mat3x3 for graphics use

#include "Math/MathCustom.h"
#include "ECS/Components.h"

//struct Transform
//{
//public:
//    vec2 scale;                     //! Width and height of the object
//    float orientation{ 0.f };       //! Counterclockwise angle (in degrees) about z
//    vec2 position{};           //! Position of the center in world space
//
//public:
//    /*!***********************************************************************************
//    \brief  Gets the matrix to transform coordinates in model space to world space as
//            a 4x4 matrix.
//
//    \return glm::mat4 - 4x4 matrix to transform coordinates in model space to world space.
//    *************************************************************************************/
//    glm::mat4 GetTransformMatrix() const
//    {
//        // Get scale matrix
//        glm::mat4 scale_matrix{
//            width,  0.f,    0.f, 0.f,
//            0.f,    height, 0.f, 0.f,
//            0.f,    0.f,    1.f, 0.f,
//            0.f,    0.f,    0.f, 1.f
//        };
//
//        // Get rotation matrix
//        GLfloat angle_rad{ glm::radians(orientation) };
//        GLfloat sin_angle{ glm::sin(angle_rad) };
//        GLfloat cos_angle{ glm::cos(angle_rad) };
//        glm::mat4 rotation_matrix{
//            cos_angle,  sin_angle, 0.f, 0.f,
//            -sin_angle, cos_angle, 0.f, 0.f,
//            0.f,        0.f,       1.f, 0.f,
//            0.f,        0.f,       0.f, 1.f
//        };
//
//        // Get translation matrix
//        glm::mat4 translation_matrix{
//            1.f,    0.f,    0.f,    0.f,
//            0.f,    1.f,    0.f,    0.f,
//            0.f,    0.f,    1.f,    0.f,
//            position.x, position.y, 0.f, 1.f
//        };
//
//        return translation_matrix * rotation_matrix * scale_matrix;
//    }
//};

namespace PE
{
    struct Transform : public Component
    {
        float width, height;
        float orientation; // in radians
        vec2 position;

        mat3x3 GetTransformMatrix3x3() const
        {
            mat3x3 scaleMat;
            scaleMat.Scale(width, height);
            mat3x3 rotMat;
            rotMat.RotateRad(orientation);
            mat3x3 transMat;
            transMat.Translate(position.x, position.y);

            return transMat * rotMat * scaleMat;
        }
        mat4x4 GetTransformMatrix4x4() const
        {
            return GetTransformMatrix3x3().ConvertTo4x4();
        }

        // Conversion functions
        glm::mat4 GetTransformMatrix() const // for graphics use only
        {
            // Get scale matrix
            glm::mat4 scale_matrix{
                width,  0.f,    0.f, 0.f,
                0.f,    height, 0.f, 0.f,
                0.f,    0.f,    1.f, 0.f,
                0.f,    0.f,    0.f, 1.f
            };
    
            // Get rotation matrix
            float angle_rad{ glm::radians(orientation) };
            float sin_angle{ glm::sin(angle_rad) };
            float cos_angle{ glm::cos(angle_rad) };
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

        glm::vec2 GetPosition() const
        {
            glm::vec2 ret{ position.x, position.y };
            return ret;
        }
    };
}