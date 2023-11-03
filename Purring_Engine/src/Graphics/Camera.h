#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Camera.h
 \date     28-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the camera class, which contains data and 
           member functions to compute the world to NDC transform matrix using
           orthographic projection.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // ortho()



#include "Math/Transform.h"
#include <Data/json.hpp>


namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
        \brief  Contains data and member functions to compute the view transform matrix.
        *************************************************************************************/
        class Camera
        {
            // ----- Public getters ----- // 
        public:

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in world space to view space as
                    a 4x4 matrix.

            \return glm::mat4 - 4x4 matrix to transform coordinates in world space to view space.
            *************************************************************************************/
            inline glm::mat4 GetWorldToViewMatrix() const { return m_cachedWorldToViewMatrix; }

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in view space to NDC space as
                    a 4x4 matrix.

            \return glm::mat4 - 4x4 matrix to transform coordinates in view space to NDC space.
            *************************************************************************************/
            inline glm::mat4 GetViewToNdcMatrix() const { return m_cachedViewToNdcMatrix; }

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in world space to NDC space as
                    a 4x4 matrix.

            \return glm::mat4 - 4x4 matrix to transform coordinates in world space to NDC space.
            *************************************************************************************/
            inline glm::mat4 GetWorldToNdcMatrix() const { return m_cachedWorldToNdcMatrix; }

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in NDC to view space as
                    a 4x4 matrix.

            \return glm::mat4 - 4x4 matrix to transform coordinates in NDC to view space.
            *************************************************************************************/
            inline glm::mat4 GetNdcToViewMatrix() const { return m_cachedNdcToViewMatrix; }

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in view to world space as
                    a 4x4 matrix.

            \return glm::mat4 - 4x4 matrix to transform coordinates in view to world space.
            *************************************************************************************/
            inline glm::mat4 GetViewToWorldMatrix() const { return m_cachedViewToWorldMatrix; }

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in NDC to world space as
                    a 4x4 matrix.

            \return glm::mat4 - 4x4 matrix to transform coordinates in NDC to world space.
            *************************************************************************************/
            inline glm::mat4 GetNdcToWorldMatrix() const { return m_cachedNdcToWorldMatrix; }

            /*!***********************************************************************************
            \brief  Returns true if this camera is the main camera and is being used to render 
                    gameobjects in the game scene, false otherwise.

            \return true - This camera is the main camera and is being used to render gameobjects 
                        in the game scene.
            \return false - This camera is not the main camera.
            *************************************************************************************/
            inline bool GetIsMainCamera() const { return m_isMainCamera; }

            /*!***********************************************************************************
            \brief  Returns true if Whether this camera is the main camera has been changed in 
                    the last frame.

            \return true - Whether this camera is the main camera has been changed in the last 
                        frame.
            \return false - The status of this camera has not changed.
            *************************************************************************************/
            inline bool GetMainCameraStatusChanged() const { return m_isMainCamera != m_cachedIsMainCamera; }
            
            /*!***********************************************************************************
            \brief  Returns true if the camera's transform has been updated and its matrix 
                    needs to be recalculated; false otherwise.

            \param[in] r_transform Reference to the camera's transform component.

            \return true - Use this as the main camera to render the gameobjects through.
            \return false - This camera is not the main camera. 
            *************************************************************************************/
            inline bool GetHasChanged(Transform const& r_transform) const
            {
                return (m_cachedPositionX != r_transform.position.x ||
                    m_cachedPositionY != r_transform.position.y ||
                    m_cachedOrientation != r_transform.orientation ||
                    hasTransformChanged);
            }

            /*!***********************************************************************************
            \brief  Returns the normalized up vector of the camera.

            \param[in] orientation Orientation of the camera about the z-axis (in radians, 
                            counter-clockwise from the x-axis)

            \return glm::vec2 - The normalized right vector of the camera.
            *************************************************************************************/
            inline glm::vec2 GetUpVector(float const orientation) const
            {
                return glm::vec2{-glm::sin(orientation), glm::cos(orientation)};
            }

            /*!***********************************************************************************
            \brief  Returns the normalized right vector of the camera.

            \param[in] orientation Orientation of the camera about the z-axis (in radians, 
                            counter-clockwise from the x-axis)

            \return glm::vec2 - The normalized right vector of the camera.
            *************************************************************************************/
            inline glm::vec2 GetRightVector(float const orientation) const
            {
                return glm::vec2{ glm::cos(orientation), glm::sin(orientation) };
            }

            /*!***********************************************************************************
            \brief  Returns the aspect ratio (width / height) of the camera's view frustrum.

            \return float - The aspect ratio (width / height) of the camera's view frustrum.
            *************************************************************************************/
            inline float GetAspectRatio() const { return (m_viewportWidth / m_viewportHeight); }

            /*!***********************************************************************************
            \brief  Returns the zoom to apply to the camera.

            \return float - The zoom to apply to the camera.
            *************************************************************************************/
            inline float GetMagnification() const { return m_magnification; }

            /*!***********************************************************************************
            \brief  Returns the width of the viewport of the camera.

            \return float - Width of the viewport of the camera.
            *************************************************************************************/
            inline float GetViewportWidth() const { return m_viewportWidth; }

            /*!***********************************************************************************
            \brief  Returns the height of the viewport of the camera.

            \return float - Height of the viewport of the camera.
            *************************************************************************************/
            inline float GetViewportHeight() const { return m_viewportHeight; }


            // ----- Public setters ----- // 
        public:

            /*!***********************************************************************************
            \brief  Sets whether this main camera will be used as the main camera to render 
                    gameobjects through.

            \param[in] value Set to true to use this camera as the main camera to render the 
                        gameobjects through, false otherwise.
            *************************************************************************************/
            void SetMainCamera(bool const value);

            /*!***********************************************************************************
            \brief  Sets the dimensions of the viewport that the camera renders to.

            \param[in] width Width of the viewport that the camera renders to.
            \param[in] height Height of the viewport that the camera renders to.
            *************************************************************************************/
            void SetViewDimensions(float const width, float const height);

            /*!***********************************************************************************
            \brief  Sets the magnification of the camera to the value passed in.

            \param[in] value Value to set the magnification of the camera to.
            *************************************************************************************/
            void SetMagnification(float value);

            /*!***********************************************************************************
            \brief  Changes the magnification of the camera by the amount passed in.

            \param[in] delta Amount to change the magnification of the camera by.
            *************************************************************************************/
            void AdjustMagnification(float const delta);


            // ----- Public methods ----- // 
        public:

            /*!***********************************************************************************
            \brief  Recomputes and caches the world to NDC and NDC to world matrics if the
                    viewport dimensions or the camera's transform has been updated.

            \param[in] r_transform Reference to the camera's transform component.
            \param[in] isMainCamera Set to true if this camera is the main camera, false otherwise.
            *************************************************************************************/
            void UpdateCamera(Transform const& r_transform, bool const isMainCamera);

            nlohmann::json ToJson(size_t id) const;
            static Camera Deserialize(const nlohmann::json& j);


            // ----- Protected members ----- // 
        protected:
            bool m_isMainCamera{ false };     // Set to true to use this as the main camera to render the gameobjects through. 
            float m_magnification{ 1.f };     // Zoom to apply to the camera
            float m_viewportWidth{ 1.f }, m_viewportHeight{ 1.f };  // Height and width of the camera viewport

            // ----- Cached Variables ----- //
            bool m_cachedIsMainCamera{ false };     // Whether this camera was the main camera in the previous frame
            float m_cachedPositionX{ -1.f }, m_cachedPositionY{ -1.f }; // Position of center of camera in the world used by the cached matrix
            float m_cachedOrientation{ -1.f };       // Orientation of the camera about the z-axis (in radians, counter-clockwise from the x-axis)

            glm::mat4 m_cachedWorldToViewMatrix{}; // To prevent unnecessary recalculation of the world to view matrix
            glm::mat4 m_cachedViewToNdcMatrix{}; // To prevent unnecessary recalculation of the view to NDC matrix
            glm::mat4 m_cachedWorldToNdcMatrix{}; // To prevent unnecessary recalculation of the world to NDC matrix
            
            glm::mat4 m_cachedViewToWorldMatrix{}; // To prevent unnecessary recalculation of the view to world matrix
            glm::mat4 m_cachedNdcToViewMatrix{}; // To prevent unnecessary recalculation of the NDC to view matrix
            glm::mat4 m_cachedNdcToWorldMatrix{}; // To prevent unnecessary recalculation of the NDC to world matrix

            // Set to true when the world to NDC matrix should be recalculated
            bool hasTransformChanged{ true }, hasViewportChanged{ true };


            // ----- Protected methods ----- // 
        protected:

            /*!***********************************************************************************
            \brief  Compute the matrix to transform coordinates in world space to view space and 
                    view to world space as 4x4 matrices and cache them.

            \param[in] r_transform Class containing data about the position and orientation of
                                    the camera.
            *************************************************************************************/
            void ComputeViewMatrix(Transform const& r_transform);

            /*!***********************************************************************************
            \brief  Compute the matrix to transform coordinates in world space to view space and 
                    view to world space as 4x4 matrices and cache them.

            \param[in] positionX X position of center of camera in the world.
            \param[in] positionY Y position of center of camera in the world.
            \param[in] orientation Orientation of the camera about the z-axis
                                   (in radians, counter-clockwise from the x-axis).
            \param[in] magnification Zoom to apply to the camera.
            *************************************************************************************/
            void ComputeViewMatrix(float positionX, float positionY, float orientation, float magnification);

            /*!***********************************************************************************
            \brief  Compute the matrix to transform coordinates in view space to NDC space and 
                    NDC to view space as 4x4 matrices and cache them.
            *************************************************************************************/
            void ComputeNDCMatrix();
        };
    } // End of Graphics namespace
} // End of PE namespace