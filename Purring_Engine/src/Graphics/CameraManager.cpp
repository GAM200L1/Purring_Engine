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

        CameraManager::CameraManager(float const windowWidth, float const windowHeight)
        {
            // Update the editor camera viewport size
            m_editorCamera.SetViewDimensions(windowWidth, windowHeight);

            // Store the window size
            m_windowWidth = windowWidth, m_windowHeight = windowHeight;
        }


        std::optional<glm::mat4> CameraManager::GetWorldToNdcMatrix(bool const editorMode)
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

                return std::nullopt;
            }
        }


        std::optional<glm::mat4> CameraManager::GetViewToNdcMatrix(bool const editorMode)
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

                return std::nullopt;
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


        std::optional<glm::mat4> CameraManager::GetNdcToWorldMatrix(bool const editorMode)
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

                return std::nullopt;
            }
        }


        std::optional<glm::mat4> CameraManager::GetNdcToViewMatrix(bool const editorMode)
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

                return std::nullopt;
            }
        }


        std::optional<glm::mat4> CameraManager::GetViewToWorldMatrix(bool const editorMode)
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

                return std::nullopt;
            }
        }


        std::optional<std::reference_wrapper<Camera>> CameraManager::GetMainCamera()
        {
            // Check if the main camera ID stored is valid
            if (EntityManager::GetInstance().Has(m_mainCameraId, EntityManager::GetInstance().GetComponentID<Graphics::Camera>()))
            {
                return std::reference_wrapper<Camera>{EntityManager::GetInstance().Get<Camera>(m_mainCameraId)};
            }

            bool foundCamera{ false };
            // Check if there are any runtime cameras
            for (const EntityID& id : SceneView<Camera>())
            {
                // Skip the UI camera 
                if (id == m_uiCameraId) { continue; }

                // Return the first camera
                SetMainCamera(id);
                foundCamera = true;
            }

            if (foundCamera)
            {
                return std::reference_wrapper<Camera>{EntityManager::GetInstance().Get<Camera>(m_mainCameraId)};
            }

            return std::nullopt;
        }


        Camera& CameraManager::GetUiCamera()
        {
            // Check if the main camera ID stored is valid
            if (!EntityManager::GetInstance().Has(m_uiCameraId, EntityManager::GetInstance().GetComponentID<Graphics::Camera>()))
            {
                CreateUiCamera();
            }

            return EntityManager::GetInstance().Get<Camera>(m_uiCameraId);
            // @TODO    what if the camera ui obj exists but the camera component has been removed 
            //          for some reason? Delete the old obj first?

        }


        void CameraManager::SetMainCamera(EntityID const cameraEntityId)            
        {
            m_mainCameraId = cameraEntityId;
        }


        void CameraManager::CreateUiCamera()            
        {
            // Create a UI camera
            m_uiCameraId = EntityFactory::GetInstance().CreateFromPrefab("CameraObject");
            EntityManager::GetInstance().Get<Graphics::Camera>(m_uiCameraId).SetViewDimensions(m_windowWidth, m_windowHeight);
            // @TODO Name the gameobject or hide it in the editor
        }


        void CameraManager::InitializeSystem()
        {
            // Subscribe listeners to the events
            ADD_ALL_WINDOW_EVENT_LISTENER(CameraManager::OnWindowEvent, this)
            ADD_ALL_MOUSE_EVENT_LISTENER(CameraManager::OnMouseEvent, this)
            ADD_ALL_KEY_EVENT_LISTENER(CameraManager::OnKeyEvent, this)

            // Create a UI camera
            CreateUiCamera();
        }


        void CameraManager::UpdateSystem(float deltaTime)
        {
            deltaTime; // Prevent warnings

            // Update the editor camera
            m_editorCamera.UpdateCamera();


            // Check if the main camera ID stored is valid
            bool setNewMainCamera{ !EntityManager::GetInstance().Has(m_mainCameraId, EntityManager::GetInstance().GetComponentID<Graphics::Camera>()) };

            Transform& playerTransform{ EntityManager::GetInstance().Get<Transform>(1) };          

            unsigned i{};

            // Loop through all runtime cameras
            for (const EntityID& id : SceneView<Camera, Transform>())
            {
                // Make the first camera that exists the primary camera if a new main camera is needed
                if (setNewMainCamera && (id != m_uiCameraId))
                {
                    SetMainCamera(id);
                    setNewMainCamera = false;
                } 

                // Recompute the matrices of all the cameras
                Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) };

                if (!i) {
                    // @TODO - Remove in the future
                    // For testing, the first camera will follow the player's transform
                    r_transform.position.x = playerTransform.position.x;
                    r_transform.position.y = playerTransform.position.y;
                    r_transform.orientation = playerTransform.orientation;
                }

                Camera& r_camera{ EntityManager::GetInstance().Get<Camera>(id) };
                r_camera.UpdateCamera(r_transform);

                ++i; // @TODO - Remove in the future. Used to test swapping between cameras
            }

            // No runtime cameras exist so just set the id to zero
            if (setNewMainCamera)
            {
                SetMainCamera(0);
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
            // @TODO remove this after testing
            // Switch the main camera on pressing the MMB
            if (r_event.GetType() == MouseEvents::MouseButtonReleased)
            {
                MouseButtonReleaseEvent event = dynamic_cast<const MouseButtonReleaseEvent&>(r_event);

                if (event.button == 2) 
                {
                    for (const EntityID& id : SceneView<Camera, Transform>())
                    {
                        // Swap the main cameras
                        if ((m_mainCameraId != id) && (id != m_uiCameraId))
                        {
                            SetMainCamera(id);
                            break;
                        }
                    }
                }
                
            }

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
