/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     BatchRenderer.cpp
 \date:    11-12-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief    This file contains the BatchRenderer class, which manages batches of
           vertices and submits draw calls to the GPU.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"

#include "BatchRenderer.h"
#include "GraphicsUtilities.h"

#include "ResourceManager/ResourceManager.h"
#include "Logging/Logger.h"
extern Logger engine_logger;

namespace PE
{
    namespace Graphics
    {
        void BatchRenderer::SetTexturedShaderProgram(ShaderProgramKey const& newKey)
        {
            m_texturedShaderProgramKey = newKey;
        }


        void BatchRenderer::SetColoredShaderProgram(ShaderProgramKey const& newKey)
        {
            m_coloredShaderProgramKey = newKey;
        }


        void BatchRenderer::Initialize(unsigned int const maxVertices, unsigned int const maxTextures)
        {
            m_maxVertices = maxVertices;
            m_maxTextures = maxTextures;

            // Reserve enough space in the containers
            //ReserveContainersSpace();
            m_untexturedVertices.reserve(m_maxVertices);
            m_texturedVertices.reserve(m_maxVertices);
            m_untexturedIndices.reserve(m_maxVertices);
            m_texturedIndices.reserve(m_maxVertices);

            m_textureIds.reserve(m_maxTextures);

            // Create the vertex array objects and buffer objects
            CreateTexturedVAO();
            CreateUntexturedVAO();
        }


        void BatchRenderer::Cleanup()
        {
            // Delete VBOs and VAOs
            glDeleteBuffers(1, &m_untexturedVBO);
            glDeleteBuffers(1, &m_texturedVBO);

            glDeleteBuffers(1, &m_indexBufferObject);

            glDeleteVertexArrays(1, &m_untexturedVAO);
            glDeleteVertexArrays(1, &m_texturedVAO);

            m_indexBufferCreated = false;
            m_texturedBufferCreated = false;
            m_untexturedBufferCreated = false;
        }


        void BatchRenderer::Flush()
        {
            switch (m_lastMeshDrawn)
            {
            case EnumMeshType::QUAD:
            {
                RenderUntexturedBatch(GL_TRIANGLES);
                break;
            }
            case EnumMeshType::TEXTURED_QUAD:
            {
                RenderTexturedBatch(GL_TRIANGLES);
                break;
            }
            case EnumMeshType::DEBUG_SQUARE:
            case EnumMeshType::DEBUG_CIRCLE:
            case EnumMeshType::DEBUG_LINE:
            {
                RenderUntexturedBatch(GL_LINES);
                break;
            }
            case EnumMeshType::DEBUG_POINT:
            {
                RenderUntexturedBatch(GL_POINTS);
                break;
            }
            default: break;
            }
        }




        // ----------------------------- HELPER DRAW FUNCTIONS ----------------------------- // 

        void BatchRenderer::DrawQuad(TexturedRenderData const& r_renderData, glm::mat4 const& r_worldToNdc)
        {
            // Flush the current batch if we're swapping between 
            if (m_lastMeshDrawn != EnumMeshType::NONE && m_lastMeshDrawn != r_renderData.meshType)
            {
                Flush();
            }
            m_lastMeshDrawn = r_renderData.meshType;

            // Get texture binding point
            float bindingPoint{ static_cast<float>(GetTextureBindingPoint(r_renderData.textureId)) };

            // Get model to NDC matrix
            glm::mat4 modelToNdc{ r_worldToNdc * GenerateTransformMatrix(
                r_renderData.width, r_renderData.height, r_renderData.orientation, 
                r_renderData.position.x, r_renderData.position.y) };

            // Compute vertex positions in NDC space
            Vector2Container ndcPositions{ QuadMeshData::GetTransformedVertexPositions(modelToNdc) };
            //Vector2Container ndcPositions{ 
            //    glm::vec2{-0.5f, -0.5f}, // bottom left
            //    glm::vec2{ 0.5f, -0.5f}, // bottom right
            //    glm::vec2{ 0.5f,  0.5f}, // top right
            //    glm::vec2{-0.5f,  0.5f}  // top left
            //};

            // Create vertex data
            const std::size_t containerSize{ 4 };
            TexturedVertexContainer vertices{};
            vertices.reserve(containerSize);
            for (std::size_t i{}; i < containerSize; ++i) {
                vertices.emplace_back(glm::vec2{ndcPositions[i].x, ndcPositions[i].y}, r_renderData.color, r_renderData.uvCoordinates[i], bindingPoint);
            }

            AddToBatch(vertices, QuadMeshData::GetFilledQuadIndices());
        }


        void BatchRenderer::DrawQuad(RenderData const& r_renderData, glm::mat4 const& r_worldToNdc)
        {
            // Get model to NDC matrix
            glm::mat4 modelToNdc{ r_worldToNdc * GenerateTransformMatrix(
                r_renderData.width, r_renderData.height, r_renderData.orientation,
                r_renderData.position.x, r_renderData.position.y) };

            // Get vertex positions in NDC
            Vector2Container ndcPositions{ QuadMeshData::GetTransformedVertexPositions(modelToNdc) };

            // Create vertex data
            const std::size_t containerSize{ 4 };
            UntexturedVertexContainer vertices{};
            vertices.reserve(containerSize);
            for (std::size_t i{}; i < containerSize; ++i) {
                vertices.emplace_back(glm::vec2{ ndcPositions[i].x, ndcPositions[i].y }, r_renderData.color);
            }

            AddToBatch(EnumMeshType::QUAD, vertices, QuadMeshData::GetFilledQuadIndices());
        }


        void BatchRenderer::DrawPoint(RenderData const& r_renderData, glm::mat4 const& r_worldToNdc)
        {
            // Create vertex data
            UntexturedVertexContainer vertices{ {PointMeshData::GetTransformedVertexPositions(r_worldToNdc, r_renderData.position.x, r_renderData.position.y), r_renderData.color } };

            AddToBatch(EnumMeshType::DEBUG_POINT, vertices, PointMeshData::GetPointIndices());
        }


        void BatchRenderer::DrawLine(RenderData const& r_renderData, glm::mat4 const& r_worldToNdc)
        {
            // Get model to NDC matrix
            glm::mat4 modelToNdc{ r_worldToNdc * GenerateTransformMatrix(
                r_renderData.width, r_renderData.height, r_renderData.orientation,
                r_renderData.position.x, r_renderData.position.y) };

            // Get vertex positions in NDC
            Vector2Container ndcPositions{ QuadMeshData::GetTransformedVertexPositions(modelToNdc) };

            // Create vertex data
            const std::size_t containerSize{ 2 };
            UntexturedVertexContainer vertices{};
            vertices.reserve(containerSize);

            for (std::size_t i{}; i < containerSize; ++i) {
                vertices.emplace_back(glm::vec2{ ndcPositions[i].x, ndcPositions[i].y }, r_renderData.color);
            }

            AddToBatch(EnumMeshType::DEBUG_LINE, vertices, LineMeshData::GetLineIndices());
        }


        void BatchRenderer::DrawRectangleOutline(RenderData const& r_renderData, glm::mat4 const& r_worldToNdc)
        {
            // Get model to NDC matrix
            glm::mat4 modelToNdc{ r_worldToNdc * GenerateTransformMatrix(
                r_renderData.width, r_renderData.height, r_renderData.orientation,
                r_renderData.position.x, r_renderData.position.y) };

            // Get vertex positions in NDC
            Vector2Container ndcPositions{ QuadMeshData::GetTransformedVertexPositions(modelToNdc) };

            // Create vertex data
            const std::size_t containerSize{ 4 };
            UntexturedVertexContainer vertices{};
            vertices.reserve(containerSize);
            for (std::size_t i{}; i < containerSize; ++i) {
                vertices.emplace_back(glm::vec2{ ndcPositions[i].x, ndcPositions[i].y }, r_renderData.color);
            }

            AddToBatch(EnumMeshType::DEBUG_SQUARE, vertices, QuadMeshData::GetSquareOutlineIndices());
        }


        void BatchRenderer::DrawCircleOutline(RenderData const& r_renderData, glm::mat4 const& r_worldToNdc)
        {
            // Get model to NDC matrix
            glm::mat4 modelToNdc{ r_worldToNdc * GenerateTransformMatrix(
                r_renderData.width, r_renderData.height, r_renderData.orientation,
                r_renderData.position.x, r_renderData.position.y) };

            // Get vertex positions in NDC
            Vector2Container ndcPositions{ CircleMeshData::GetTransformedVertexPositions(modelToNdc) };

            // Create vertex data
            const std::size_t containerSize{ CircleMeshData::Get1x1VertexPositions().size() };
            UntexturedVertexContainer vertices{};
            vertices.reserve(containerSize);
            for (std::size_t i{}; i < containerSize; ++i) {
                vertices.emplace_back(glm::vec2{ ndcPositions[i].x, ndcPositions[i].y }, r_renderData.color);
            }

            AddToBatch(EnumMeshType::DEBUG_CIRCLE, vertices, CircleMeshData::GetCircleOutlineIndices());
        }



        // ----------------------------- Texture Getters ----------------------------- //

        int BatchRenderer::GetTextureBindingPoint(GLuint const& r_textureId)
        {
            std::vector<TextureID>::iterator it{ std::find(m_textureIds.begin(), m_textureIds.end(), r_textureId) };
            
            // Check if the texture has been added to this batch alr
            if (it != m_textureIds.end())
            {
                return static_cast<int>(it - m_textureIds.begin());
            }

            // Check how many textures have been bound
            if (m_textureIds.size() == m_maxTextures)
            {
                // Too many textures are bound, flush the current batch first
                Flush();
                //m_textureKeys.clear();
            }

            // Add this texture to the container of textures to be bound
            m_textureIds.emplace_back(r_textureId);
            return static_cast<int>(m_textureIds.end() - m_textureIds.begin() - 1);
        }



        // ----------------------------- BATCH MANAGEMENT ----------------------------- // 

        void BatchRenderer::AddToBatch(EnumMeshType meshType, UntexturedVertexContainer const& r_meshData, IndexContainer const& r_indices)
        {
            /*
                Reasons to break batch:
                    - Too many textures - NA
                    - Too many vertices
                    - New shaderprogram - done
                    - New mesh
            */
            
            if ((m_lastMeshDrawn != EnumMeshType::NONE && m_lastMeshDrawn != meshType) // Check if a new mesh is being drawn
                || ((m_untexturedIndices.size() + r_indices.size()) >= m_maxVertices)) // Check if the max number of vertices has been reached
            {
                Flush();
            }
            m_lastMeshDrawn = meshType;

            // Add the indices and vertex data to the batch
            m_untexturedVertices.insert(m_untexturedVertices.cend(), r_meshData.begin(), r_meshData.end());
            m_untexturedIndices.insert(m_untexturedIndices.cend(), r_indices.begin(), r_indices.end());

            // Add primitive restart index
            m_untexturedIndices.emplace_back(restartIndex);

            // Increase primitive count
            ++m_primitiveCount;
        }


        void BatchRenderer::AddToBatch(TexturedVertexContainer const& r_meshData, IndexContainer const& r_indices)
        {
            /*
                Reasons to break batch:
                    - Too many textures - done
                    - Too many vertices
                    - New shaderprogram - done
                    - New mesh - done
            */

            // We've checked if this is a new mesh in the helper draw function that calls this function

            // Check if the max number of vertices has been reached
            if ((m_texturedIndices.size() + r_indices.size()) >= m_maxVertices)
            {
                Flush();
            }

            // Add the indices and vertex data to the batch
            m_texturedVertices.insert(m_texturedVertices.cend(), r_meshData.begin(), r_meshData.end());
            m_texturedIndices.insert(m_texturedIndices.cend(), r_indices.begin(), r_indices.end());

            // Add primitive restart index
            m_texturedIndices.emplace_back(restartIndex);

            // Increase primitive count
            ++m_primitiveCount;
        }


        void BatchRenderer::RenderTexturedBatch(GLenum const primitiveType)
        {
            if (m_texturedVertices.empty()) { return; }

            // Bind shaderprogram
            std::shared_ptr<ShaderProgram> shaderProgramPointer{ ResourceManager::GetInstance().GetShaderProgram(m_texturedShaderProgramKey) };
            if (!shaderProgramPointer) { return; }
            shaderProgramPointer->Use();


            // ----------------------------- Load and bind textures ----------------------------- //
            // LEAVING THIS CODE HERE FOR HISTORICAL PURPOSES. 
            // This was used when I stored the texture keys instead of texture IDs.
            // 
            //std::vector<std::shared_ptr<Texture>> textures{};
            //textures.reserve(m_textureKeys.size());

            //for (TextureKey const& key : m_textureKeys)
            //{
            //    textures.emplace_back(ResourceManager::GetInstance().GetTexture(key));
            //}

            //// Ensure that we don't bind the same texture twice
            //std::vector<std::shared_ptr<Texture>>::iterator last{ std::unique(textures.begin(), textures.end()) };
            //textures.erase(last, textures.end());

            //// Bind all the textures
            //for (unsigned int i{}; i < textures.size(); ++i)
            //{
            //    textures[i]->Bind(i);
            //}
             
             
            // Bind all the textures
            for (unsigned int i{}; i < m_textureIds.size(); ++i)
            {
                glBindTextureUnit(i, m_textureIds[i]);
            }
            // ----------------------------- end of load and bind textures ----------------------------- //

            
            // Update VAO attributes
            UpdateTexturedVAO();
            BindTexturedVAO();

            // Make drawcall
            glDrawElements(primitiveType, static_cast<GLsizei>(m_texturedIndices.size()), GL_UNSIGNED_SHORT, NULL);

            // Unbind all textures
            for (unsigned int i{}; i < m_textureIds.size(); ++i)
            {
                glBindTextureUnit(i, 0);
            }

            // Unbind shader program and VAO
            shaderProgramPointer->UnUse();
            UnbindAllVAO();

            // Clear containers
            m_texturedVertices.clear(); m_texturedIndices.clear();
            m_textureIds.clear();
            m_lastMeshDrawn = EnumMeshType::NONE;
        }


        void BatchRenderer::RenderUntexturedBatch(GLenum const primitiveType)
        {
            if (m_untexturedVertices.empty()) { return; }

            // Bind shaderprogram
            std::shared_ptr<ShaderProgram> shaderProgramPointer{ ResourceManager::GetInstance().GetShaderProgram(m_texturedShaderProgramKey) };
            if (!shaderProgramPointer) { return; }
            shaderProgramPointer->Use();


            // Update VAO attributes
            UpdateUntexturedVAO();
            BindUntexturedVAO();


            // Make drawcall
            glDrawElements(primitiveType, static_cast<GLsizei>(m_untexturedIndices.size()), GL_UNSIGNED_SHORT, NULL);


            // Unbind shader program and VAO
            shaderProgramPointer->UnUse();
            UnbindAllVAO();


            // Clear containers
            m_untexturedVertices.clear(); m_untexturedIndices.clear();
            m_lastMeshDrawn = EnumMeshType::NONE;
        }



        // ----------------------------- VAO and VBO Management ----------------------------- //

        void BatchRenderer::CreateTexturedVAO()
        {
            //@CHECK krystal
            if (m_texturedBufferCreated) { return; }
            m_texturedBufferCreated = true;

            // Create buffer object for vertex data
            glCreateBuffers(1, &m_texturedVBO);
            glNamedBufferStorage(m_texturedVBO,
                static_cast<GLsizeiptr>(m_maxVertices * sizeof(TexturedVertexData)),
                nullptr, GL_DYNAMIC_STORAGE_BIT);

            // Create VAO
            glCreateVertexArrays(1, &m_texturedVAO);

            // Bind the positions to attrib 0
            GLuint attributeIndex{ 0 }, bindingIndex{ 0 };

            glEnableVertexArrayAttrib(m_texturedVAO, attributeIndex);
            glVertexArrayVertexBuffer(m_texturedVAO, bindingIndex, m_texturedVBO, 0,
                static_cast<GLsizei>(sizeof(TexturedVertexData)));
            glVertexArrayAttribFormat(m_texturedVAO, attributeIndex, 2, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(m_texturedVAO, attributeIndex, bindingIndex);

            // Bind the colors to attrib 1
            ++attributeIndex, ++bindingIndex;
            glEnableVertexArrayAttrib(m_texturedVAO, attributeIndex);
            glVertexArrayVertexBuffer(m_texturedVAO, bindingIndex, m_texturedVBO, 0,
                static_cast<GLsizei>(sizeof(TexturedVertexData)));
            glVertexArrayAttribFormat(m_texturedVAO, attributeIndex, 4, GL_FLOAT, GL_FALSE,
                static_cast<GLuint>(sizeof(glm::vec2)));	// offset by vert pos
            glVertexArrayAttribBinding(m_texturedVAO, attributeIndex, bindingIndex);

            // Bind the texture coordinate to attrib 2
            ++attributeIndex, ++bindingIndex;
            glEnableVertexArrayAttrib(m_texturedVAO, attributeIndex);
            glVertexArrayVertexBuffer(m_texturedVAO, bindingIndex, m_texturedVBO, 0,
                static_cast<GLsizei>(sizeof(TexturedVertexData)));
            glVertexArrayAttribFormat(m_texturedVAO, attributeIndex, 2, GL_FLOAT, GL_FALSE,
                static_cast<GLintptr>(sizeof(glm::vec2) + sizeof(glm::vec4))); // offset by vert pos and color
            glVertexArrayAttribBinding(m_texturedVAO, attributeIndex, bindingIndex);

            // Bind the texture unit num to attrib 3
            ++attributeIndex, ++bindingIndex;
            glEnableVertexArrayAttrib(m_texturedVAO, attributeIndex);
            glVertexArrayVertexBuffer(m_texturedVAO, bindingIndex, m_texturedVBO, 0,
                static_cast<GLsizei>(sizeof(TexturedVertexData)));
            glVertexArrayAttribFormat(m_texturedVAO, attributeIndex, 1, GL_FLOAT, GL_FALSE, // if i don't set it to a float, my numbers get funky
                static_cast<GLintptr>(sizeof(glm::vec2) + sizeof(glm::vec4) + sizeof(glm::vec2))); // offset by vert pos, color and texture coords
            glVertexArrayAttribBinding(m_texturedVAO, attributeIndex, bindingIndex);

            // Bind the element array to the vert array
            CreateIndexBufferObject();
            glVertexArrayElementBuffer(m_texturedVAO, m_indexBufferObject);

            std::string vboName{ "Textured VBO" };
            GLsizei vboNameLength{ static_cast<GLsizei>(vboName.length()) };
            glObjectLabel(GL_BUFFER, m_texturedVBO, vboNameLength, vboName.c_str());

            std::string vaoName{ "Textured VAO" };
            GLsizei vaoNameLength{ static_cast<GLsizei>(vaoName.length()) };
            glObjectLabel(GL_VERTEX_ARRAY, m_texturedVAO, vaoNameLength, vaoName.c_str());

            // Break the binding
            UnbindAllVAO();
        }
        

        void BatchRenderer::CreateUntexturedVAO()
        {
            //@CHECK krystal
            if (m_untexturedBufferCreated) { return; }
            m_untexturedBufferCreated = true;

            // Create buffer object for vertex data
            glCreateBuffers(1, &m_untexturedVBO);
            glNamedBufferStorage(m_untexturedVBO,
                static_cast<GLsizeiptr>(m_maxVertices * sizeof(VertexDataBase)),
                nullptr, GL_DYNAMIC_STORAGE_BIT);

            // Create VAO
            glCreateVertexArrays(1, &m_untexturedVAO);

            // Bind the positions to attrib 0
            GLuint attributeIndex{ 0 }, bindingIndex{ 0 };

            glEnableVertexArrayAttrib(m_untexturedVAO, attributeIndex);
            glVertexArrayVertexBuffer(m_untexturedVAO, bindingIndex, m_untexturedVBO, 0,
                static_cast<GLsizei>(sizeof(VertexDataBase)));
            glVertexArrayAttribFormat(m_untexturedVAO, attributeIndex, 2, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(m_untexturedVAO, attributeIndex, bindingIndex);

            // Bind the colors to attrib 1
            ++attributeIndex, ++bindingIndex;
            glEnableVertexArrayAttrib(m_untexturedVAO, attributeIndex);
            glVertexArrayVertexBuffer(m_untexturedVAO, bindingIndex, m_untexturedVBO, 0,
                static_cast<GLsizei>(sizeof(VertexDataBase)));
            glVertexArrayAttribFormat(m_untexturedVAO, attributeIndex, 4, GL_FLOAT, GL_FALSE,
                static_cast<GLuint>(sizeof(glm::vec2)));	// offset by vert pos
            glVertexArrayAttribBinding(m_untexturedVAO, attributeIndex, bindingIndex);

            // Bind the element array to the vert array
            CreateIndexBufferObject();
            glVertexArrayElementBuffer(m_untexturedVAO, m_indexBufferObject);

            std::string vboName{ "Untextured VBO" };
            GLsizei vboNameLength{ static_cast<GLsizei>(vboName.length()) };
            glObjectLabel(GL_BUFFER, m_untexturedVBO, vboNameLength, vboName.c_str());

            std::string vaoName{ "Untextured VAO" };
            GLsizei vaoNameLength{ static_cast<GLsizei>(vaoName.length()) };
            glObjectLabel(GL_VERTEX_ARRAY, m_untexturedVAO, vaoNameLength, vaoName.c_str());

            // Break the binding
            UnbindAllVAO();
        }

        
        void BatchRenderer::CreateIndexBufferObject()
        {
            if (m_indexBufferCreated) { return; }
            m_indexBufferCreated = true;

            glCreateBuffers(1, &m_indexBufferObject);
            glNamedBufferStorage(m_indexBufferObject,
                static_cast<GLsizeiptr>(m_maxVertices * sizeof(GLushort)),
                nullptr, GL_DYNAMIC_STORAGE_BIT);

            std::string objectName{ "Index Buffer Object" };
            GLsizei nameLength{ static_cast<GLsizei>(objectName.length()) };
            glObjectLabel(GL_BUFFER, m_indexBufferObject, nameLength, objectName.c_str());
        }


        void BatchRenderer::UpdateTexturedVAO()
        {
            //@CHECK krystal
            
            // Bind the buffer and VAO
            BindTexturedVAO();

            /*
            glNamedBufferSubData(	GLuint buffer,
                GLintptr offset,
                GLsizei size,
                const void *data);


            TexturedVertexData(glm::vec2 const& r_position, glm::vec2 const& r_color,
                glm::vec2 const& r_textureCoord, float const bindingPoint)
            */
            m_texturedVertices.resize(m_maxVertices, TexturedVertexData{
                glm::vec2{}, glm::vec4{}, glm::vec2{}, 0.f });
            m_texturedIndices.resize(m_maxVertices, restartIndex);

            // Update the buffer data of the VBO
            //glBindBuffer(GL_ARRAY_BUFFER, m_texturedVBO);
            glNamedBufferSubData(m_texturedVBO, static_cast<GLintptr>(0), 
                static_cast<GLsizei>(m_texturedVertices.size() * sizeof(TexturedVertexData)), m_texturedVertices.data());
            //glNamedBufferData(m_texturedVBO, static_cast<GLsizeiptr>(m_texturedVertices.size() * sizeof(TexturedVertexData)),
            //    m_texturedVertices.data(), GL_DYNAMIC_DRAW);

            // Update the index buffer data of the EBO
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);
            glNamedBufferSubData(m_indexBufferObject, static_cast<GLintptr>(0),
                static_cast<GLsizei>(m_texturedIndices.size() * sizeof(GLushort)), m_texturedIndices.data());
            //glNamedBufferData(m_indexBufferObject, static_cast<GLsizeiptr>(m_texturedIndices.size() * sizeof(GLushort)),
            //    m_texturedIndices.data(), GL_DYNAMIC_DRAW);

            // Unbind the VAO
            UnbindAllVAO();
        }


        void BatchRenderer::UpdateUntexturedVAO()
        {
            //@CHECK krystal
            
            // Bind the buffer and VAO
            BindTexturedVAO();

            /*
            glNamedBufferSubData(	GLuint buffer,
                GLintptr offset,
                GLsizei size,
                const void *data);
            */

            // Update the sub buffer data of the VBO
            glBindBuffer(GL_ARRAY_BUFFER, m_untexturedVBO);
            glNamedBufferSubData(m_untexturedVBO, static_cast<GLintptr>(0),
                static_cast<GLsizei>(m_untexturedVertices.size()), m_untexturedVertices.data());

            // Update the index buffer data of the EBO
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);
            glNamedBufferSubData(m_indexBufferObject, static_cast<GLintptr>(0),
                static_cast<GLsizei>(m_untexturedIndices.size()), m_untexturedIndices.data());

            // Unbind the VAO
            UnbindAllVAO();
        }


        void BatchRenderer::BindTexturedVAO()
        {
            glBindVertexArray(m_texturedVAO);
        }


        void BatchRenderer::BindUntexturedVAO()
        {
            glBindVertexArray(m_untexturedVAO);
        }


        void BatchRenderer::UnbindAllVAO()
        {
            glBindVertexArray(0);
            //glBindBuffer(GL_ARRAY_BUFFER, 0);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        //void BatchRenderer::ConfigureVAOAttribute(GLuint VAOID, GLuint VBOID, GLuint attributeIndex, GLuint bindingIndex, GLintptr VBOoffset,
        //    GLsizei stride, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)
        //{
        //    glEnableVertexArrayAttrib(VAOID, attributeIndex);
        //    glVertexArrayVertexBuffer(VAOID, bindingIndex, VBOID, VBOoffset, stride);
        //    glVertexArrayAttribFormat(VAOID, attributeIndex, size, type, normalized, relativeoffset);
        //    glVertexArrayAttribBinding(VAOID, attributeIndex, bindingIndex);
        //}

    } // End of Graphics namespace
} // End of PE namespace