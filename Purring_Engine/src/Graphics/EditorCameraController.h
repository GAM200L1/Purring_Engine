#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EditorCameraController.h
 \date     12-10-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the class that controls the editor camera.
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include <glm/glm.hpp>

#include "EditorCamera.h"
#include "Input/InputSystem.h"
#include "Events/EventHandler.h"

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
        \brief  Contains listeners to move the editor camera based on mouse and key events.
        *************************************************************************************/
        class EditorCameraController
        {
        public:

            /*!***********************************************************************************
            \brief      Subscribe this controller's listeners to the input events.
            *************************************************************************************/
            void InitializeCameraController();

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
        };
    } // End of Graphics namespace
} // End of PE namespace