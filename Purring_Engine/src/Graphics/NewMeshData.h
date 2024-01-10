#pragma once

/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     NewMeshData.cpp
 \date     12-12-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the declaration of the MeshData class, 
            which stores the 
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "Graphics/GLHeaders.h"

#include <glm/glm.hpp>

namespace PE
{
    namespace Graphics
    {
        // ----- Type definitions ----- //
        using Vector2Container = std::vector<glm::vec2>;
        using IndexContainer = std::vector<GLushort>;
        using UVContainer = std::array<glm::vec2, 4>;


        /*!***********************************************************************************
         \brief 
        *************************************************************************************/
        struct QuadMeshData 
        {
        public:

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static Vector2Container GetTransformedVertexPositions(glm::mat4 const& r_transformMatrix);


            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static inline Vector2Container Get1x1VertexPositions()
            {
                return modelVertexPositions;
            }


            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static inline UVContainer const& GetDefaultUvCoordinates()
            {
                return defaultUvCoordinates;
            }
            

            /*!***********************************************************************************
             \brief

             \param[in] minUv - Minimum x and y values for the UV coordinates.
             \param[in] maxUv - Maximum x and y values for the UV coordinates.
            *************************************************************************************/
            static UVContainer GetUvCoordinates(glm::vec2 const& minUv, glm::vec2 const& maxUv);


            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static inline IndexContainer const& GetFilledQuadIndices() 
            {
                return filledQuadIndices;
            }
            

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static inline IndexContainer const& GetSquareOutlineIndices() 
            { 
                return squareOutlineIndices; 
            }

        private:
            // Positions of the vertices of a 1x1 square in model space
            const static inline Vector2Container modelVertexPositions { 
                glm::vec2{-0.5f, -0.5f}, glm::vec2{0.5f, -0.5f}, // bottom-left, bottom-right
                glm::vec2{0.5f, 0.5f}, glm::vec2{-0.5f, 0.5f} }; // top-right, top-left

            // Default UV coordinates of the vertices
            const static inline UVContainer defaultUvCoordinates{
                glm::vec2{-1.f, -1.f}, glm::vec2{1.f, -1.f}, // bottom-left, bottom-right
                glm::vec2{1.f, 1.f}, glm::vec2{-1.f, 1.f} }; // top-right, top-left

            // Indices of a quad made of two triangles
            const static inline IndexContainer filledQuadIndices{
                0, 1, 2,    // bottom-right triangle
                2, 3, 0 };  // top-left triangle

            // Indices of a square outline made of a series of lines
            const static inline IndexContainer squareOutlineIndices{
                0, 1, 1, 2,    // bottom, right
                2, 3, 3, 0 };  // top, left
        };
        

        /*!***********************************************************************************
         \brief 
        *************************************************************************************/
        struct CircleMeshData 
        {        
        public:

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static Vector2Container GetTransformedVertexPositions(glm::mat4 const& r_transformMatrix);

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static Vector2Container Get1x1VertexPositions();

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static IndexContainer const& GetCircleOutlineIndices();

        private:
            constexpr static inline std::size_t segments{ 32 };
            static inline Vector2Container modelVertexPositions{};
            static inline IndexContainer circleOutlineIndices{};


            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static void GenerateCircleMeshData();
        };
        

        /*!***********************************************************************************
         \brief 
        *************************************************************************************/
        struct LineMeshData
        {
        
        public:

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static Vector2Container GetTransformedVertexPositions(glm::mat4 const& r_transformMatrix);


            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static inline Vector2Container Get1x1VertexPositions()
            {
                return modelVertexPositions;
            }


            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static inline IndexContainer const& GetLineIndices() 
            { 
                return indices; 
            }

        private:
            const static inline Vector2Container modelVertexPositions 
                { glm::vec2{-0.5f, 0.f}, glm::vec2{0.5f, 0.f} };

            const static inline IndexContainer indices{ 0, 1 };
        };


        struct PointMeshData
        {
            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static inline IndexContainer const& GetPointIndices()
            {
                return indices;
            }

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            static glm::vec2 GetTransformedVertexPositions(glm::mat4 const& r_transformMatrix, // world to ndc
                float const worldPositionX, float const worldPositionY)
            {
                glm::vec4 vec4Pos{ r_transformMatrix * glm::vec4{worldPositionX, worldPositionY, 0.f, 1.f} };
                return glm::vec2{ vec4Pos.x, vec4Pos.y };
            }

            const static inline IndexContainer indices{ 0 };
        };


    } // End of Graphics namespace
} // End of PE namespace
