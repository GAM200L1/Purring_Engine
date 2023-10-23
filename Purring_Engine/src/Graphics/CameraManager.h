#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CameraManager.h
 \date     12-10-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the class that manages the editor and runtime cameras.
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include <glm/glm.hpp>
#include <optional>

#include "Camera.h"
#include "EditorCamera.h"

#include "Events/EventHandler.h"
#include "ECS/Entity.h"
#include "System.h"

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
        \brief  Manages the editor and runtime cameras.
        *************************************************************************************/
        class CameraManager : public System
        {
            // ----- Constructors ----- //
        public:
            /*!***********************************************************************************
             \brief Initializes the camera manager.

             \param[in] editorViewWidth Viewport width of the editor camera.
             \param[in] editorViewHeight Viewport height of the editor camera.
            *************************************************************************************/
            CameraManager(float const editorViewWidth, float const editorViewHeight);

            // ----- Public getters ----- // 
        public:

            /*!***********************************************************************************
             \brief Returns an optional object initalized with the 4x4 world to NDC matrix 
                    the editor camera or the main runtime camera.

             \param[in] editorMode Set to true to get the 4x4 world to NDC matrix of 
                            the editor camera, false to get that of the main runtime camera.

             \return std::optional<glm::mat4 const&> - Returns an optional object initalized 
                        with the 4x4 world to NDC matrix the editor camera or the main 
                        runtime camera. If editorMode is false and there are no runtime cameras, 
                        optional::has_value() will return false.
            *************************************************************************************/
            std::optional<glm::mat4> GetWorldToNdcMatrix(bool const editorMode);

            /*!***********************************************************************************
             \brief Returns an optional object initalized with the 4x4 view to NDC matrix 
                    the editor camera or the main runtime camera.

             \param[in] editorMode Set to true to get the 4x4 view to NDC matrix of 
                            the editor camera, false to get that of the main runtime camera.

             \return std::optional<glm::mat4 const&> - Returns an optional object initalized 
                        with the 4x4 view to NDC matrix the editor camera or the main 
                        runtime camera. If editorMode is false and there are no runtime cameras, 
                        optional::has_value() will return false.
            *************************************************************************************/
            std::optional<glm::mat4> GetViewToNdcMatrix(bool const editorMode);
            
            /*!***********************************************************************************
             \brief Returns an optional object initalized with the 4x4 NDC to world matrix 
                    the editor camera or the main runtime camera.

             \param[in] editorMode Set to true to get the 4x4 NDC to world matrix of 
                            the editor camera, false to get that of the main runtime camera.

             \return std::optional<glm::mat4 const&> - Returns an optional object initalized 
                        with the 4x4 NDC to world matrix the editor camera or the main 
                        runtime camera. If editorMode is false and there are no runtime cameras, 
                        optional::has_value() will return false.
            *************************************************************************************/
            std::optional<glm::mat4> GetNdcToWorldMatrix(bool const editorMode);

            /*!***********************************************************************************
             \brief Returns an optional object initalized with the 4x4 NDC to view matrix 
                    the editor camera or the main runtime camera.

             \param[in] editorMode Set to true to get the 4x4 NDC to view matrix of 
                            the editor camera, false to get that of the main runtime camera.

             \return std::optional<glm::mat4 const&> - Returns an optional object initalized 
                        with the 4x4 NDC to view matrix the editor camera or the main 
                        runtime camera. If editorMode is false and there are no runtime cameras, 
                        optional::has_value() will return false.
            *************************************************************************************/
            std::optional<glm::mat4> GetNdcToViewMatrix(bool const editorMode);
            
            /*!***********************************************************************************
             \brief Returns an optional object initalized with the 4x4 view to world matrix 
                    the editor camera or the main runtime camera.

             \param[in] editorMode Set to true to get the 4x4 view to world matrix of 
                            the editor camera, false to get that of the main runtime camera.

             \return std::optional<glm::mat4 const&> - Returns an optional object initalized 
                        with the 4x4 view to world matrix the editor camera or the main 
                        runtime camera. If editorMode is false and there are no runtime cameras, 
                        optional::has_value() will return false.
            *************************************************************************************/
            std::optional<glm::mat4> GetViewToWorldMatrix(bool const editorMode);

            /*!***********************************************************************************
             \brief Returns an optional object with a reference to the Camera component for 
                    the primary camera used during runtime. 
                    
                    If the ID of the primary camera stored is invalid (i.e. an entity of that ID 
                    does not exist or the entity does not have a Camera component), the value is 
                    instead defaulted to the first runtime camera created. If no runtime cameras 
                    exist, optional::has_value() will return false.

             \return std::optional<Camera const&> - An optional object with a reference to 
                        the Camera component for the primary camera used during runtime. 
                        If no runtime cameras exist, optional::has_value() will return false.
            *************************************************************************************/
            std::optional <std::reference_wrapper<Camera>> GetMainCamera();

            /*!***********************************************************************************
             \brief Returns a reference to the camera used in editor mode.
             \return EditorCamera& - A reference to the camera used in editor mode.
            *************************************************************************************/
            inline EditorCamera& GetEditorCamera() { return m_editorCamera; }

            /*!***********************************************************************************
             \brief Get the system's name, useful for debugging and identification.
             \return std::string The name of the system.
            *************************************************************************************/
            inline std::string GetName() { return m_systemName; }


            // ----- Public setters ----- //
        public:

            /*!***********************************************************************************
             \brief Stores the ID passed in as the ID of the entity to be used as the main camera.
                    NOTE: If the ID passed in is invalid (i.e. an entity of that ID does not exist 
                    or the entity does not have a Camera component), the value is instead defaulted 
                    to the first runtime camera created.
            *************************************************************************************/
            void SetMainCamera(EntityID const cameraEntityId);


            // ----- Public methods ----- //
        public:
            /*!***********************************************************************************
             \brief Does nothing.
            *************************************************************************************/
            void InitializeSystem();

            /*!***********************************************************************************
             \brief Recomputes the matrices of all the cameras if their transforms or viewport 
                    dimensions have been updated.

             \param[in] deltaTime Timestep (in seconds). Not used by this system.
            *************************************************************************************/
            void UpdateSystem(float deltaTime);

            /*!***********************************************************************************
             \brief Does nothing.
            *************************************************************************************/
            void DestroySystem();

            /*!***********************************************************************************
            \brief      Handle window-specific events.

            \param[in]  r_event Event containing window-specific details.
            *************************************************************************************/
            void OnWindowEvent(const PE::Event<PE::WindowEvents>& r_event);

            /*!***********************************************************************************
            \brief      Handle mouse-specific events.

            \param[in]  r_event Event containing mouse-specific details.
            *************************************************************************************/
            void OnMouseEvent(const PE::Event<PE::MouseEvents>& r_event);

            /*!***********************************************************************************
            \brief      Handle keyboard-specific events.

            \param[in]  r_event Event containing keyboard-specific details.
            *************************************************************************************/
            void OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event);

        private:
            std::string m_systemName{ "Camera Manager" }; // Name of system

            // The entity number of the primary camera used during runtime
            EntityID m_mainCameraId{};

            // The camera used in editor mode
            EditorCamera m_editorCamera{};
        };
    } // End of Graphics namespace
} // End of PE namespace
