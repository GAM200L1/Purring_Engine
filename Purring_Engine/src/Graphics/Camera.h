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
            glm::vec2 position{ 0.f, 0.f }; //! Position of center of camera in the world
            float orientation{ 0.f };       //! Orientation of the camera about the z-axis (in degrees, counter-clockwise from the x-axis)

            // ----- Public getters ----- // 
        public:

            /*!***********************************************************************************
            \brief  Gets the matrix to transform coordinates in world space to view space as
                    a 4x4 matrix.

            \return glm::mat4 - 4x4 matrix to transform coordinates in world space to view space.
            *************************************************************************************/
            glm::mat4 GetWorldToViewMatrix();


            // ----- Public setters ----- // 
        public:

            /*!***********************************************************************************
            \brief  Changes the magnification of the camera by the amount passed in.

            \param[in] delta Amount to change the magnification of the camera by.
            *************************************************************************************/
            void SetMagnification(float delta);


        private:
            float magnification{ 1.f };     //! Zoom to apply to the camera
            glm::mat4 m_cachedViewMatrix{}; //! To prevent unnecessary recalculation of the view matrix
            int m_cachedWindowWidth{}, m_cachedWindowHeight{}; //! Cached window width and height 
        };
    } // End of Graphics namespace
} // End of PE namespace