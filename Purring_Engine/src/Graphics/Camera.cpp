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

#include "prpch.h"
#include "Camera.h"

namespace PE 
{
    namespace Graphics
    {
        void Camera::UpdateCamera(Transform const& r_transform)
        {
            if (GetHasChanged(r_transform) || hasViewportChanged)
            {
                ComputeViewMatrix(r_transform);
                ComputeNDCMatrix();
                m_cachedWorldToNdcMatrix = GetViewToNdcMatrix() * GetWorldToViewMatrix();
                m_cachedNdcToWorldMatrix = GetViewToWorldMatrix() * GetNdcToViewMatrix();
            }
        }


        void Camera::ComputeNDCMatrix()
        {
            float halfWidth{ m_viewportWidth * 0.5f };
            float halfHeight{ m_viewportHeight * 0.5f };

            m_cachedViewToNdcMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -10.f, 10.f);
            m_cachedNdcToViewMatrix = glm::inverse(m_cachedViewToNdcMatrix);
            hasViewportChanged = false;
        }


        void Camera::ComputeViewMatrix(Transform const& r_transform)
        {
            ComputeViewMatrix(r_transform.position.x, r_transform.position.y, r_transform.orientation, m_magnification);
        }
        

        void Camera::ComputeViewMatrix(float positionX, float positionY, float orientation, float magnification)
        {
            glm::vec2 up{ GetUpVector(orientation) };
            glm::vec2 right{ up.y, -up.x };

            m_cachedViewToWorldMatrix = glm::mat4{
                right.x, right.y, 0.f, 0.f,
                up.x, up.y, 0.f, 0.f,
                0.f,    0.f,   1.f, 0.f,
                positionX, positionY, 0.f, 1.f
            };

            float scaleReciprocal{ 1.f / magnification };
            up *= scaleReciprocal, right *= scaleReciprocal;

            float upDotPosition{ up.x * positionX + up.y * positionY };
            float rightDotPosition{ right.x * positionX + right.y * positionY };

            // Update the cached values
            m_cachedWorldToViewMatrix = glm::mat4{
                right.x, up.x, 0.f, 0.f,
                right.y, up.y, 0.f, 0.f,
                0.f,    0.f,   1.f, 0.f,
                -rightDotPosition, -upDotPosition, 0.f, 1.f
            };

            m_cachedPositionX = positionX;
            m_cachedPositionY = positionY;
            m_cachedOrientation = orientation;
            hasTransformChanged = false;
        }


        void Camera::SetViewDimensions(float const width, float const height)
        {
            if (width != m_viewportWidth || height != m_viewportHeight)
            {
                m_viewportWidth = width;
                m_viewportHeight = height;
                hasViewportChanged = true;
            }
        }


        void Camera::SetMagnification(float value)
        {
            // Clamping to 10 is arbitrary.
            value = glm::clamp(value, 0.1f, 10.f);
            if (value != m_magnification)
            {
                m_magnification = value;
                hasTransformChanged = true;
            }
        }


        void Camera::AdjustMagnification(float const delta)
        {
            SetMagnification(m_magnification + delta);
        }

    } // End of Graphics namespace
} // End of PE namspace
