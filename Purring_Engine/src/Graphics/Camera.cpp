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
#include <glm/gtc/matrix_transform.hpp> // ortho()

namespace PE 
{
    namespace Graphics
    {
        glm::mat4 Camera::GetWorldToViewMatrix(Transform const& r_transform)
        {
            if (GetHasChanged(r_transform))
            {
                ComputeViewMatrix(r_transform);
            }

            return m_cachedViewMatrix;
        }


        glm::mat4 Camera::GetViewToNdcMatrix() const
        {
            float halfWidth{ m_viewportWidth * 0.5f };
            float halfHeight{ m_viewportHeight * 0.5f };

            return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -m_magnification, m_magnification);
        }


        glm::mat4 Camera::GetWorldToNdcMatrix(Transform const& r_transform)
        {
            if (GetHasChanged(r_transform) || hasViewportChanged)
            {
                m_cachedWorldToNdcMatrix = GetWorldToViewMatrix(r_transform) * GetViewToNdcMatrix();
                hasViewportChanged = false;
            }

            return m_cachedWorldToNdcMatrix;
        }


        bool Camera::GetHasChanged(Transform const& r_transform) const
        {
            return (m_cachedPositionX != r_transform.position.x ||
                m_cachedPositionY != r_transform.position.y ||
                m_cachedOrientation != r_transform.orientation ||
                hasTransformChanged);
        }


        void Camera::ComputeViewMatrix(Transform const& r_transform)
        {
            float scaleReciprocal{ 1.f / m_magnification };

            glm::vec2 up{ -glm::sin(r_transform.orientation), glm::cos(r_transform.orientation) };
            up *= scaleReciprocal;

            glm::vec2 right{ up.y, -up.x };

            float upDotPosition{ (up.x * r_transform.position.x + up.y * r_transform.position.y) };
            float rightDotPosition{ (right.x * r_transform.position.x + right.y * r_transform.position.y) };


            // Update the cached values
            m_cachedViewMatrix = glm::mat4{
                right.x, up.x, 0.f, 0.f,
                right.y, up.y, 0.f, 0.f,
                0.f,    0.f,   1.f, 0.f,
                -rightDotPosition, -upDotPosition, 1.f, 1.f
            }; 

            m_cachedPositionX = r_transform.position.x;
            m_cachedPositionY = r_transform.position.y;
            m_cachedOrientation = r_transform.orientation;
            hasTransformChanged = false;
        }


        void Camera::SetViewDimensions(float const width, float const height)
        {
            m_viewportWidth = width;
            m_viewportHeight = height;
            hasViewportChanged = true;
        }


        void Camera::SetMagnification(float const value)
        {
            // Clamping to 10 is arbitrary.
            m_magnification = glm::clamp(value, 0.1f, 10.f);
            hasTransformChanged = true;
        }


        void Camera::AdjustMagnification(float const delta)
        {
            SetMagnification(m_magnification + delta);
            hasTransformChanged = true;
        }

    } // End of Graphics namespace
} // End of PE namspace
