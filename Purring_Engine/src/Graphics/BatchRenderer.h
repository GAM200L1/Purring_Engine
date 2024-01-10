#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     BatchRenderer.h
 \date     11-12-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the BatchRenderer class, which manages batches of
           vertices and submits draw calls to the GPU.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "GLHeaders.h"
#include <glm/glm.hpp>

#include "Renderer.h" // EnumMeshType

#include "ShaderProgram.h"
#include "NewMeshData.h"

#include <vector>
#include <string>
#include <memory>

namespace PE
{
    namespace Graphics
    {
        // ----- Type definitions ----- //
        using ShaderProgramKey = std::string;
        using TextureKey = std::string;
        using TextureID = GLuint;

        struct RenderData
        {
            RenderData(EnumMeshType _meshType, float const _width, float const _height, 
                float const _orientation, glm::vec2 const& r_position, glm::vec4 const& r_color) 
                : meshType{ _meshType }, width{ _width }, height{ _height }, 
                orientation{ _orientation }, position{ r_position }, color{ r_color } { /* Empty by design */ }

            EnumMeshType meshType;
            float width, height;
            float orientation;      // in radians, about the z-axis from the x-axis
            glm::vec2 position;
            glm::vec4 color;
        };

        struct TexturedRenderData : public RenderData
        {
            TexturedRenderData(EnumMeshType _meshType, float const _width, float const _height,
                float const _orientation, glm::vec2 const& r_position, glm::vec4 const& r_color,
                UVContainer const& r_uvCoordinates, TextureID const _textureId)
                : RenderData{ _meshType, _width, _height, _orientation, r_position, r_color },
                uvCoordinates{ r_uvCoordinates }, textureId{ _textureId }
            { /* Empty by design */ }

            UVContainer uvCoordinates; // values range from 0 to 1
            TextureID textureId;
        };


        /*!***********************************************************************************
         \brief 
        *************************************************************************************/
        struct VertexDataBase
        {
        public:
            VertexDataBase(glm::vec2 const& r_position, glm::vec4 const& r_color)
                : position{ r_position }, color{ r_color } { /* Empty by design */ }

        public:
            glm::vec2 position{};
            glm::vec4 color{}; // RGBA color. All values have a domain of [0, 1]
        };

        /*!***********************************************************************************
         \brief 
        *************************************************************************************/
        struct TexturedVertexData : public VertexDataBase
        {
        public: 
            TexturedVertexData(glm::vec2 const& r_position, glm::vec4 const& r_color, 
                glm::vec2 const& r_textureCoord, float const bindingPoint)
                : VertexDataBase{ r_position, r_color }, uvCoordinates{ r_textureCoord }, textureBindingPoint { bindingPoint } 
            { /* Empty by design */ }

        public:
            glm::vec2 uvCoordinates{};
            float textureBindingPoint{};
        };


        // ----- Type definitions ----- //
        using RenderDataContainer = std::vector<RenderData>;
        using TexturedRenderDataContainer = std::vector<TexturedRenderData>;

        using UntexturedVertexContainer = std::vector<VertexDataBase>;
        using TexturedVertexContainer = std::vector<TexturedVertexData>;


        /*!***********************************************************************************
         \brief 
        *************************************************************************************/
        class BatchRenderer
        {
            // ----- Public Setters ----- //
        public:

            /*!***********************************************************************************
             \brief Sets the key for the shader program to use for textured objects.

             \param[in] newKey - Key for the shader program to use for textured objects.
            *************************************************************************************/
            void SetTexturedShaderProgram(ShaderProgramKey const& newKey);

            /*!***********************************************************************************
             \brief Sets the key for the shader program to use for colored objects.

             \param[in] newKey - Key for the shader program to use for colored objects.
            *************************************************************************************/
            void SetColoredShaderProgram(ShaderProgramKey const& newKey);


            // ----- Public Methods ----- //
        public:

            /*!***********************************************************************************
             \brief

             \param[in] maxVertices - 
             \param[in] maxTextures - 
            *************************************************************************************/
            void Initialize(unsigned int const maxVertices = 1000U, unsigned int const maxTextures = 16U);

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void Cleanup();

            /*!***********************************************************************************
             \brief              
            *************************************************************************************/
            void Flush();

            /*!***********************************************************************************
             \brief Adds the vertices for a textured quad to the batch.

             \param[in] r_renderData - Data to generate the vertex data with.
             \param[in] r_worldToNdc - Matrix to transform coordinates from world to NDC space.
            *************************************************************************************/
            void DrawQuad(TexturedRenderData const& r_renderData, glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief Adds the vertices for a colored, untextured quad to the batch.

             \param[in] r_renderData - Data to generate the vertex data with.
             \param[in] r_worldToNdc - Matrix to transform coordinates from world to NDC space.
            *************************************************************************************/
            void DrawQuad(RenderData const& r_renderData, glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief Adds the vertex for a point to the batch.

             \param[in] r_renderData - Data to generate the vertex data with.
             \param[in] r_worldToNdc - Matrix to transform coordinates from world to NDC space.
            *************************************************************************************/
            void DrawPoint(RenderData const& r_renderData, glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief Adds the vertices for a line to the batch.

             \param[in] r_renderData - Data to generate the vertex data with.
             \param[in] r_worldToNdc - Matrix to transform coordinates from world to NDC space.
            *************************************************************************************/
            void DrawLine(RenderData const& r_renderData, glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief Adds the vertices for the outline of a rectangle to the batch.

             \param[in] r_renderData - Data to generate the vertex data with.
             \param[in] r_worldToNdc - Matrix to transform coordinates from world to NDC space.
            *************************************************************************************/
            void DrawRectangleOutline(RenderData const& r_renderData, glm::mat4 const& r_worldToNdc);

            /*!***********************************************************************************
             \brief Adds the vertices for the outline of a circle to the batch.

             \param[in] r_renderData - Data to generate the vertex data with.
             \param[in] r_worldToNdc - Matrix to transform coordinates from world to NDC space.
            *************************************************************************************/
            void DrawCircleOutline(RenderData const& r_renderData, glm::mat4 const& r_worldToNdc);


            // ----- Public Methods ----- //
        private:
            UntexturedVertexContainer m_untexturedVertices;
            TexturedVertexContainer m_texturedVertices;
            IndexContainer m_untexturedIndices, m_texturedIndices;

            unsigned int m_primitiveCount{};

            EnumMeshType m_lastMeshDrawn{EnumMeshType::NONE};
            std::vector<TextureID> m_textureIds; // IDs of the textures to be bound. The index of the ID is the index of the texture binding point.

            // Shader program keys
            ShaderProgramKey m_coloredShaderProgramKey{ "Colored" };      // Shader used for batched rendering of colored, untextured meshes
            ShaderProgramKey m_texturedShaderProgramKey{ "Textured" };    // Shader used for batched rendering of textured meshes
            
            // Batch-breaking parameters
            unsigned int m_maxVertices; // Maximum number of vertices before the batch should be broken
            unsigned int m_maxTextures; // Maximum number of textures that can be bound before the batch should be broken

            GLuint m_untexturedVAO, m_texturedVAO;  // IDs for VAOs for textures and untextured objects
            GLuint m_untexturedVBO, m_texturedVBO;  // IDs for VBOs for textured and untextured objects
            GLuint m_indexBufferObject; // ID for index buffer object
            bool m_indexBufferCreated{ false }, m_texturedBufferCreated{ false }, m_untexturedBufferCreated{ false };

            // ----- Private Getters ----- //
        private:

            // ----- Texture Getters ----- //

            /*!***********************************************************************************
             \brief Returns the binding point of the texture ID. If it has not already been stored 
                    for this batch, the texture ID is assigned a binding point. If the maximum 
                    number of textures has been bound, the batch is flushed before the texture ID
                    is assigned a binding point.

             \param[in] r_textureId - OpenGL ID of the texture.

             \return int - Binding point of the texture key for this batch.
            *************************************************************************************/
            int GetTextureBindingPoint(GLuint const& r_textureId);


            // ----- Private Methods ----- //
        private:

            // ----- Batch Management ----- //

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void AddToBatch(EnumMeshType meshType, UntexturedVertexContainer const& r_meshData, IndexContainer const& r_indices);

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void AddToBatch(TexturedVertexContainer const& r_meshData, IndexContainer const& r_indices);

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void RenderTexturedBatch(GLenum const primitiveType);

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void RenderUntexturedBatch(GLenum const primitiveType);


            // ----- VAO and VBO Management ----- //

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void CreateTexturedVAO();

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void CreateUntexturedVAO();

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void CreateIndexBufferObject();

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void UpdateTexturedVAO();

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void UpdateUntexturedVAO();

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void BindTexturedVAO();

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void BindUntexturedVAO();

            /*!***********************************************************************************
             \brief
            *************************************************************************************/
            void UnbindAllVAO();

            ///*!***********************************************************************************
            // \brief Reserves sufficient space for one batch in all the member containers. Ensure 
            //        that [m_maxVertices] and [m_maxTextures] have been set before calling this function.
            //*************************************************************************************/
            //void ReserveContainersSpace();

        }; // End of class

    } // End of Graphics namespace
} // End of PE namespace