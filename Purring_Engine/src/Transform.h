#pragma once

struct Transform
{
public:
    float width{ 400.f }, height{ 400.f };  //! Width and height of the object
    float orientation{ 0.f };         //! Counterclockwise angle (in degrees) about z
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