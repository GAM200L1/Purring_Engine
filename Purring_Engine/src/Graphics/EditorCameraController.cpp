/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EditorCameraController.cpp
 \date     12-10-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the definition of the member functions of the class
           that manages the editor and runtime cameras.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "EditorCameraController.h"

namespace PE 
{
    namespace Graphics
    {
        EditorCameraController::EditorCameraController() 
        {
            //InitializeCameraController();
        }

        void EditorCameraController::InitializeCameraController()
        {
            ADD_ALL_WINDOW_EVENT_LISTENER(EditorCameraController::OnWindowEvent, this)
            ADD_ALL_MOUSE_EVENT_LISTENER(EditorCameraController::OnMouseEvent, this)
            ADD_ALL_KEY_EVENT_LISTENER(EditorCameraController::OnKeyEvent, this)
        }


        void EditorCameraController::OnWindowEvent(const PE::Event<PE::WindowEvents>&)
        {
            std::cout << "EditorCameraController::OnWindowEvent\n";
        }


        void EditorCameraController::OnMouseEvent(const PE::Event<PE::MouseEvents>&)
        {
            std::cout << "EditorCameraController::OnMouseEvent\n";
        }


        void EditorCameraController::OnKeyEvent(const PE::Event<PE::KeyEvents>&)
        {
            std::cout << "EditorCameraController::OnKeyEvent\n";
        }

    } // End of Graphics namespace
} // End of PE namspace
