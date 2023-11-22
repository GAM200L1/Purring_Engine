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
        void EditorCamera::SetPosition(float const valueX, float const valueY)
        {
            if (valueX != m_position.x || valueY != m_position.y)
            {
                m_position.x = valueX;
                m_position.y = valueY;
                hasTransformChanged = true;
            }
        }


        void EditorCamera::SetRotationDegrees(float const degrees)
        {
            float angleRadians{ glm::radians(degrees) };
            if (angleRadians != m_orientation)
            {
                m_orientation = angleRadians;
                hasTransformChanged = true;
            }
        }


        void EditorCamera::SetRotationRadians(float const radians)
        {
            if (radians != m_orientation)
            {
                m_orientation = radians;
                hasTransformChanged = true;
            }
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


        void EditorCamera::UpdateCamera()
        {
            // Recompute matrices if its properties have changed
            if (GetHasChanged() || hasViewportChanged)
            {
                ComputeViewMatrix(m_position.x, m_position.y, m_orientation, m_magnification);
                ComputeNDCMatrix();
                m_cachedWorldToNdcMatrix = GetViewToNdcMatrix() * GetWorldToViewMatrix();
                m_cachedNdcToWorldMatrix = GetViewToWorldMatrix() * GetNdcToViewMatrix();
            }
        }
    } // End of Graphics namespace
} // End of PE namspace
