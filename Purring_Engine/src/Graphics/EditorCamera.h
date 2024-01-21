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
#include "Camera.h"

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
        \brief  Contains data and member functions to compute the view transform matrix.
        *************************************************************************************/
        class EditorCamera : public Camera
        {
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
            \brief  Returns true if the camera's transform has been updated and its matrix
                    needs to be recalculated; false otherwise.

            \param[in] r_transform Reference to the camera's transform component.

            \return true - If the camera's transform has been updated and its matrix needs to be
                            recalculated
            \return false - Camera's transform has not changed, cached camera matrix can be used.
            *************************************************************************************/
            inline bool GetHasChanged() const { return hasTransformChanged; }

            // ----- Public setters ----- // 
        public:

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
            \brief  Changes the local position of the camera by the amount passed in.

            \param[in] deltaX Amount to move the camera along the camera's local x-axis.
            \param[in] deltaY Amount to move the camera along the camera's local y-axis.
            *************************************************************************************/
            void AdjustLocalPosition(float const deltaX, float const deltaY);

            /*!***********************************************************************************
            \brief  Changes the world position of the camera by the amount passed in.

            \param[in] deltaX Amount to move the camera along the world's x-axis.
            \param[in] deltaY Amount to move the camera along the world's y-axis.
            *************************************************************************************/
            void AdjustWorldPosition(float const deltaX, float const deltaY);

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


            // ----- Public methods ----- // 
        public:

            /*!***********************************************************************************
            \brief  Recomputes and caches the world to NDC and NDC to world matrics if the
                    viewport dimensions or the camera's transform has been updated.

            \param[in] r_transform Reference to the camera's transform component.
            *************************************************************************************/
            void UpdateCamera();


        private:
            glm::vec2 m_position{ 0.f, 0.f }; // Position of center of camera in the world
            float m_orientation{ 0.f };       // Orientation of the camera about the z-axis (in radians, counter-clockwise from the x-axis)

        };
    } // End of Graphics namespace
} // End of PE namespace