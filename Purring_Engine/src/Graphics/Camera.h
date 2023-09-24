#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Camera.h
 \date     28-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the camera class, which contains data and 
           member functions to compute the view transform matrix.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glm/glm.hpp>

namespace PE
{
    namespace Graphics
    {
        class Camera
        {
        public:
            // ----- Public getters ----- // 
            glm::vec2 GetPosition() { return m_position; }
        public:

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in world space to view space as
                    a 4x4 matrix.

            \return glm::mat4 - 4x4 matrix to transform coordinates in world space to view space.
            *************************************************************************************/
            glm::mat4 GetWorldToViewMatrix();

            /*!***********************************************************************************
            \brief  Returns true if the camera's transform has been updated and its matrix 
                    needs to be recalculated; false otherwise.

            \return true - If the camera's transform has been updated and its matrix needs to be 
                            recalculated
            \return false - Camera's transform has not changed, cached camera matrix can be used.
            *************************************************************************************/
            bool GetHasChanged() const;


            // ----- Public setters ----- // 
        public:

            /*!***********************************************************************************
            \brief  Sets the magnification of the camera to the value passed in.

            \param[in] value Value to set the magnification of the camera to.
            *************************************************************************************/
            void SetMagnification(float const value);

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
            glm::vec2 m_position{ 0.f, 0.f }; //! Position of center of camera in the world
            float m_orientation{ 0.f };       //! Orientation of the camera about the z-axis (in radians, counter-clockwise from the x-axis)
            float m_magnification{ 1.f };     //! Zoom to apply to the camera

            glm::mat4 m_cachedViewMatrix{}; //! To prevent unnecessary recalculation of the view matrix
            glm::vec2 m_cachedPosition{ -1.f, -1.f }; //! Position of center of camera in the world used by the cached matrix
            float m_cachedOrientation{ -1.f };       //! Orientation of the camera about the z-axis (in radians, counter-clockwise from the x-axis)
            float m_cachedMagnification{ -1.f };     //! Zoom to apply to the camera
        };
    } // End of Graphics namespace
} // End of PE namespace