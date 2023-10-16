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

namespace PE 
{
    namespace Graphics
    {
        // Define static editor camera variable
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
            if (EntityManager::GetInstance().Has(m_mainCameraId, "Camera")) 
            {
                return std::reference_wrapper<Camera>{EntityManager::GetInstance().Get<Camera>(m_mainCameraId)};
            }
            else 
            {
                // Check if there are any runtime cameras
                for (const EntityID& id : SceneView<Camera>())
                {
                    // Return the first camera
                    m_mainCameraId = id;
                    return std::reference_wrapper<Camera>{EntityManager::GetInstance().Get<Camera>(m_mainCameraId)};
                }

                return std::nullopt;
            }
        }


        void CameraManager::InitializeSystem()
        {
            /* Empty by design */
        }


        void CameraManager::UpdateSystem(float deltaTime)
        {
            deltaTime; // Prevent warnings

            // Check if the main camera ID stored is valid
            bool setNewMainCamera{ EntityManager::GetInstance().Has(m_mainCameraId, "Camera") };


            // Loop through all runtime cameras
            for (const EntityID& id : SceneView<Camera, Transform>())
            {
                // Make the first camera that exists the primary camera if a new main camera is needed
                if (setNewMainCamera) 
                { 
                    m_mainCameraId = id;
                    setNewMainCamera = false;
                } 

                // Recompute the matrices of all the cameras
                Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) };
                Camera& r_camera{ EntityManager::GetInstance().Get<Camera>(id) };
                r_camera.UpdateCamera(r_transform);
            }

            // No runtime cameras exist so just set the id to zero
            if (setNewMainCamera)
            {
                m_mainCameraId = 0;
            }
        }


        void CameraManager::DestroySystem()
        {
            /* Empty by design */
        }

    } // End of Graphics namespace
} // End of PE namspace
