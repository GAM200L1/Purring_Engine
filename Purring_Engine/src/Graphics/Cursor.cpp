/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Cursor.cpp
 \date     06-04-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the Cursor class, which is used to render a custom cursor.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "Cursor.h"
#include "Texture.h"
#include "../ResourceManager/ResourceManager.h"
#include "CameraManager.h"
#include "System.h"

#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE

namespace PE
{
    namespace Graphics
    {
        std::shared_ptr<Graphics::Texture> Cursor::GetAppropriateTexture() const
        {
            switch (m_currentState)
            {
            case EnumCursorState::HOVER:
                return ResourceManager::GetInstance().GetHoverCursorTexture();
            default:
                return ResourceManager::GetInstance().GetDefaultCursorTexture();
            }
        }

        void Cursor::SetVisibility(bool _isVisible)
        {
            if (m_isVisible != _isVisible)
            {
                // Enable the original cursor if the custom cursor should not be visible
                WindowManager::GetInstance().ToggleCursor(!_isVisible);
            }

            m_isVisible = _isVisible;
        }


        void Cursor::Update()
        {
            // Update the position of the cursor
            float mouseX{}, mouseY{};
            InputSystem::GetCursorViewportPosition(WindowManager::GetInstance().GetWindow(), mouseX, mouseY);
            m_position = GETCAMERAMANAGER()->GetWindowToWorldPosition(mouseX, mouseY);
        }
    } // End of Graphics namespace
} // End of PE namespace