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
#include "Input/InputSystem.h"
#include "RendererManager.h"

namespace PE 
{
    namespace Graphics
    {
        void Camera::UpdateCamera(Transform const& r_transform, bool const isMainCamera)
        {
            if (GetHasChanged(r_transform) || hasViewportChanged)
            {
                ComputeViewMatrix(r_transform);
                ComputeNDCMatrix();
                m_cachedWorldToNdcMatrix = GetViewToNdcMatrix() * GetWorldToViewMatrix();
                m_cachedNdcToWorldMatrix = GetViewToWorldMatrix() * GetNdcToViewMatrix();
            }

            m_isMainCamera = isMainCamera;
            m_cachedIsMainCamera = m_isMainCamera;
        }

        void Camera::SetBackgroundColor(float const r, float const g, float const b, float const a)
        {
            m_backgroundColor.r = glm::clamp(r, 0.f, 1.f);
            m_backgroundColor.g = glm::clamp(g, 0.f, 1.f);
            m_backgroundColor.b = glm::clamp(b, 0.f, 1.f);
            m_backgroundColor.a = glm::clamp(a, 0.f, 1.f);
        }


        void Camera::ComputeNDCMatrix()
        {
            float const orthoNear{ -10.f }, orthoFar{ 10.f };
            float const subFarNear{ orthoFar - orthoNear };

            m_cachedViewToNdcMatrix = glm::mat4 {
                2.f / m_viewportWidth, 0.f, 0.f, 0.f,
                0.f, 2.f / m_viewportHeight, 0.f, 0.f,
                0.f, 0.f, -2.f / subFarNear, 0.f,
                0.f, 0.f, 0.f, 1.f // assumption: view frustrum is centered
            };

            m_cachedNdcToViewMatrix = glm::mat4 {
                m_viewportWidth * 0.5f, 0.f, 0.f, 0.f,
                0.f, m_viewportHeight * 0.5f, 0.f, 0.f,
                0.f, 0.f, subFarNear * -0.5f, 0.f,
                0.f, 0.f, 0.f, 1.f // assumption: view frustrum is centered
            };

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
                right.x * magnification, right.y * magnification, 0.f, 0.f,
                up.x * magnification, up.y * magnification, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
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


        vec2 Camera::GetViewportToWorldPosition(float const x, float const y) const
        {
            return vec2{
                (x * m_cachedViewToWorldMatrix[0][0] + m_cachedViewToWorldMatrix[3][0]),
                (y * m_cachedViewToWorldMatrix[1][1] + m_cachedViewToWorldMatrix[3][1])
            };
        }


        vec2 Camera::GetWorldToViewportPosition(float const x, float const y) const
        {
            return vec2{
                (x * m_cachedWorldToViewMatrix[0][0] + m_cachedWorldToViewMatrix[3][0]),
                (y * m_cachedWorldToViewMatrix[1][1] + m_cachedWorldToViewMatrix[3][1])
            };
        }


        bool Camera::GetPositionWithinViewport(float const x, float const y, float const padding) const
        {
            // Transform the coordinates into viewport space 
            float viewX{ x * m_cachedWorldToViewMatrix[0][0] + y * m_cachedWorldToViewMatrix[1][0] + m_cachedWorldToViewMatrix[3][0] };
            float viewY{ x * m_cachedWorldToViewMatrix[0][1] + y * m_cachedWorldToViewMatrix[1][1] + m_cachedWorldToViewMatrix[3][1] };

            // Check if the coordinates are within the viewport
            float halfWidth{ (m_viewportWidth - padding) * 0.5f }, halfHeight{ (m_viewportHeight - padding) * 0.5f };
            return (viewX >= -halfWidth && viewX <= halfWidth) && (viewY >= -halfHeight && viewY <= halfHeight);
        }


        void Camera::SetMainCamera(bool const value)
        {
            m_isMainCamera = value;
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

        nlohmann::json Camera::ToJson(size_t id) const
        {
            UNREFERENCED_PARAMETER(id);

            nlohmann::json j;
            j["magnification"] = m_magnification;
            j["viewportWidth"] = m_viewportWidth;
            j["viewportHeight"] = m_viewportHeight;
            j["ismaincamera"] = m_isMainCamera;
            j["backgroundColor"]["r"] = m_backgroundColor.r;
            j["backgroundColor"]["g"] = m_backgroundColor.g;
            j["backgroundColor"]["b"] = m_backgroundColor.b;
            j["backgroundColor"]["a"] = m_backgroundColor.a;
            return j;
        }

        Camera Camera::Deserialize(const nlohmann::json& j)
        {
            Camera cam;
            cam.m_magnification = j["magnification"];
            cam.m_viewportWidth = j["viewportWidth"];
            cam.m_viewportHeight = j["viewportHeight"];
            if (j.contains("ismaincamera"))
                cam.m_isMainCamera = j["ismaincamera"];
            if (j.contains("backgroundColor"))
            {
				cam.m_backgroundColor.r = j["backgroundColor"]["r"];
				cam.m_backgroundColor.g = j["backgroundColor"]["g"];
				cam.m_backgroundColor.b = j["backgroundColor"]["b"];
				cam.m_backgroundColor.a = j["backgroundColor"]["a"];
			}
            return cam;
        }


    } // End of Graphics namespace
} // End of PE namspace
