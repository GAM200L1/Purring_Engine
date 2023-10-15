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
#include "Math/Transform.h"

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
        \brief  Contains data and member functions to compute the view transform matrix.
        *************************************************************************************/
        class Camera
        {
        public:

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in world space to view space as
                    a 4x4 matrix.

            \param[in] r_transform Reference to the camera's transform component.

            \return glm::mat4 - 4x4 matrix to transform coordinates in world space to view space.
            *************************************************************************************/
            glm::mat4 GetWorldToViewMatrix(Transform const& r_transform);

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
            glm::mat4 GetWorldToNdcMatrix(Transform const& r_transform);

            /*!***********************************************************************************
            \brief  Returns true if the camera's transform has been updated and its matrix 
                    needs to be recalculated; false otherwise.

            \param[in] currentPositionX X position of center of camera in the world.
            \param[in] currentPositionY Y position of center of camera in the world.
            \param[in] currentOrientation Orientation of the camera about the z-axis 
                                          (in radians, counter-clockwise from the x-axis).

            \return true - If the camera's transform has been updated and its matrix needs to be 
                            recalculated
            \return false - Camera's transform has not changed, cached camera matrix can be used.
            *************************************************************************************/
            bool GetHasChanged(Transform const& r_transform) const;

            /*!***********************************************************************************
            \brief  Compute the matrix to transform coordinates in world space to view space as
                    a 4x4 matrix and store it as m_cachedViewMatrix.

            \param[in] currentPositionX X position of center of camera in the world.
            \param[in] currentPositionY Y position of center of camera in the world.
            \param[in] currentOrientation Orientation of the camera about the z-axis
                                          (in radians, counter-clockwise from the x-axis).
            \param[in] currentMagnification Zoom to apply to the camera.
            *************************************************************************************/
            void ComputeViewMatrix(Transform const& r_transform);

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
            void SetMagnification(float const value);

            /*!***********************************************************************************
            \brief  Changes the magnification of the camera by the amount passed in.

            \param[in] delta Amount to change the magnification of the camera by.
            *************************************************************************************/
            void AdjustMagnification(float const delta);


        protected:
            float m_magnification{ 1.f };     // Zoom to apply to the camera
            float m_viewportWidth{ 1.f }, m_viewportHeight{ 1.f };  // Height and width of the camera viewport

            // ----- Cached Variables ----- //
            glm::mat4 m_cachedViewMatrix{}; // To prevent unnecessary recalculation of the view matrix
            glm::mat4 m_cachedWorldToNdcMatrix{}; // To prevent unnecessary recalculation of the world to NDC matrix
            float m_cachedPositionX{ -1.f }, m_cachedPositionY{ -1.f }; // Position of center of camera in the world used by the cached matrix
            float m_cachedOrientation{ -1.f };       // Orientation of the camera about the z-axis (in radians, counter-clockwise from the x-axis)

            bool hasTransformChanged{ true }, hasViewportChanged{ true };

        };
    } // End of Graphics namespace
} // End of PE namespace