/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Text.h
 \date     23-09-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the declarations of functions and classes required for
           rendering text.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include "Text.h"
#include "ResourceManager/ResourceManager.h"

namespace PE
{
    void Font::Init(std::shared_ptr<Graphics::ShaderProgram> shader)
    {
        TextShader = shader;
        Load("../Assets/fonts/KernlGrotesk.otf");

        glGenVertexArrays(1, &mVao);
        glGenBuffers(1, &mVbo);
        glBindVertexArray(mVao);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Font::Load(const std::string& fontpath, unsigned int fontsize)
    {
        Characters.clear();
        
        // Initialize freetype library
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        }

        // Load font as face
        FT_Face face;
        if (FT_New_Face(ft, fontpath.c_str(), 0, &face))
        {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        }
        
        // Define pixel font size to extract
        FT_Set_Pixel_Sizes(face, 0, fontsize);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 ASCII characters for now
        for (GLubyte ch = 0; ch < 128; ch++)
        {
            // load character glyph 
            if (FT_Load_Char(face, ch, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }

            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };

            Characters.insert(std::pair<char, Character>(ch, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void Font::RenderText(const std::string& text, glm::vec2 position, float scale, Graphics::Camera& camera, glm::vec3 color)
    {
        // activate corresponding render state	
        TextShader->Use();
        TextShader->SetUniform("u_ViewProjection", camera.GetWorldToViewMatrix());
        TextShader->SetUniform("textColor", color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(mVao);

        // iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = Characters[*c];
            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { position.x,     position.y + h,   0.0f, 0.0f },
                { position.x ,    position.y,       0.0f, 1.0f },
                { position.x + w, position.y,       1.0f, 1.0f },

                { position.x,     position.y + h,   0.0f, 0.0f },
                { position.x + w, position.y ,      1.0f, 1.0f },
                { position.x + w, position.y + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, mVbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph
            position.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

    }
}