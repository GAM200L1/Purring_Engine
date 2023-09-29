/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Transform.h
 \date     20-09-2023

 \author               Liew Yeni
 \par      email:      yeni.l\@digipen.edu

 \brief    Contains the implementation of the Transform component.


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include "Math/MathCustom.h"
#include "ECS/Components.h"
#include "Data/json.hpp"

namespace PE
{
    struct Transform
    {
        float width{100}, height{100};
        float orientation{}; // in radians
        vec2 position{};
        
        /*!***********************************************************************************
         \brief Gets the Transformation Matrix version of this Transform

         \return mat3x3		The transformation matrix as a mat3x3
        *************************************************************************************/
        mat3x3 GetTransformMatrix3x3() const
        {
            mat3x3 scaleMat;
            scaleMat.Scale(width, height);
            mat3x3 rotMat;
            rotMat.RotateRad(orientation);
            mat3x3 transMat;
            transMat.Translate(position.x, position.y);

            return transMat * rotMat * scaleMat;
        }

        // Serialization
        nlohmann::json ToJson() const
        {
            nlohmann::json j;
            j["width"] = width;
            j["height"] = height;
            j["orientation"] = orientation;
            j["position"]["x"] = position.x;
            j["position"]["y"] = position.y;
            return j;
        }

        // Deserialization
        static Transform FromJson(const nlohmann::json& r_j)
        {
            Transform t;
            t.width = r_j["width"];
            t.height = r_j["height"];
            t.orientation = r_j["orientation"];
            t.position.x = r_j["position"]["x"];
            t.position.y = r_j["position"]["y"];
            return t;
        }
    };
}