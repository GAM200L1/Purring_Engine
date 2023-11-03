/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CameraManager.cpp
 \date     12-10-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the definition of the member functions of the class
           that manages the editor and runtime cameras.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "CameraManager.h"
#include "ECS/SceneView.h"
#include "Editor/Editor.h"

namespace PE 
{
    namespace Graphics
    {
        // Initialize static variables
        float CameraManager::m_windowWidth{}, CameraManager::m_windowHeight{};
        EntityID CameraManager::m_uiCameraId{};
        EditorCamera CameraManager::m_editorCamera{};


        CameraManager::CameraManager(float const windowWidth, float const windowHeight)
        {
            // Update the editor camera viewport size
            m_editorCamera.SetViewDimensions(windowWidth, windowHeight);

            // Store the window size
            m_windowWidth = windowWidth, m_windowHeight = windowHeight;
        }


        glm::mat4 CameraManager::GetWorldToNdcMatrix(bool const editorMode)
        {
            if (editorMode) 
            {
                // Return the editor camera matrix
                return m_editorCamera.GetWorldToNdcMatrix();
            }
            else 
            {
                // Return the main camera matrix
                std::optional<std::reference_wrapper<Camera>> mainCamera{ GetMainCamera()};
                if (mainCamera.has_value())
                {
                    return mainCamera.value().get().GetWorldToNdcMatrix();
                }

                return glm::mat4{ 0 };
            }
        }


        glm::mat4 CameraManager::GetViewToNdcMatrix(bool const editorMode)
        {
            if (editorMode)
            {
                // Return the editor camera matrix
                return m_editorCamera.GetViewToNdcMatrix();
            }
            else
            {
                // Return the main camera matrix
                std::optional<std::reference_wrapper<Camera>> mainCamera{ GetMainCamera() };
                if (mainCamera.has_value())
                {
                    return mainCamera.value().get().GetViewToNdcMatrix();
                }

                return glm::mat4{ 0 };
            }
        }


        glm::mat4 CameraManager::GetUiViewToNdcMatrix()
        {
            return GetUiCamera().GetViewToNdcMatrix();
        }
        


        glm::mat4 CameraManager::GetUiNdcToViewMatrix()
        {
            return GetUiCamera().GetNdcToViewMatrix();
        }


        glm::mat4 CameraManager::GetNdcToWorldMatrix(bool const editorMode)
        {
            if (editorMode)
            {
                // Return the editor camera matrix
                return m_editorCamera.GetNdcToWorldMatrix();
            }
            else
            {
                // Return the main camera matrix
                std::optional<std::reference_wrapper<Camera>> mainCamera{ GetMainCamera() };
                if (mainCamera.has_value())
                {
                    return mainCamera.value().get().GetNdcToWorldMatrix();
                }

                return glm::mat4{ 0 };
            }
        }


        glm::mat4 CameraManager::GetNdcToViewMatrix(bool const editorMode)
        {
            if (editorMode)
            {
                // Return the editor camera matrix
                return m_editorCamera.GetNdcToViewMatrix();
            }
            else
            {
                // Return the main camera matrix
                std::optional<std::reference_wrapper<Camera>> mainCamera{ GetMainCamera() };
                if (mainCamera.has_value())
                {
                    return mainCamera.value().get().GetNdcToViewMatrix();
                }

                return glm::mat4{ 0 };
            }
        }


        glm::mat4 CameraManager::GetViewToWorldMatrix(bool const editorMode)
        {
            if (editorMode)
            {
                // Return the editor camera matrix
                return m_editorCamera.GetViewToWorldMatrix();
            }
            else
            {
                // Return the main camera matrix
                std::optional<std::reference_wrapper<Camera>> mainCamera{ GetMainCamera() };
                if (mainCamera.has_value())
                {
                    return mainCamera.value().get().GetViewToWorldMatrix();
                }

                return glm::mat4{ 0 };
            }
        }


        std::optional<std::reference_wrapper<Camera>> CameraManager::GetMainCamera()
        {
            // Check if the main camera ID stored is valid
            if (EntityManager::GetInstance().Has(m_mainCameraId, EntityManager::GetInstance().GetComponentID<Graphics::Camera>()))
            {
                return std::reference_wrapper<Camera>{EntityManager::GetInstance().Get<Camera>(m_mainCameraId)};
            }

            return std::nullopt;
        }


        Camera& CameraManager::GetUiCamera()
        {
            return EntityManager::GetInstance().Get<Camera>(m_uiCameraId);
        }


        bool CameraManager::SetMainCamera(EntityID const cameraEntityId)            
        {
            // Don't set it if it's the UI camera, or if it doesn't have a camera component on it
            if (cameraEntityId == m_uiCameraId || !EntityManager::GetInstance().Has(cameraEntityId, EntityManager::GetInstance().GetComponentID<Graphics::Camera>()))
            {
                return false;
            }

            m_mainCameraId = cameraEntityId;
            return true;
        }


        void CameraManager::SetUiCamera(EntityID const cameraEntityId)
        {
            m_uiCameraId = cameraEntityId;
        }


        void CameraManager::InitializeSystem()
        {
            // Subscribe listeners to the events
            ADD_ALL_WINDOW_EVENT_LISTENER(CameraManager::OnWindowEvent, this)
            ADD_ALL_MOUSE_EVENT_LISTENER(CameraManager::OnMouseEvent, this)
            ADD_ALL_KEY_EVENT_LISTENER(CameraManager::OnKeyEvent, this)
        }


        void CameraManager::UpdateSystem(float deltaTime)
        {
            deltaTime; // Prevent warnings

            // Update the editor camera
            m_editorCamera.UpdateCamera();


            // Check if the main camera ID stored is valid
            bool setNewMainCamera{ !EntityManager::GetInstance().Has(m_mainCameraId, EntityManager::GetInstance().GetComponentID<Graphics::Camera>()) };

            // Loop through all runtime cameras
            for (const EntityID& id : SceneView<Camera, Transform>())
            {
                // Recompute the matrices of all the cameras
                Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) };
                Camera& r_camera{ EntityManager::GetInstance().Get<Camera>(id) };

                // If this camera has been set as the main camera in the last frame
                // or this camera is the first valid runtime camera available 
                if ((r_camera.GetMainCameraStatusChanged() && r_camera.GetIsMainCamera()) 
                    || (setNewMainCamera && (id != m_uiCameraId)))
                {
                    SetMainCamera(id);
                    setNewMainCamera = false;
                }

                r_camera.UpdateCamera(r_transform, m_mainCameraId == id);
            }

            // No runtime cameras exist so just set the id to the default ID
            if (setNewMainCamera)
            {
                SetMainCamera(defaultId);
            }
        }


        void CameraManager::DestroySystem()
        {
            /* Empty by design */
        }


        void CameraManager::OnWindowEvent(const PE::Event<PE::WindowEvents>& r_event)
        {
            if (r_event.GetType() == WindowEvents::WindowResize)
            {
                WindowResizeEvent event;
                event = dynamic_cast<const WindowResizeEvent&>(r_event);

                // Store the window size
                m_windowWidth = static_cast<float>(event.width);
                m_windowHeight = static_cast<float>(event.height);

                // Update the size of the viewport of all the cameras
                for (const EntityID& id : SceneView<Camera>())
                {
                    Camera& r_camera{ EntityManager::GetInstance().Get<Camera>(id) };
                    r_camera.SetViewDimensions(m_windowWidth, m_windowHeight);
                }
            }
        }


        void CameraManager::OnMouseEvent(const PE::Event<PE::MouseEvents>& r_event)
        {
            // Zoom the editor camera in and out on mouse scroll
            if (Editor::GetInstance().IsMouseInScene() && r_event.GetType() == MouseEvents::MouseScrolled)
            {
                MouseScrolledEvent event;
                event = dynamic_cast<const MouseScrolledEvent&>(r_event);
                GetEditorCamera().AdjustMagnification(-event.yOffset * 0.5f);

            }
        }


        void CameraManager::OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event)
        {
            if (r_event.GetType() == KeyEvents::KeyPressed)
            {
                KeyPressedEvent event;
                event = dynamic_cast<const KeyPressedEvent&>(r_event);

                // Move the editor camera
                if (event.keycode == GLFW_KEY_UP)
                {
                    GetEditorCamera().AdjustPosition(0.f, 10.f);
                }
                if (event.keycode == GLFW_KEY_DOWN)
                {
                    GetEditorCamera().AdjustPosition(0.f, -10.f);
                }
                if (event.keycode == GLFW_KEY_LEFT)
                {
                    GetEditorCamera().AdjustPosition(-10.f, 0.f);
                }
                if (event.keycode == GLFW_KEY_RIGHT)
                {
                    GetEditorCamera().AdjustPosition(10.f, 0.f);
                }

                // Rotate the editor camera
                if (event.keycode == GLFW_KEY_COMMA)
                {
                    GetEditorCamera().AdjustRotationRadians(0.1f);
                }
                if (event.keycode == GLFW_KEY_PERIOD)
                {
                    GetEditorCamera().AdjustRotationRadians(-0.1f);
                }

                // Zoom the main runtime camera in and out
                if (event.keycode == GLFW_KEY_C)
                {
                    std::optional<std::reference_wrapper<Camera>> mainCamera{ GetMainCamera() };
                    if (mainCamera) {
                        mainCamera.value().get().AdjustMagnification(0.5f);
                    }
                }
                if (event.keycode == GLFW_KEY_V)
                {
                    std::optional<std::reference_wrapper<Camera>> mainCamera{ GetMainCamera() };
                    if (mainCamera) {
                        mainCamera.value().get().AdjustMagnification(-0.5f);
                    }
                }
            }            
        }

    } // End of Graphics namespace
} // End of PE namspace
