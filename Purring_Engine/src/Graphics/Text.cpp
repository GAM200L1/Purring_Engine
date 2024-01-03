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
        if (!Load(r_fontPath, fontSize))
        {
            return false;;
        }

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

        m_lineHeight = static_cast<float>(face->size->metrics.height >> 6) * 0.7f;

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
        m_font = ResourceManager::GetInstance().GetFont(fontKey);
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

    void TextComponent::SetAlpha(float const alpha)
    {
        m_color.a = alpha;
    }

    void TextComponent::SetHAlignment(TextAlignment hAlignment)
    {
        m_hAlignment = hAlignment;
    }

    void TextComponent::SetVAlignment(TextAlignment vAlignment)
    {
        m_vAlignment = vAlignment;
    }

    void TextComponent::SetHOverflow(TextOverflow hOverflow)
    {
        m_hOverflow = hOverflow;
    }

    void TextComponent::SetVOverflow(TextOverflow vOverflow)
    {
        m_vOverflow = vOverflow;
    }

    void TextComponent::SetLineSpacing(float lineSpacing)
    {
        m_lineSpacing = lineSpacing;
    }

    std::vector<std::string> SplitTextIntoLines(TextComponent const& r_textComponent, TextBox textBox)
    {
        std::vector<std::string> lines;
        std::string text{ r_textComponent.GetText() };
        std::string currentLine;

        float currentLineWidth{ 0.f }; // width of current line

        for (char c : text)
        {
            // new line
            if (c == '\n')
            {
				lines.emplace_back(currentLine);
				currentLine.clear();
				currentLineWidth = 0.f;
				continue;
			}

            if (r_textComponent.GetHOverflow() == TextOverflow::WRAP)
            {
                Character ch = r_textComponent.GetFont()->Characters.at(c);
                currentLineWidth += (ch.Advance >> 6) * r_textComponent.GetSize();

                if (currentLineWidth > textBox.width)
                {
                    // find last space
                    size_t lastSpace = currentLine.find_last_of(' ');

                    // get last word
                    if (lastSpace != std::string::npos)
                    {
                        // add line without last word
                        lines.emplace_back(currentLine.substr(0, lastSpace));

                        // add last word to next line
                        currentLine = currentLine.substr(lastSpace + 1, currentLine.size() - lastSpace - 1);
                        currentLineWidth = CalculateLineWidth(r_textComponent, currentLine);
                    }
                    // no space found, wrap word
                    else
                    {
                        lines.emplace_back(currentLine);
                        currentLine.clear();
                        currentLineWidth = 0.f;
                    }
                }
            }

            currentLine += c;
        }

        // add last line
        if (!currentLine.empty())
        {
			lines.emplace_back(currentLine);
		}

        // handle vertical overflow
        if (r_textComponent.GetVOverflow() == TextOverflow::TRUNCATE)
        {
			// get text height, first line is not included
            std::vector<std::string> truncatedLines;
            float textHeight{ 0.f };

			// if text height is greater than text box height
            for (std::string const& line : lines)
            {
                // add line height to text height
                textHeight += r_textComponent.GetFont()->m_lineHeight * r_textComponent.GetLineSpacing() * r_textComponent.GetSize();

                // if text height exceeds, truncate text
                if (textHeight > textBox.height)
                {
                    lines = truncatedLines;
                    break;
                }

                // add line to truncated lines
                truncatedLines.emplace_back(line);
            }
		}

        return lines;
    }

    void HorizontalTextAlignment(TextComponent const& r_textComponent, std::string const& line, TextBox textBox, float& hAlignOffset)
    {
        // get line width
        float lineWidth{ CalculateLineWidth(r_textComponent, line) };

        switch (r_textComponent.GetHAlignment())
        {
        case TextAlignment::LEFT:
            hAlignOffset = -textBox.width / 2.f;
            break;
        case TextAlignment::CENTER:
            hAlignOffset = -lineWidth / 2.f;
            break;
        case TextAlignment::RIGHT:
            hAlignOffset = (textBox.width / 2.f) - lineWidth;
            break;
        }
    }

    void VerticalTextAlignment(TextComponent const& r_textComponent, std::vector<std::string> const& lines, TextBox textBox, float& vAlignOffset)
    {
        // get text height, first line is not included
        float textHeight = (lines.size() - 1) * (r_textComponent.GetFont()->m_lineHeight * r_textComponent.GetLineSpacing() * r_textComponent.GetSize());

        switch (r_textComponent.GetVAlignment())
        {
        case TextAlignment::TOP:
            vAlignOffset = textBox.height / 2.f - r_textComponent.GetFont()->m_lineHeight * r_textComponent.GetSize();
            break;
        case TextAlignment::CENTER:
            vAlignOffset = (textHeight - r_textComponent.GetFont()->m_lineHeight * r_textComponent.GetSize()) / 2.f;
            break;
        case TextAlignment::BOTTOM:
            vAlignOffset = - (textBox.height / 2.f) + textHeight;
            break;
        }
    }

    float CalculateLineWidth(TextComponent const& r_textComponent, std::string const& line)
    {
        float lineWidth{ 0.f };

        // get the width of each character and add them to line width
        for (char c : line)
        {
            Character ch = r_textComponent.GetFont()->Characters.at(c);
            lineWidth += (ch.Advance >> 6) * r_textComponent.GetSize();
        }

        return lineWidth;
    }
}
