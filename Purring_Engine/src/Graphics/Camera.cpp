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
                m_cachedWorldToNdcMatrix = GetViewToNdcMatrix() * GetWorldToViewMatrix();
                hasViewportChanged = false;
            }
        }


        void Camera::ComputeViewMatrix(Transform const& r_transform)
        {
            float scaleReciprocal{ 1.f / m_magnification };
            glm::vec2 up{ GetUpVector(r_transform.orientation) };
            up *= scaleReciprocal;
            glm::vec2 right{ up.y, -up.x };

            float upDotPosition{ up.x * r_transform.position.x + up.y * r_transform.position.y };
            float rightDotPosition{ right.x * r_transform.position.x + right.y * r_transform.position.y };

            // Update the cached values
            m_cachedViewMatrix = glm::mat4{
                right.x, up.x, 0.f, 0.f,
                right.y, up.y, 0.f, 0.f,
                0.f,    0.f,   1.f, 0.f,
                -rightDotPosition, -upDotPosition, 0.f, 1.f
            };

            m_cachedPositionX = r_transform.position.x;
            m_cachedPositionY = r_transform.position.y;
            m_cachedOrientation = r_transform.orientation;
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
