#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GUIRenderer.h
 \date     25-10-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the GUIRenderer class, which inherits from 
            the Renderer to add to GUI entities to be rendered.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "Renderer.h"

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
         \brief Component to attach to GUI entities to be drawn. This components contains 
                information about the color and texture of the GUI element to be rendered.         
        *************************************************************************************/
        class GUIRenderer : public Renderer {
              /* 
                     Empty by design; it has the same functionality as the Renderer, 
                     but has been separated so that the render passes of the UI and 
                     gameobjects can be separated. 
              */
         };
    } // End of Graphics namespace
} // End of PE namespace