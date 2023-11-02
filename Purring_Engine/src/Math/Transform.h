/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Transform.h
 \date     20-09-2023
 
 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu

 \brief 
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include "Math/MathCustom.h"
#include "Data/json.hpp"

namespace PE
{
    struct Transform
    {
        // ----- Pubic Variables ----- //
        float width{100}, height{100};
        float orientation{}; // in radians
        vec2 position{};

        float relOrientation{};
        vec2 relPosition{};
        
        // ----- Getter ----- //
        /*!***********************************************************************************
         \brief Gets the Transformation Matrix version of this Transform

         \return mat3x3		The transformation matrix as a mat3x3
        *************************************************************************************/
        mat3x3 GetTransformMatrix3x3() const
        {
            //mat3x3 scaleMat;
            //scaleMat.Scale(width, height);
            mat3x3 rotMat;
            rotMat.RotateRad(orientation);
            mat3x3 transMat;
            transMat.Translate(position.x, position.y);

            return transMat * rotMat;//* scaleMat;
        }

        // ----- Serialization ------ //
        
        /*!***********************************************************************************
        \brief Serializes the Transform object to a JSON object. Converts the width, height,
                orientation, and position of the Transform object to a JSON object for
                easy storage and transmission.

        \return The JSON representation of the Transform object.
        *************************************************************************************/        
        nlohmann::json ToJson() const
        {
            nlohmann::json j;
            j["width"] = width;
            j["height"] = height;
            j["orientation"] = orientation;
            j["position"]["x"] = position.x;
            j["position"]["y"] = position.y;
            
            j["relativeposition"]["x"] = relPosition.x;
            j["relativeposition"]["y"] = relPosition.y;
            j["relorientation"] = relOrientation;
            return j;
        }

        /*!***********************************************************************************
        \brief Deserializes the Transform object from a JSON object. Converts a JSON object to a
                Transform by extracting width, height, orientation, and position values.

        \param[in] r_j JSON object containing the values to load into the Transform object.
        \return The deserialized Transform object.
        *************************************************************************************/
        static Transform FromJson(const nlohmann::json& r_j)
        {
            Transform t;
            t.width = r_j["width"];
            t.height = r_j["height"];
            t.orientation = r_j["orientation"];
            t.position.x = r_j["position"]["x"];
            t.position.y = r_j["position"]["y"];
            if (r_j.contains("relativeposition"))
            {
                t.relPosition.x = r_j["relativeposition"]["x"];
                t.relPosition.y = r_j["relativeposition"]["y"];
            }
            if (r_j.contains("relorientation"))
                t.relOrientation = r_j["relorientation"];
            return t;
        }
    };
}
