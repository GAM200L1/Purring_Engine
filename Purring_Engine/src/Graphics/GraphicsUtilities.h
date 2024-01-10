#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GraphicsUtilities.h
 \date:    18-12-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief    This file contains helper functions for graphics, such as converting
           PE::vec* types to glm::vec* types, and generating transformation matrices.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "GLHeaders.h"
#include <glm/glm.hpp>

namespace PE
{
    namespace Graphics
    {
        // Index to restart the primitive. Used in function glPrimitiveRestartIndex()
        const inline GLushort restartIndex{ 0xFFFF };

        // GPU specs
        GLint inline maxVertexCount{};       // GL_MAX_ELEMENTS_VERTICES
        GLint inline maxIndicesCount{};      // GL_MAX_ELEMENTS_INDICES
        GLint inline maxTextureBindings{};   // GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
        GLint inline maxTextureSize{};       // GL_MAX_TEXTURE_SIZE

        /*!***********************************************************************************
         \brief Prints the hardware specifications of the device related to graphics.
        *************************************************************************************/
        void RetrieveSpecifications();

        /*!***********************************************************************************
         \brief Prints the hardware specifications of the device related to graphics.
        *************************************************************************************/
        void PrintSpecifications();

        /*!***********************************************************************************
         \brief Prints error messages from OpenGL.
        *************************************************************************************/
        void APIENTRY GLDebugOutput(GLenum source, GLenum type, unsigned int id,
            GLenum severity, GLsizei length, const char* message, const void* userParam);

        /*!***********************************************************************************
         \brief  Gets the angle (in radians) of the vector from the x-axis.
 
         \param[in] r_vector - Vector to get the angle of.

         \return float - Angle (in radians) of the vector from the x-axis.
        *************************************************************************************/
        inline float ComputeOrientation(float const x, float const y)
        {
            glm::vec2 r_vector{ x, y };
            return (glm::dot(r_vector, glm::vec2{ 1.f, 0.f }) / glm::length(r_vector));
        }

        /*!***********************************************************************************
         \brief  Computes the 4x4 matrix to transform coordinates in model space to world space.
 
         \param[in] width Width of the object.
         \param[in] height Height of the object.
         \param[in] orientation Counterclockwise angle (in radians) about the z-axis from the x-axis.
         \param[in] positionX X position of the object (in world space).
         \param[in] positionY Y position of the object (in world space).
 
         \return glm::mat4 - 4x4 matrix to transform coordinates in model space to world space.
        *************************************************************************************/
        glm::mat4 GenerateTransformMatrix(float const width, float const height,
            float const orientation, float const positionX, float const positionY);

        /*!***********************************************************************************
         \brief  Computes the 4x4 matrix to transform coordinates in model space to world space.
 
         \param[in] rightVector Right vector of the object.
         \param[in] upVector Up vector of the object.
         \param[in] centerPosition Position of the center of the object (in world space).
 
         \return glm::mat4 - 4x4 matrix to transform coordinates in model space to world space.
        *************************************************************************************/
        glm::mat4 GenerateTransformMatrix(glm::vec2 const& rightVector,
            glm::vec2 const& upVector, glm::vec2 const& centerPosition);

        /*!***********************************************************************************
         \brief  Computes the 4x4 matrix to transform coordinates in world space to model space.
 
         \param[in] width Width of the object.
         \param[in] height Height of the object.
         \param[in] orientation Counterclockwise angle (in radians) about the z-axis from the x-axis.
         \param[in] positionX X position of the object (in world space).
         \param[in] positionY Y position of the object (in world space).
 
         \return glm::mat4 - 4x4 matrix to transform coordinates in world space to model space.
        *************************************************************************************/
        glm::mat4 GenerateInverseTransformMatrix(float const width, float const height,
            float const orientation, float const positionX, float const positionY);
    } // End of Graphics namespace
} // End of PE namespace