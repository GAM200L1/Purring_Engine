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
        // Define static editor camera variables
        EditorCamera CameraManager::m_editorCamera{};

        CameraManager::CameraManager(float const editorViewWidth, float const editorViewHeight)
        {
            // Update the editor camera viewport size
            m_editorCamera.SetViewDimensions(editorViewWidth, editorViewHeight);
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


        void CameraManager::SetMainCamera(EntityID const cameraEntityId)            
        {
            m_mainCameraId = cameraEntityId;
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

            // Check if the main camera ID stored is valid
            bool setNewMainCamera{ !EntityManager::GetInstance().Has(m_mainCameraId, EntityManager::GetInstance().GetComponentID<Graphics::Camera>()) };

            Transform& playerTransform{ EntityManager::GetInstance().Get<Transform>(1) };          

            unsigned i{};

            // Loop through all runtime cameras
            for (const EntityID& id : SceneView<Camera, Transform>())
            {
                // Make the first camera that exists the primary camera if a new main camera is needed
                if (setNewMainCamera) 
                {
                    SetMainCamera(id);
                    setNewMainCamera = false;
                } 

                // Recompute the matrices of all the cameras
                Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) };

                //if (!i) {
                //    // @TODO - Remove in the future
                //    // For testing, the first camera will follow the player's transform
                //    r_transform.position.x = playerTransform.position.x;
                //    r_transform.position.y = playerTransform.position.y;
                //    r_transform.orientation = playerTransform.orientation;
                //}

                Camera& r_camera{ EntityManager::GetInstance().Get<Camera>(id) };
                r_camera.UpdateCamera(r_transform);

                ++i; // @TODO - Remove in the future
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

                for (const EntityID& id : SceneView<Camera>())
                {
                    // Update the size of the viewport of the camera
                    Camera& r_camera{ EntityManager::GetInstance().Get<Camera>(id) };
                    r_camera.SetViewDimensions(static_cast<float>(event.width), static_cast<float>(event.height));
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
                        if (m_mainCameraId != id)
                        {
                            SetMainCamera(id);
                            break;
                        }
                    }
                }
                
            }

            // Zoom the editor camera in and out on mouse scroll
            if (r_event.GetType() == MouseEvents::MouseScrolled)
            {
                MouseScrolledEvent event;
                event = dynamic_cast<const MouseScrolledEvent&>(r_event);
                Graphics::CameraManager::GetEditorCamera().AdjustMagnification(-event.yOffset * 0.5f);

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
                    m_editorCamera.AdjustPosition(0.f, 10.f);
                }
                if (event.keycode == GLFW_KEY_DOWN)
                {
                    m_editorCamera.AdjustPosition(0.f, -10.f);
                }
                if (event.keycode == GLFW_KEY_LEFT)
                {
                    m_editorCamera.AdjustPosition(-10.f, 0.f);
                }
                if (event.keycode == GLFW_KEY_RIGHT)
                {
                    m_editorCamera.AdjustPosition(10.f, 0.f);
                }

                // Rotate the editor camera
                if (event.keycode == GLFW_KEY_COMMA)
                {
                    m_editorCamera.AdjustRotationRadians(0.1f);
                }
                if (event.keycode == GLFW_KEY_PERIOD)
                {
                    m_editorCamera.AdjustRotationRadians(-0.1f);
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
