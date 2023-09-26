/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Transform.h
 \date     20-09-2023

 \author               Liew Yeni
 \par      email:      yeni.l\@digipen.edu

 \brief     Contains the implementation of the Transform component.


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#pragma once

#include "Math/MathCustom.h"
#include "ECS/Components.h"

namespace PE
{
    struct Transform
    {
        float width{}, height{};
        float orientation{}; // in radians
        vec2 position{};

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

        mat4x4 GetTransformMatrix4x4() const
        {
            return GetTransformMatrix3x3().ConvertTo4x4();
        }
    };
}