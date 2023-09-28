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
            if (GetHasChanged())
            {
                glm::vec2 up{ -glm::sin(m_orientation), glm::cos(m_orientation) };
                glm::vec2 right{ up.y, -up.x };

                float upDotPosition{ (up.x * m_position.x + up.y * m_position.y) };
                float rightDotPosition{ (right.x * m_position.x + right.y * m_position.y) };

                float scaleReciprocal{ 1.f / m_magnification };

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

                // Update the cached values
                m_cachedViewMatrix = viewScale * viewRotation;
                m_cachedPosition = m_position;
                m_cachedOrientation = m_orientation;
                m_cachedMagnification = m_magnification;
            }

            return m_cachedViewMatrix;
        }

        bool Camera::GetHasChanged() const
        {
            return (m_cachedPosition != m_position ||
                m_cachedOrientation != m_orientation ||
                m_cachedMagnification != m_magnification);
        }

        void Camera::SetMagnification(float const value)
        {
            // Clamping to 10 is arbitrary.
            m_magnification = glm::clamp(value, 0.1f, 10.f);
        }


        void Camera::SetPosition(float const valueX, float const valueY)
        {
            m_position.x = valueX;
            m_position.y = valueY;
        }


        void Camera::SetRotationDegrees(float const degrees)
        {
            float angleRadians{ glm::radians(degrees) };
            m_orientation = angleRadians;
        }


        void Camera::SetRotationRadians(float const radians)
        {
            m_orientation = radians;
        }


        void Camera::AdjustMagnification(float const delta)
        {
            SetMagnification(m_magnification + delta);
        }

        
        void Camera::AdjustPosition(float const deltaX, float const deltaY)
        {
            m_position.x += deltaX;
            m_position.y += deltaY;
        }


        void Camera::AdjustRotationDegrees(float const delta)
        {
            float angleRadians{ glm::radians(delta) };
            m_orientation += angleRadians;
        }


        void Camera::AdjustRotationRadians(float const delta)
        {
            m_orientation += delta;
        }
    } // End of Graphics namespace
} // End of PE namspace
