#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Canvas.h
 \date     18-12-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the Canvas component. Only GUI elements childed 
           to an object with a Canvas component will be drawn and updated. 

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

//#include "GUIRenderer.h"

namespace PE
{
    /*!***********************************************************************************
        \brief The Canvas component. Only GUI elements childed to an object 
                with a Canvas component will be drawn and updated. 
    *************************************************************************************/
    class Canvas
    {    
    public:
        /*!***********************************************************************************
            \brief Returns the width of the canvas. Should match the target resolution 
                    of the window.
        *************************************************************************************/
        inline float GetWidth() const { return m_width; }

        /*!***********************************************************************************
            \brief Returns the height of the canvas. Should match the target resolution 
                    of the window.
        *************************************************************************************/
        inline float GetHeight() const { return m_height; }

    private:
        // Dimensions of the canvas. Should match the target resolution of the window.
        float m_width, m_height; 

    }; // End of class

} // End of PE namespace