/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EditorCamera.cpp
 \date     12-10-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the definition for the member functions of 
           the editor camera class.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "EditorCamera.h"
#include <glm/gtc/matrix_transform.hpp> // ortho()

namespace PE 
{
    namespace Graphics
    {
        glm::mat4 EditorCamera::GetWorldToViewMatrix()
        {
            if (GetHasChanged())
            {
                ComputeViewMatrix();
            }

            return m_cachedViewMatrix;
        }

        
        glm::mat4 EditorCamera::GetViewToNdcMatrix() const
        {
            float halfWidth{ m_viewportWidth * 0.5f };
            float halfHeight{ m_viewportHeight * 0.5f };

            return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -m_magnification, m_magnification);
        }


        glm::mat4 EditorCamera::GetWorldToNdcMatrix()
        {
            if (GetHasChanged() || hasViewportChanged)
            {
                m_cachedWorldToNdcMatrix = GetWorldToViewMatrix() * GetViewToNdcMatrix();
                hasViewportChanged = false;
            }

            return m_cachedWorldToNdcMatrix;
        }


        bool EditorCamera::GetHasChanged() const
        {
            return hasTransformChanged;
        }


        void EditorCamera::ComputeViewMatrix()
        {
            float scaleReciprocal{ 1.f / m_magnification };

            glm::vec2 up{ -glm::sin(m_orientation), glm::cos(m_orientation) };
            up *= scaleReciprocal;

            glm::vec2 right{ up.y, -up.x };

            float upDotPosition{ (up.x * m_position.x + up.y * m_position.y) };
            float rightDotPosition{ (right.x * m_position.x + right.y * m_position.y) };


            // Update the cached values
            m_cachedViewMatrix = glm::mat4{
                right.x, up.x, 0.f, 0.f,
                right.y, up.y, 0.f, 0.f,
                0.f,    0.f,   1.f, 0.f,
                -rightDotPosition, -upDotPosition, 1.f, 1.f
            };

            hasTransformChanged = false;
        }


        void EditorCamera::SetViewDimensions(float const width, float const height)
        {
            m_viewportWidth = width;
            m_viewportHeight = height;
            hasViewportChanged = true;
        }


        void EditorCamera::SetMagnification(float const value)
        {
            // Clamping to 10 is arbitrary.
            m_magnification = glm::clamp(value, 0.1f, 10.f);
            hasTransformChanged = true;
        }


        void EditorCamera::SetPosition(float const valueX, float const valueY)
        {
            m_position.x = valueX;
            m_position.y = valueY;
            hasTransformChanged = true;
        }


        void EditorCamera::SetRotationDegrees(float const degrees)
        {
            float angleRadians{ glm::radians(degrees) };
            m_orientation = angleRadians;
            hasTransformChanged = true;
        }


        void EditorCamera::SetRotationRadians(float const radians)
        {
            m_orientation = radians;
            hasTransformChanged = true;
        }


        void EditorCamera::AdjustMagnification(float const delta)
        {
            SetMagnification(m_magnification + delta);
            hasTransformChanged = true;
        }

        
        void EditorCamera::AdjustPosition(float const deltaX, float const deltaY)
        {
            m_position.x += deltaX;
            m_position.y += deltaY;
            hasTransformChanged = true;
        }


        void EditorCamera::AdjustRotationDegrees(float const delta)
        {
            float angleRadians{ glm::radians(delta) };
            m_orientation += angleRadians;
            hasTransformChanged = true;
        }


        void EditorCamera::AdjustRotationRadians(float const delta)
        {
            m_orientation += delta;
            hasTransformChanged = true;
        }
    } // End of Graphics namespace
} // End of PE namspace
