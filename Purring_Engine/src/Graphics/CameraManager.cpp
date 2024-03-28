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
#include "Layers/LayerManager.h"
#include "RendererManager.h"

#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE

namespace PE 
{
    namespace Graphics
    {
        // Initialize static variables
        float CameraManager::m_windowWidth{}, CameraManager::m_windowHeight{};
        EntityID CameraManager::m_uiCameraId{};
        EditorCamera CameraManager::m_editorCamera{};


        CameraManager::CameraManager(float const windowWidth, float const windowHeight)
            : m_viewportWidth{ windowWidth }, m_viewportHeight{ windowHeight }
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


        glm::mat4 CameraManager::GetWorldToViewMatrix(bool const editorMode)
        {
            if (editorMode)
            {
                // Return the editor camera matrix
                return m_editorCamera.GetWorldToViewMatrix();
            }
            else
            {
                // Return the main camera matrix
                std::optional<std::reference_wrapper<Camera>> mainCamera{ GetMainCamera() };
                if (mainCamera.has_value())
                {
                    return mainCamera.value().get().GetWorldToViewMatrix();
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


        std::optional<std::reference_wrapper<Camera>> CameraManager::GetMainCamera() const
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


        vec2 CameraManager::GetWorldToWindowPosition(float const x, float const y) const
        {
            // Get the main camera
            std::optional<std::reference_wrapper<Camera>> ref_mainCamera{ GetMainCamera() };

            if (ref_mainCamera.has_value())
            {
                Camera& r_mainCamera = ref_mainCamera.value().get();
                
#ifndef GAMERELEASE
                // Get the size of the render window
                float editorWindowSizeX{}, editorWindowSizeY{};
                Editor::GetInstance().GetWindowSize(editorWindowSizeX, editorWindowSizeY);
                float widthRatio{ !CompareFloats(m_viewportWidth, 0.f) ? editorWindowSizeX / m_viewportWidth : 1.f };
                float heightRatio{ !CompareFloats(m_viewportHeight, 0.f) ? editorWindowSizeY / m_viewportHeight : 1.f };

                // Adjust scale of the position
                vec2 ret{ r_mainCamera.GetWorldToViewportPosition(x, y - Editor::GetInstance().GetPlayWindowOffset()) };
                ret.x *= widthRatio, ret.y *= heightRatio;
#else
                float widthRatio{ !CompareFloats(m_viewportWidth, 0.f) ? m_windowWidth / m_viewportWidth : 1.f };
                float heightRatio{ !CompareFloats(m_viewportHeight, 0.f) ? m_windowHeight / m_viewportHeight : 1.f };
                vec2 ret{ r_mainCamera.GetWorldToViewportPosition(x, y) };
                ret.x *= widthRatio, ret.y *= heightRatio;
#endif // !GAMERELEASE

                return ret;
            }
            else 
            {
                return vec2{x, y};
            }
        }


        vec2 CameraManager::GetWindowToWorldPosition(float const x, float const y) const
        {
            // Get the main camera
            std::optional<std::reference_wrapper<Camera>> ref_mainCamera{ GetMainCamera() };

            if (ref_mainCamera.has_value())
            {
                Camera& r_mainCamera = ref_mainCamera.value().get();
                
#ifndef GAMERELEASE
                // Get the size of the render window
                float editorWindowSizeX{}, editorWindowSizeY{};
                Editor::GetInstance().GetWindowSize(editorWindowSizeX, editorWindowSizeY);
                float widthRatio{ !CompareFloats(editorWindowSizeX, 0.f) ? m_viewportWidth / editorWindowSizeX : 1.f };
                float heightRatio{ !CompareFloats(editorWindowSizeY, 0.f) ? m_viewportHeight / editorWindowSizeY : 1.f };

                // Adjust scale of the position
                vec2 ret{ r_mainCamera.GetViewportToWorldPosition(x * widthRatio, y * heightRatio) };
                ret.y += Editor::GetInstance().GetPlayWindowOffset();
#else
                float widthRatio{ !CompareFloats(m_windowWidth, 0.f) ? m_viewportWidth / m_windowWidth : 1.f };
                float heightRatio{ !CompareFloats(m_windowHeight, 0.f) ? m_viewportHeight / m_windowHeight : 1.f };
                vec2 ret{ r_mainCamera.GetViewportToWorldPosition(x * widthRatio, y * heightRatio) };
#endif // !GAMERELEASE

                return ret;
            }
            else 
            {
                return vec2{x, y};
            }
        }


        vec2 CameraManager::GetUiWindowToScreenPosition(float const x, float const y)
        {
            // Get the UI camera
#ifndef GAMERELEASE
                // Get the size of the render window
            float editorWindowSizeX{}, editorWindowSizeY{};
            Editor::GetInstance().GetWindowSize(editorWindowSizeX, editorWindowSizeY);
            float widthRatio{ !CompareFloats(editorWindowSizeX, 0.f) ? GetUiCamera().GetViewportWidth() / editorWindowSizeX : 1.f };
            float heightRatio{ !CompareFloats(editorWindowSizeY, 0.f) ? GetUiCamera().GetViewportHeight() / editorWindowSizeY : 1.f };

            // Adjust scale of the position
            vec2 ret{ x * widthRatio, y * heightRatio };
            ret.y += Editor::GetInstance().GetPlayWindowOffset();
#else
            float widthRatio{ !CompareFloats(m_windowWidth, 0.f) ? GetUiCamera().GetViewportWidth() / m_windowWidth : 1.f };
            float heightRatio{ !CompareFloats(m_windowHeight, 0.f) ? GetUiCamera().GetViewportHeight() / m_windowHeight : 1.f };
            vec2 ret{ x * widthRatio, y * heightRatio };
#endif // !GAMERELEASE

            return ret;
        }



        bool CameraManager::SetMainCamera(EntityID const cameraEntityId)            
        {
            // Don't set it if it's the UI camera, or if it doesn't have a camera component on it
            if (cameraEntityId == m_uiCameraId || !EntityManager::GetInstance().Has(cameraEntityId, EntityManager::GetInstance().GetComponentID<Graphics::Camera>()))
            {
                return false;
            }

            m_mainCameraId = cameraEntityId;

            Camera const& r_mainCamera{ EntityManager::GetInstance().Get<Camera>(m_mainCameraId) };
            m_viewportWidth = r_mainCamera.GetViewportWidth();
            m_viewportHeight = r_mainCamera.GetViewportHeight();

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

#ifndef GAMERELEASE
            // Update the editor camera
            m_editorCamera.UpdateCamera();
#endif // !GAMERELEASE

            // Check if the main camera ID stored is valid
            bool setNewMainCamera{ !EntityManager::GetInstance().Has(m_mainCameraId, EntityManager::GetInstance().GetComponentID<Graphics::Camera>()) };

            // Loop through all runtime cameras
            for (const auto& layer : LayerView<Camera, Transform>())
            {
                for (const EntityID& id : InternalView(layer))
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

                    // Update the cached viewport dimensions
                    if (m_mainCameraId == id) {
                        if (m_viewportWidth != r_camera.GetViewportWidth())
                        {
                            m_viewportWidth = r_camera.GetViewportWidth();
                        }

                        if (m_viewportHeight != r_camera.GetViewportHeight())
                        {
                            m_viewportHeight = r_camera.GetViewportHeight();
                        }

                        // set the background color of the renderer manager to the main camera's background color
                        const_cast<Graphics::RendererManager*>(GETRENDERERMANAGER())->SetBackgroundColor(r_camera.GetBackgroundColor().r,
                                                                                                         r_camera.GetBackgroundColor().g,
                                                                                                         r_camera.GetBackgroundColor().b,
                                                                                                         r_camera.GetBackgroundColor().a);
                    }
                }
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

                // Store the ratio of the main camera's viewport to the actual window size
                std::optional<std::reference_wrapper<Camera>> mainCamera{ GetMainCamera() };
                if (mainCamera) {
                    m_viewportWidth = mainCamera.value().get().GetViewportWidth();
                    m_viewportHeight = mainCamera.value().get().GetViewportHeight();
                } 
            }
        }

        void CameraManager::OnMouseEvent(const PE::Event<PE::MouseEvents>& r_event)
        {
            r_event; // remove if used
#ifndef GAMERELEASE
            // Zoom the editor camera in and out on mouse scroll
            if (Editor::GetInstance().IsMouseInScene() && r_event.GetType() == MouseEvents::MouseScrolled)
            {
                MouseScrolledEvent event;
                event = dynamic_cast<const MouseScrolledEvent&>(r_event);
                if(Editor::GetInstance().IsEditorActive())
                GetEditorCamera().AdjustMagnification(-event.yOffset * 0.5f);
            }
#endif // !GAMERELEASE
        }



        void CameraManager::OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event)
        {
			r_event;
#ifndef GAMERELEASE
            //if not in scene view dont edit camera
            if (!Editor::GetInstance().IsSceneViewFocused())
                return;

            if (r_event.GetType() == KeyEvents::KeyTriggered)
            {
                KeyTriggeredEvent event;
                event = dynamic_cast<const KeyTriggeredEvent&>(r_event);


                // Move the editor camera
                if (event.keycode == GLFW_KEY_UP)
                {
                    GetEditorCamera().AdjustLocalPosition(0.f, 10.f);
                }
                if (event.keycode == GLFW_KEY_DOWN)
                {
                    GetEditorCamera().AdjustLocalPosition(0.f, -10.f);
                }
                if (event.keycode == GLFW_KEY_LEFT)
                {
                    GetEditorCamera().AdjustLocalPosition(-10.f, 0.f);
                }
                if (event.keycode == GLFW_KEY_RIGHT)
                {
                    GetEditorCamera().AdjustLocalPosition(10.f, 0.f);
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
            }

            if (r_event.GetType() == KeyEvents::KeyPressed)
            {
                KeyPressedEvent event;
                event = dynamic_cast<const KeyPressedEvent&>(r_event);
                // Move the editor camera
                if (event.keycode == GLFW_KEY_UP)
                {
                    GetEditorCamera().AdjustLocalPosition(0.f, 10.f);
                }
                if (event.keycode == GLFW_KEY_DOWN)
                {
                    GetEditorCamera().AdjustLocalPosition(0.f, -10.f);
                }
                if (event.keycode == GLFW_KEY_LEFT)
                {
                    GetEditorCamera().AdjustLocalPosition(-10.f, 0.f);
                }
                if (event.keycode == GLFW_KEY_RIGHT)
                {
                    GetEditorCamera().AdjustLocalPosition(10.f, 0.f);
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

            }   
#endif // !GAMERELEASE
        }

    } // End of Graphics namespace
} // End of PE namspace
