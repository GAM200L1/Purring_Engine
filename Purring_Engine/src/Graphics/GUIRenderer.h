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

                     Will be extended in the future to allow for different colors 
                     and textures for different states (on button hover, on button 
                     click etc.).
              */
        public:
              /*!***********************************************************************************
             \brief Serializes the data attached to this renderer.
            *************************************************************************************/
            nlohmann::json ToJson(size_t id) const
            {
                id;

                nlohmann::json j;
                j["TextureKey"] = GetTextureKey();
                j["Color"]["r"] = GetColor().r;
                j["Color"]["g"] = GetColor().g;
                j["Color"]["b"] = GetColor().b;
                j["Color"]["a"] = GetColor().a;
                return j;
            }

            /*!***********************************************************************************
             \brief Deserializes data from a JSON file and loads it as values to set this
                    component to.

             \param[in] j JSON object containing the values to load into the renderer component.
            *************************************************************************************/
            static GUIRenderer FromJson(const nlohmann::json& j)
            {
                GUIRenderer r;
                r.SetTextureKey(j["TextureKey"]);
                r.SetColor(j["Color"]["r"], j["Color"]["g"], j["Color"]["b"], j["Color"]["a"]);
                return r;
            }
         };
    } // End of Graphics namespace
} // End of PE namespace