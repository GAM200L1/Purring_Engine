/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     MeshData.cpp
 \date     28-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief     This file contains the definitions of the functions in the MeshData class, 
            which create and manage the vertex array object created using data
            from the MeshData class.
            
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "NewMeshData.h"
#include <glm/gtc/constants.hpp>    // pi()

namespace PE
{
    namespace Graphics
    {
        // ----------------------------- QUAD MESH ----------------------------- // 
        
        Vector2Container QuadMeshData::GetTransformedVertexPositions(glm::mat4 const& r_transformMatrix)
        {
            Vector2Container transformedVertices;
            transformedVertices.reserve(modelVertexPositions.size());

            for (glm::vec2 const& position : modelVertexPositions)
            {
                glm::vec4 newPosition{ r_transformMatrix * glm::vec4{position.x, position.y, 0.f, 1.f} };
                transformedVertices.emplace_back(newPosition.x, newPosition.y);
            }

            return transformedVertices;
        }


        UVContainer QuadMeshData::GetUvCoordinates(glm::vec2 const& minUv, glm::vec2 const& maxUv)
        {
            return UVContainer{ 
                minUv, glm::vec2{maxUv.x, minUv.y}, // bottom-left, bottom-right
                maxUv, glm::vec2{minUv.x, maxUv.y}  // top-right, top-left
            };
        }




        // ----------------------------- CIRCLE MESH ----------------------------- // 

        Vector2Container CircleMeshData::GetTransformedVertexPositions(glm::mat4 const& r_transformMatrix)
        {
            if (modelVertexPositions.empty()) { GenerateCircleMeshData(); }

            Vector2Container transformedVertices;
            transformedVertices.reserve(modelVertexPositions.size());

            for (glm::vec2 const& position : modelVertexPositions)
            {
                glm::vec4 newPosition{ r_transformMatrix * glm::vec4{position.x, position.y, 0.f, 1.f} };
                transformedVertices.emplace_back(newPosition.x, newPosition.y);
            }

            return transformedVertices;
        }


        Vector2Container CircleMeshData::Get1x1VertexPositions()
        {
            if (modelVertexPositions.empty()) { GenerateCircleMeshData(); }
            return modelVertexPositions;
            //return Vector2Container{};
        }


        IndexContainer const& CircleMeshData::GetCircleOutlineIndices()
        {
            if(circleOutlineIndices.empty()){ GenerateCircleMeshData(); }
            return circleOutlineIndices; 
            //return IndexContainer{};
        }            
        
        
        void CircleMeshData::GenerateCircleMeshData()
        {
            modelVertexPositions.reserve(segments);
            circleOutlineIndices.reserve(segments);

            float angle{ glm::pi<float>() * 2.f / static_cast<float>(segments) };

            // Generate vertices in a circle
            for (int i{ 0 }; i < (int)segments; ++i) {
                float const totalAngle{ static_cast<float>(i) * angle };
                modelVertexPositions.emplace_back(
                    glm::vec2{ glm::cos(totalAngle) * 0.5f, glm::sin(totalAngle) * 0.5f });

                circleOutlineIndices.emplace_back(((i - 1) < 0 ? (short)segments - 1 : (short)i - 1));
                circleOutlineIndices.emplace_back((short)i);
            }
        }




        // ----------------------------- LINE MESH ----------------------------- // 

        Vector2Container LineMeshData::GetTransformedVertexPositions(glm::mat4 const& r_transformMatrix)
        {
            Vector2Container transformedVertices;
            transformedVertices.reserve(modelVertexPositions.size());

            for (glm::vec2 const& position : modelVertexPositions)
            {
                glm::vec4 newPosition{ r_transformMatrix * glm::vec4{position.x, position.y, 0.f, 1.f} };
                transformedVertices.emplace_back(newPosition.x, newPosition.y);
            }

            return transformedVertices;
        }

    } // End of Graphics namespace
} // End of PE namespace
