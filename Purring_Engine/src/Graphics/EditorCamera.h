#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EditorCamera.h
 \date     12-10-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the editor camera class, which contains data and 
           member functions to compute the view transform matrix for the camera 
           in editor mode.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include <glm/glm.hpp>

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
        \brief  Contains data and member functions to compute the view transform matrix.
        *************************************************************************************/
        class EditorCamera
        {
            // ----- Public getters ----- // 
        public:

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in world space to view space as
                    a 4x4 matrix.

            \return glm::mat4 - 4x4 matrix to transform coordinates in world space to view space.
            *************************************************************************************/
            glm::mat4 GetWorldToViewMatrix();

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in view space to NDC space as
                    a 4x4 matrix.

            \return glm::mat4 - 4x4 matrix to transform coordinates in view space to NDC space.
            *************************************************************************************/
            glm::mat4 GetViewToNdcMatrix() const;

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in world space to NDC space as
                    a 4x4 matrix.

            \return glm::mat4 - 4x4 matrix to transform coordinates in world space to NDC space.
            *************************************************************************************/
            glm::mat4 GetWorldToNdcMatrix();

            /*!***********************************************************************************
            \brief  Returns true if the camera's transform has been updated and its matrix 
                    needs to be recalculated; false otherwise.

            \return true - If the camera's transform has been updated and its matrix needs to be 
                            recalculated
            \return false - Camera's transform has not changed, cached camera matrix can be used.
            *************************************************************************************/
            bool GetHasChanged() const;

            /*!***********************************************************************************
            \brief  Compute the matrix to transform coordinates in world space to view space as
                    a 4x4 matrix and store it as m_cachedViewMatrix.
            *************************************************************************************/
            void ComputeViewMatrix();

            /*!***********************************************************************************
            \brief  Returns the aspect ratio (width / height) of the camera's view frustrum.

            \return float - The aspect ratio (width / height) of the camera's view frustrum.
            *************************************************************************************/
            inline float GetAspectRatio() const { return (m_viewportWidth / m_viewportHeight); }

            /*!***********************************************************************************
            \brief  Returns the position of center of camera in the world.

            \return glm::vec2 - The position of center of camera in the world.
            *************************************************************************************/
            inline glm::vec2 GetPosition() const { return m_position; }

            /*!***********************************************************************************
            \brief  Returns the orientation of the camera about the z-axis 
                    (in radians, counter-clockwise from the x-axis).

            \return float - The orientation of the camera about the z-axis 
                            (in radians, counter-clockwise from the x-axis)
            *************************************************************************************/
            inline float GetOrientation() const { return m_orientation; }

            /*!***********************************************************************************
            \brief  Returns the zoom to apply to the camera.

            \return float - The zoom to apply to the camera.
            *************************************************************************************/
            inline float GetMagnification() const { return m_magnification; }


            // ----- Public setters ----- // 
        public:

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
            \brief  Sets the position of the camera to the value passed in.

            \param[in] valueX Value to set the camera to along the camera's x-axis.
            \param[in] valueY Value to set the camera to along the camera's y-axis.
            *************************************************************************************/
            void SetPosition(float const valueX, float const valueY);

            /*!***********************************************************************************
            \brief  Sets the orientation of the camera about the z-axis from the x-axis 
                    to the amount passed in.

            \param[in] degrees Orientation of the camera about the z-axis (in degrees, 
                             counter-clockwise from the x-axis)
            *************************************************************************************/
            void SetRotationDegrees(float const degrees);

            /*!***********************************************************************************
            \brief  Sets the orientation of the camera about the z-axis from the x-axis 
                    to the amount passed in.

            \param[in] radians Orientation of the camera about the z-axis (in radians, 
                             counter-clockwise from the x-axis)
            *************************************************************************************/
            void SetRotationRadians(float const radians);

            /*!***********************************************************************************
            \brief  Changes the magnification of the camera by the amount passed in.

            \param[in] delta Amount to change the magnification of the camera by.
            *************************************************************************************/
            void AdjustMagnification(float const delta);

            /*!***********************************************************************************
            \brief  Changes the position of the camera by the amount passed in.

            \param[in] deltaX Amount to move the camera along the camera's x-axis.
            \param[in] deltaY Amount to move the camera along the camera's y-axis.
            *************************************************************************************/
            void AdjustPosition(float const deltaX, float const deltaY);

            /*!***********************************************************************************
            \brief  Changes the orientation of the camera about the z-axis from the x-axis 
                    by the amount passed in.

            \param[in] delta Angle (in degrees) to rotate the camera by. Pass in a positive value
                             to rotate counter-clockwise and negative to rotate clockwise.
            *************************************************************************************/
            void AdjustRotationDegrees(float const delta);

            /*!***********************************************************************************
            \brief  Changes the orientation of the camera about the z-axis from the x-axis 
                    by the amount passed in.

            \param[in] delta Angle (in radians) to rotate the camera by. Pass in a positive value
                             to rotate counter-clockwise and negative to rotate clockwise.
            *************************************************************************************/
            void AdjustRotationRadians(float const delta);


        private:
            glm::vec2 m_position{ 0.f, 0.f }; // Position of center of camera in the world
            float m_orientation{ 0.f };       // Orientation of the camera about the z-axis (in radians, counter-clockwise from the x-axis)
            float m_magnification{ 1.f };     // Zoom to apply to the camera
            float m_viewportWidth{ 1.f }, m_viewportHeight{ 1.f };  // Height and width of the camera viewport

            // ----- Cached Variables ----- //
            glm::mat4 m_cachedViewMatrix{}; // To prevent unnecessary recalculation of the view matrix
            glm::mat4 m_cachedWorldToNdcMatrix{}; // To prevent unnecessary recalculation of the world to NDC matrix

            // Set to true when the world to NDC matrix should be recalculated
            bool hasTransformChanged{ true }, hasViewportChanged{ true };
        };
    } // End of Graphics namespace
} // End of PE namespace