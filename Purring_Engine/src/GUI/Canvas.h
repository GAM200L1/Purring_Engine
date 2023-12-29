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
#include "rttr/registration.h"

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

        /*!***********************************************************************************
         \brief Serializes the data to a JSON file

         \param[in,out] id - ID of Canvas component to get the data of
         \return nlohmann::json - JSON object with data from script
        *************************************************************************************/
        nlohmann::json ToJson(EntityID id) const
        {
            nlohmann::json ret;
            rttr::type dataType = rttr::type::get_by_name(EntityManager::GetInstance().GetComponentID<Canvas>().to_string().c_str());
            rttr::instance inst = EntityManager::GetInstance().Get<Canvas>(id);

            for (auto& prop : dataType.get_properties())
            {
                if (!inst.is_valid())
                    throw;
                // TO NOTE @KRYSTAL
                if (prop.get_type().get_name() == "float")
                {
                    float val = prop.get_value(inst).get_value<float>();
                    ret[prop.get_name().to_string().c_str()] = val;
                }
            }
            return ret;
        }

        /*!***********************************************************************************
         \brief Load the Canvas data from a file

         \param[in,out] j - JSON object with data to load into the Canvas
         \return Canvas - Copy of deserialized Canvas
        *************************************************************************************/
        static Canvas Deserialize(const nlohmann::json& j)
        {
            Canvas ret;

            rttr::type dataType = rttr::type::get_by_name(EntityManager::GetInstance().GetComponentID<Canvas>().to_string().c_str());
            rttr::instance inst = ret;

            for (auto& prop : dataType.get_properties())
            {
                if (!inst.is_valid())
                    throw;
                // can ignore testing of type for now cos only hvae floats
                if (j.contains(prop.get_name().to_string().c_str()))
                {
                    // can streamline once setters are properly setup
                    if (prop.get_name() == "Height")
                    {
                        ret.m_height = j[prop.get_name().to_string().c_str()];
                    }
                    else if (prop.get_name() == "Width")
                    {
                        ret.m_width = j[prop.get_name().to_string().c_str()];
                    }
                }
            }
            return ret;
        }
    private:
        // Dimensions of the canvas. Should match the target resolution of the window.
        float m_width, m_height;

    }; // End of class

} // End of PE namespace