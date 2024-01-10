#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Debug.h
 \date     21-12-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains helper functions for visualising debug information.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "Math/MathCustom.h"

namespace PE
{
  namespace Debug
  {
    /*!***********************************************************************************
     \brief Draws a point at the position passed in.

     \param[in] r_position Position to draw the point at.
     \param[in] r_color - [Optional] Color of the point. White by default.
    *************************************************************************************/
    void DrawPoint(vec2 const& r_position, vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });

    /*!***********************************************************************************
     \brief Draws a line starting at `from` towards `to`.

     \param[in] r_from - Starting position of the line.
     \param[in] r_to - End position of the line.
     \param[in] r_color - [Optional] Color of the line. White by default.
    *************************************************************************************/
    void DrawLine(vec2 const& r_from, vec2 const& r_to, vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });

    /*!***********************************************************************************
     \brief Draws a ray starting at `from` to `from + direction`.

     \param[in] r_from - Starting position of the ray.
     \param[in] r_direction - Vector representing the direction + magnitude to add to 
                    the start position.
     \param[in] r_color - [Optional] Color of the ray. White by default.
    *************************************************************************************/
    void DrawRay(vec2 const& r_from, vec2 const& r_direction, vec4 const& r_color = {1.f, 1.f, 1.f, 1.f});

    /*!***********************************************************************************
     \brief Draws a rectangle of dimensions `width` x `height` centered at `r_center`.

     \param[in] r_center - Center of the rectangle.
     \param[in] orientation - Orientation of the rectangle in radians about the z-axis, from the x-axis.
     \param[in] width - Width of the rectangle.
     \param[in] height - Height of the rectangle.
     \param[in] r_color - [Optional] Color of the rectangle. White by default.
    *************************************************************************************/
    void DrawRectangle(vec2 const& r_center, float const orientation, 
        float const width, float const height, vec4 const& r_color = {1.f, 1.f, 1.f, 1.f});
    
    /*!***********************************************************************************
     \brief Draws a rectangle using its top-right and bottom-left corners.

     \param[in] r_max - Maximum x and y values of the AABB.
     \param[in] r_min - Minimum x and y values of the AABB.
     \param[in] r_color - [Optional] Color of the rectangle. White by default.
    *************************************************************************************/
    void DrawAABB(vec2 const& r_max, vec2 const& r_min, vec4 const& r_color = {1.f, 1.f, 1.f, 1.f});

    /*!***********************************************************************************
     \brief Draws a circle centered at `r_center`.

     \param[in] r_center - Center of the circle.
     \param[in] r_radius - Radius of the circle.
     \param[in] r_color - [Optional] Color of the circle. White by default.
    *************************************************************************************/
    void DrawCircle(vec2 const& r_center, float const radius, vec4 const& r_color = {1.f, 1.f, 1.f, 1.f});

    /*!***********************************************************************************
     \brief Draws a "+" at `r_center`.
     
     \param[in] r_center - Position of the center of the cross.
     \param[in] width - Length of the horizontal line of the cross.
     \param[in] height - Length of the veritcal line of the cross.
     \param[in] r_color - [Optional] Color of the cross. White by default.
    *************************************************************************************/
    void DrawCross(vec2 const& r_center, float const width, float const height,
        vec4 const& r_color = { 1.f, 1.f, 1.f, 1.f });
  } // End of Debug namespace
} // End of PE namespace