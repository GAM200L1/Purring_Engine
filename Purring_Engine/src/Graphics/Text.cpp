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
    bool Font::Initialize(const std::string& r_fontPath, unsigned int fontSize)
    {
        Load(r_fontPath, fontSize);

        glGenVertexArrays(1, &m_vertexArrayObject);
        glGenBuffers(1, &m_vertexBufferObject);
        glBindVertexArray(m_vertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        return true;
    }

    bool Font::Load(const std::string& r_fontPath, unsigned int fontSize)
    {
        Characters.clear();
        
        // Initialize freetype library
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return false;
        }

        // Load font as face
        FT_Face face;
        if (FT_New_Face(ft, r_fontPath.c_str(), 0, &face))
        {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
            return false;
        }
        
        // Define pixel font size to extract
        FT_Set_Pixel_Sizes(face, 0, fontSize);

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

        // Clear freetype resource
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        return true;
    }

    void TextComponent::SetFont(std::string fontKey)
    {
        m_fontKey = fontKey;
        m_font = ResourceManager::GetInstance().Fonts[fontKey];
    }

    void TextComponent::SetText(std::string const& r_text)
    {
        m_text = r_text;
    }

    void TextComponent::SetSize(float size)
    {
        m_size = size;
    }

    void TextComponent::SetColor(float const r, float const g, float const b, float const a)
    {
        m_color = glm::vec4{ glm::clamp(r, 0.f, 1.f), glm::clamp(g, 0.f, 1.f), glm::clamp(b, 0.f, 1.f), glm::clamp(a, 0.f, 1.f) };
    }
}
