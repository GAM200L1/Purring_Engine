/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Camera.cpp
 \date     28-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the definition of the member functions of the camera 
           class.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "prpch.h"
#include "Camera.h"

namespace PE 
{
    namespace Graphics
    {
        glm::mat4 Camera::GetWorldToViewMatrix()
        {
            float angleRadians{ glm::radians(orientation) };
            glm::vec2 up{ -glm::sin(angleRadians), glm::cos(angleRadians) };
            glm::vec2 right{ up.y, -up.x };

            float upDotPosition{ (up.x * position.x + up.y * position.y) };
            float rightDotPosition{ (right.x * position.x + right.y * position.y) };

            float scaleReciprocal{ 1.f / magnification };

            glm::mat4 viewScale = {
                scaleReciprocal, 0.f, 0.f, 0.f,
                0.f, scaleReciprocal, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                0.f, 0.f, 0.f, 1.f
            };

            glm::mat4 viewRotation = {
                right.x, up.x, 0.f, 0.f,
                right.y, up.y, 0.f, 0.f,
                0.f,    0.f,   1.f, 0.f,
                -rightDotPosition, -upDotPosition, 1.f, 1.f
            };

            return (viewScale * viewRotation);
        }

        void Camera::SetMagnification(float delta)
        {
            // Clamping to 10 is arbitrary.
            magnification = glm::clamp(magnification + delta, 0.1f, 10.f);
        }
    } // End of Graphics namespace
} // End of PE namspace
